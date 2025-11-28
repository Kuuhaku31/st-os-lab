#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/rculist.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/delay.h>

/**
 * struct book - a book
 *
 * @borrow:	If it is 0, book is not borrowed. it is 1, book is borrowed.
 */
struct book {
	int id;
	int borrow;
	char name[64];
	char author[64];
	struct list_head node;
	struct rcu_head rcu;
};

#define RCU_SYNC 0
#define RCU_ASYNC 1

#define MANAGER_COUNT 2
#define STUDENT_COUNT 10
static struct task_struct *tsk_manager[MANAGER_COUNT];
static struct task_struct *tsk_student[STUDENT_COUNT];

#define BOOK_COUNT    10
#define INTERVAL	  1000

static LIST_HEAD(books);
static spinlock_t books_lock;
static int on_duty = 1;

/**
 * callback function for async-reclaim
 *
 * call_rcu() 		:  callback function is called when finish to wait every grace periods (async)
 * synchronize_rcu() :  wait to finish every grace periods (sync)
*/
static void book_reclaim_callback(struct rcu_head *rcu)
{
	struct book *b = container_of(rcu, struct book, rcu);

	/**
	 * Why print preemt_count??
	 *
	 * To check whether this callback is atomic context or not.
	 * preempt_count here is more than 0. Because it is irq context.
	*/
	pr_err("callback free : %lx, preempt_count : %d\n", (unsigned long)b, preempt_count());
	kfree(b);
}

static void add_book(int id, const char *name, const char *author)
{
	struct book *b;

	b = kzalloc(sizeof(struct book), GFP_KERNEL);
	if(!b)
		return;

	printk("%s, id:%d\n", __FUNCTION__, id);
	b->id = id;
	strncpy(b->name, name, sizeof(b->name));
	strncpy(b->author, author, sizeof(b->author));
	b->borrow = 0;

	/**
	 * list_add_rcu
	 *
	 * add_node(writer - add) use spin_lock()
	*/
	spin_lock(&books_lock);
	list_add_rcu(&b->node, &books);
	spin_unlock(&books_lock);
}

static int borrow_book(int id, int async)
{
	struct book *b = NULL;
	struct book *new_b = NULL;
	struct book *old_b = NULL;

	/**
	 * updater
	 *
	 * (updater) require that alloc new node & copy, update new node & reclaim old node
	 * list_replace_rcu() is used to do that.
	*/
	rcu_read_lock();

	list_for_each_entry(b, &books, node) {
		if(b->id == id) {
			if(b->borrow) {
				pr_err("borrow failed %d, not available\n", id);
				rcu_read_unlock();
				return -1;
			}

			old_b = b;
			break;
		}
	}

	if(!old_b) {
		pr_err("borrow failed %d, no such book\n", id);
		rcu_read_unlock();
		return -1;
	}

	new_b = kzalloc(sizeof(struct book), GFP_ATOMIC);
	if(!new_b) {
		rcu_read_unlock();
		return -1;
	}

	pr_err("%s,%s, old_b: %pS, new_b:%pS", __FUNCTION__,current->comm, old_b, new_b);
	memcpy(new_b, old_b, sizeof(struct book));
	new_b->borrow = 1;
	
	spin_lock(&books_lock);
	list_replace_rcu(&old_b->node, &new_b->node);
	spin_unlock(&books_lock);

	rcu_read_unlock();

	if(async) {
		call_rcu(&old_b->rcu, book_reclaim_callback);
	}else {
		synchronize_rcu();
		kfree(old_b);
	}

	pr_err("borrow success %d\n", id);
	return 0;
}

static int return_book(int id, int async)
{
	struct book *b = NULL;
	struct book *new_b = NULL;
	struct book *old_b = NULL;

	/**
	 * updater
	 *
	 * (updater) require that alloc new node & copy, update new node & reclaim old node
	 * list_replace_rcu() is used to do that.
	*/
	rcu_read_lock();

	list_for_each_entry(b, &books, node) {
		if(b->id == id) {
			if(!b->borrow) {
				rcu_read_unlock();
				pr_err("return failed %d, not available\n", id);
				return -1;
			}

			old_b = b;
			break;
		}
	}

	if(!old_b) {
		pr_err("return failed %d, no such book\n", id);
		rcu_read_unlock();
		return -1;
	}

	new_b = kzalloc(sizeof(struct book), GFP_ATOMIC);
	if(!new_b) {
		rcu_read_unlock();
		return -1;
	}

	pr_err("%s,%s, old_b: %pS, new_b:%pS", __FUNCTION__,current->comm, old_b, new_b);
	memcpy(new_b, old_b, sizeof(struct book));
	new_b->borrow = 0;
	
	spin_lock(&books_lock);
	list_replace_rcu(&old_b->node, &new_b->node);
	spin_unlock(&books_lock);

	rcu_read_unlock();

	if(async) {
		call_rcu(&old_b->rcu, book_reclaim_callback);
	}else {
		synchronize_rcu();
		kfree(old_b);
	}

	pr_err("return success %d\n", id);
	return 0;
}

