// Library


#include <stdarg.h>
#include <stdio.h>
#include <fcntl.h>    // open、O_RDONLY 等
#include <unistd.h>   // close 等
#include <string.h>
#include <semaphore.h>
#include <linux/slab.h>
#include <linux/kernel.h>        // printk, pr_err, container_of
#include <linux/module.h>        // module_init, module_exit, THIS_MODULE
#include <linux/init.h>          // __init, __exit
#include <linux/list.h>          // list_head, list_for_each_entry, list_replace_rcu
#include <linux/rcupdate.h>      // rcu_read_lock, synchronize_rcu, call_rcu
#include <linux/spinlock.h>      // spinlock_t, spin_lock(), spin_unlock()
#include <linux/kthread.h>       // kthread_run, kthread_should_stop
#include <linux/sched.h>         // current, task_struct
#include <linux/delay.h>         // msleep
#include <linux/random.h>        // get_random_bytes
#include <linux/printk.h>        // pr_err, pr_info（有些系统不需要）
#include <linux/sched.h>


#define RCU_SYNC      0
#define RCU_ASYNC     1
#define MANAGER_COUNT 2  // 图书管理员线程的数量
#define STUDENT_COUNT 10 // 学生线程的数量
#define BOOK_COUNT    10 // 图书的数量
#define INTERVAL      1000

static struct task_struct* tsk_manager[MANAGER_COUNT];
static struct task_struct* tsk_student[STUDENT_COUNT];

static int  RCUInit();
static int  ThreadLabManager(void* data);
static int  ThreadStudent(void* data);
static void AddBook(int id, const char* name, const char* author);
static void print_book(int id);
static int  borrow_book(int id, int async);
static void book_reclaim_callback(struct rcu_head* rcu);
static int  return_book(int id, int async);

// 图书资源
struct book
{
    int id;
    int borrow; // 0：未借阅；1：已借阅
    char name[64];
    char author[64];

    struct list_head node;
    struct rcu_head rcu;
};

static int
RCUInit()
{
    int i;
    char comm[16], book_name[16];
    pr_err("%s", __FUNCTION__);
    spin_lock_init(&books_lock);
    /*create books*/
    for(i = 0; i < BOOK_COUNT; i++)
    {
        sprintf(book_name, "KylinOS-lab%d", i);
        AddBook(i, book_name, "kylin edu");
    }
    /*create lab manager threads*/
    for(i = 0; i < MANAGER_COUNT; i++)
    {
        sprintf(comm, "lab-manager%d", i);
        tsk_manager[i] = kthread_run(ThreadLabManager, NULL, comm);
    }
    /*create student threads*/
    for(i = 0; i < STUDENT_COUNT; i++)
    {
        sprintf(comm, "student%d", i);
        tsk_student[i] = kthread_run(ThreadStudent, NULL, comm);
    }
    return 0;
}


// 图书管理员线程
// 每隔一秒遍历并且打印图书列表
static int
ThreadLabManager(void* data)
{
    // 内核的一个全局变量，是当前线程的 task_struct 指针
    // 用这个指针来获取图书管理员线程的名称和 pid 用于打印信息
    struct task_struct* tsk = current;

    pr_err("%s(%d) begins!!!\n", tsk->comm, tsk->pid);
    while(!kthread_should_stop())
    {
        // todo: add randomly, delete randomly
        pr_err("%s(%d) is going to print books!!!\n", tsk->comm, tsk->pid);
        for(int i = 0; i < BOOK_COUNT; i++)
            print_book(i);
        msleep(INTERVAL);
    }
    pr_err("%s(%d) off duty!!!\n", tsk->comm, tsk->pid);
    return 0;
}


// 学生线程
// (1)调用内核函数get_random_bytes 获取一个随机数，并除以BOOK_COUNT取余，作为该学生本次要借阅图书的id。
// (2)调用borrow_book 借阅图书。
// (3)如果借阅成功的话，休眠一秒钟，然后调用return_book归还图书。
static int
ThreadStudent(void* data)
{
    struct task_struct* tsk = current;
    unsigned int id;
    pr_err("%s(%d) begins!!!\n", tsk->comm, tsk->pid);
    while(!kthread_should_stop())
    {
        get_random_bytes(&id, sizeof(unsigned int));
        id %= BOOK_COUNT;
        pr_err("%s(%d) is going to borrow book %d\n", tsk->comm, tsk->pid, id);
        if(borrow_book((int)id, RCU_ASYNC) == 0)
        {
            msleep(INTERVAL);
            pr_err("%s(%d) is going to return book %d\n", tsk->comm, tsk->pid, id);
            return_book((int)id, RCU_ASYNC);
        }
        msleep(INTERVAL);
    }
    pr_err("%s(%d) off duty!!!\n", tsk->comm, tsk->pid);
    return 0;
}


// 创建图书
// (1)调用kzalloc 在 slab 里分配内存，其中kzalloc相当于先调用kmalloc分配内存，然后再调用memset将内存块清零。
// (2)book_lock 是一个自旋锁，RCU 不能保证多个写者的数据安全，所以需要在多个写者的情况下，需要使用自旋锁保护临界资源。
// (3)调用list_add_rcu 将创建的图书资源放到链表中，list_add_rcu会等所有读者都退出临界区之后才更新链表。
static void
AddBook(int id, const char* name, const char* author)
{
    struct book* b;
    b = kzalloc(sizeof(struct book), GFP_KERNEL);
    if(!b) return;
    printk("%s, id:%d\n", __FUNCTION__, id);
    b->id = id;
    strncpy(b->name, name, sizeof(b->name));
    strncpy(b->author, author, sizeof(b->author));
    b->borrow = 0;
    /**
     * list_add_rcu
     * add_node(writer - add) use spin_lock()
    */
    spin_lock(&books_lock);
    list_add_rcu(&b->node, &books);
    spin_unlock(&books_lock);
}


// 打印图书信息
// (1)图书管理员是一个读者线程，它只是从链表中读取数据，读者线程会使用 rcu_read_lock 标识进入临界区，这个锁不会阻止其他的读者和写者进入临界区，但当写者尝试修改数据的时 候，内核的RCU管理模块会等到所有读者都退出临界区之后才修改数据，以保证数据的完整性。
// (2)遍历图书链表的数据。
// (3)打印图书信息。
// (4)调用rcu_read_unlock 标识本线程退出临界区，如果此时有写者修改数据的话，会等到所有读者都退出临界区才进行修改。
static void
print_book(int id)
{
    struct book* b;

    rcu_read_lock();
    list_for_each_entry_rcu(b, &books, node)
    {
        if(b->id == id)
        {
            pr_err("id : %d, name : %s, author : %s, borrow : %d, addr : %lx\n",
                b->id, b->name, b->author, b->borrow, (unsigned long)b);
            rcu_read_unlock();
            return;
        }
    }
    rcu_read_unlock();

    pr_err("not exist book\n");
}


// (1)学生线程是一个写者线程，为了防止多个写者共同修改同一个数据，需要一个额外的锁 进行保护，我在调试这段代码的时候，一开始忽视了这个问题，结果导致了内核crash，我在本 实验的后面部分会对这个问题进行分析，这也是一个很好的经验分享。
// (2)如果这本书已经被借走了，则退出。
// (3)调用list_replace_rcu 通知 RCU 修改数据，如果此时没有读者线程在临界区，则立即修改，如果有读者线程，则需要等待它们退出临界区。
// (4)修改数据后，写者线程有两个选择，同步方式和异步方式，异步方式是调用call_rcu注
// 册一个回调函数来处理旧的数据，比如释放旧数据的内存，本实验的回调函数是
// book_reclaim_callback，稍后看一下它的代码。
// (5)同步方式是调用synchronize_rcu，写者线程休眠等待，直到所有读者退出临界区之后才
// 被唤醒，然后释放旧数据的内存。
// (6)list_for_each_entry:被预定义为一个for循环语句，for循环的第一句获取(head)->next指向
// 的member成员的结构体指针，将pos初始化为链表中出链表头之外的第一个实体链表成员，for
// 的第三句通过pos->member.next指针遍历整个实体链表，当pos->member.next再次指向链表头
// 的时候，说明已经遍历完毕，退出循环。
static int
borrow_book(int id, int async)
{
    struct book* b = NULL;
    struct book* new_b = NULL;
    struct book* old_b = NULL;
    spin_lock(&books_lock);
    list_for_each_entry(b, &books, node)
    {
        if(b->id == id)
        {
            if(b->borrow)
            {
                //此书已经被借阅出去了
                pr_err("borrow failed %d, not available\n", id);
                goto err;
            }
            old_b = b;
            break;
        }
    }
    if(!old_b)
    {
        pr_err("borrow failed %d, no such book\n", id);
        goto err;
    }
    new_b = kzalloc(sizeof(struct book), GFP_ATOMIC);
    if(!new_b)
        goto err;
    pr_err("%s,%s, old_b: %pS, new_b:%pS", __FUNCTION__, current->comm, old_b, new_b);
    old_b->borrow = 1;
    memcpy(new_b, old_b, sizeof(struct book));
    list_replace_rcu(&old_b->node, &new_b->node);
    spin_unlock(&books_lock);
    if(async) { call_rcu(&old_b->rcu, book_reclaim_callback); }
    else
    {
        synchronize_rcu();
        kfree(old_b);
    }
    pr_err("borrow success %d\n", id);
    return 0;
err:
    spin_unlock(&books_lock);
    return -1;
}


static void
book_reclaim_callback(struct rcu_head* rcu)
{
    struct
    book* b = container_of(rcu, struct book, rcu);
    pr_err("callback free : %lx, preempt_count : %d\n", (unsigned long)b, preempt_count());
    kfree(b);
}


static int
return_book(int id, int async)
{
    struct book* b = NULL;
    struct book* new_b = NULL;
    struct book* old_b = NULL;
    spin_lock(&books_lock);
    list_for_each_entry(b, &books, node)
    {
        if(b->id == id)
        {
            if(!b->borrow)
            {
                //图书借阅标记为0即未被借阅则不能还书
                pr_err("return failed %d, not available\n", id);
                goto err;
            }
            old_b = b;
            break;
        }
    }
    if(!old_b)
    {
        pr_err("return failed %d, no such book\n", id);
        goto err;
    }
    new_b = kzalloc(sizeof(struct book), GFP_ATOMIC);
    if(!new_b)
        goto err;
    pr_err("%s,%s, old_b: %pS, new_b:%pS", __FUNCTION__, current->comm, old_b, new_b);
    old_b->borrow = 0;
    memcpy(new_b, old_b, sizeof(struct book));
    list_replace_rcu(&old_b->node, &new_b->node);
    spin_unlock(&books_lock);
    if(async) { call_rcu(&old_b->rcu, book_reclaim_callback); }
    else
    {
        synchronize_rcu();
        kfree(old_b);
    }
    pr_err("return success %d\n", id);
    return 0;
err:
    spin_unlock(&books_lock);
    return -1;
}


// 2025-11-27 kuuhaku-kzr