static void delete_book(int id, int async)
{
	struct book *b;

	spin_lock(&books_lock);
	list_for_each_entry(b, &books, node) {
		if(b->id == id) {
			/**
			 * list_del
			 *
			 * del_node(writer - delete) require locking mechanism.
			 * we can choose 3 ways to lock. Use 'a' here.
			 *
			 *	a.	locking,
			 *	b.	atomic operations, or
			 *	c.	restricting updates to a single task.
			*/
			list_del_rcu(&b->node);
			spin_unlock(&books_lock);

			if(async) {
				call_rcu(&b->rcu, book_reclaim_callback);
			}else {
				synchronize_rcu();
				kfree(b);
			}
			return;
		}
	}
	spin_unlock(&books_lock);

	pr_err("not exist book\n");
}

static void print_book(int id)
{
	struct book *b;

	printk("%s, id:%d\n", __FUNCTION__, id);
	rcu_read_lock();
	list_for_each_entry_rcu(b, &books, node) {
		if(b->id == id) {
			/**
			 * Why print address of "struct book *b"??
			 *
			 * If b was updated, address of b must be different.
			 * We can know whether b is updated or not by address.
			*/
			//pr_err("id : %d, name : %s, author : %s, borrow : %d, addr : %lx\n", \
			//			b->id, b->name, b->author, b->borrow, (unsigned long)b);
			rcu_read_unlock();
			return;
		}
	}
	rcu_read_unlock();

	pr_err("not exist book\n");
}

static int kthread_lab_manager(void *data)
{
	struct task_struct *tsk = current;
	int i;

	pr_err("%s(%d) begins!!!\n", tsk->comm, tsk->pid);

	while (on_duty) {
		//todo: add randomly, delete randomly
		pr_err("%s(%d) is going to print books!!!\n", tsk->comm, tsk->pid);
		for (i = 0; i < BOOK_COUNT; i++)
			print_book(i);

		msleep(INTERVAL);
	}

	pr_err("%s(%d) off duty!!!\n", tsk->comm, tsk->pid);
	return 0;
}

static int kthread_student(void *data)
{
	struct task_struct *tsk = current;
	unsigned int id;

	pr_err("%s(%d) begins!!!\n", tsk->comm, tsk->pid);

	while (on_duty) {
		get_random_bytes(&id, sizeof(unsigned int));
		id %= BOOK_COUNT;

		pr_err("%s(%d) is going to borrow book %d\n", tsk->comm, tsk->pid, id);
		if (borrow_book((int)id, RCU_ASYNC) == 0) {
			msleep(INTERVAL);
			pr_err("%s(%d) is going to return book %d\n", tsk->comm, tsk->pid, id);
			return_book((int)id, RCU_ASYNC);
		}
		msleep(INTERVAL);
	}
	
	pr_err("%s(%d) off duty!!!\n", tsk->comm, tsk->pid);
	return 0;
}

static int kylin_rcu_init(void)
{
	int i;
	char comm[16], book_name[16];

	pr_err("%s", __FUNCTION__);
	spin_lock_init(&books_lock);

	/*create books*/
	for (i = 0; i < BOOK_COUNT; i++) {
		sprintf(book_name, "KylinOS-lab%d", i);
		add_book(i, book_name, "kylin edu");
	}

	/*create lab manager threads*/
	for (i = 0; i < MANAGER_COUNT; i++) {
		sprintf(comm, "lab-manager%d", i);
	    tsk_manager[i] = kthread_run(kthread_lab_manager, NULL, comm);
	}

	/*create student threads*/
	for (i = 0; i < STUDENT_COUNT; i++) {
		sprintf(comm, "student%d", i);
	    tsk_student[i] = kthread_run(kthread_student, NULL, comm);
	}

	return 0;
}

static void kylin_rcu_exit(void)
{
	int i;

	pr_err("%s", __FUNCTION__);

	on_duty = 0;
	msleep(INTERVAL);
	/*delete books*/
	for (i = 0; i < BOOK_COUNT; i++)
		delete_book(i, RCU_SYNC);

	return;
}

module_init(kylin_rcu_init);
module_exit(kylin_rcu_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kylin edu");
