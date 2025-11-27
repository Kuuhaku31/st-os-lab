
// process_sync_sem.c

#include <semaphore.h>
#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_KEY 0x666
#define DATA_SIZE 10


// 定义共享数据结构
struct shareData
{
    sem_t sem_w; // 信号量
    sem_t sem_r;
    int   data[DATA_SIZE];
};


void
printData(const struct shareData* data)
{
    for(int i = 0; i < DATA_SIZE; i++)
    {
        printf("data[%d]: %d\n", i, data->data[i]); // 打印 data
    }
}


int
main()
{
    printf("process_sync\n");

    //=================== shmget：创建/获取共享内存段（shm） ===================
    // shm（shared memory）是一块由内核分配的“公共内存”
    // shmget 不会让进程直接访问，只是创建/获取共享内存段，并返回一个 shm_id
    const int shm_id = shmget(SHM_KEY, sizeof(struct shareData), 0666 | IPC_CREAT);
    if(shm_id < 0)
    {
        perror("shmget failed");
        return -1;
    }

    //=================== shmat：attach（附加/映射） ===================
    // shmat 作用：把共享内存映射到当前进程的虚拟地址空间
    // 返回值是一个普通指针，进程可以像访问普通数组一样访问共享内存
    void* shm = shmat(shm_id, NULL, 0);
    if(shm == (void*)-1)
    {
        perror("shmat failed");
        return -1;
    }

    printf("SHM Key:    %d\n", SHM_KEY);
    printf("共享内存ID: %d\n", shm_id);
    printf("地址位于:   %p\n", shm);

    // 映射后的共享内存可通过结构体访问
    struct shareData* shareData = shm;

    // 初始化数据
    for(int i = 0; i < DATA_SIZE; i++) shareData->data[i] = 0;
    printf("Data 初始状况:\n");
    printData(shareData);

    // 初始化信号量
    sem_init(&shareData->sem_w, 1, 1); // 写信号量初始化值为1，确保在数据读好之前，写操作不会被允许
    sem_init(&shareData->sem_r, 1, 0); // 读信号量初始化值为0，确保在没有数据可读之前，读取操作不会被允许

    // 创建子进程：父进程写，子进程读
    const int pid = fork();
    if(pid > 0) // 父进程：写入共享内存
    {
        printf("父进程：写入共享内存\n");
        sleep(2); // 父进程等待几秒

        // 尝试占有写信号量
        sem_wait(&shareData->sem_w);

        printf("父进程开始发送数据:\n");
        for(int i = 0; i < DATA_SIZE; i++)
        {
            shareData->data[i] = i + 1;
            printf("父进程发送数据[%d]: %d\n", i, shareData->data[i]);
            sleep(1);
        }
        printf("父进程发送完成\n");

        // 释放读信号量
        sem_post(&shareData->sem_r);

        // 回收内存
        shmdt(shm);                     // shmdt：detach（解除映射）
        shmctl(shm_id, IPC_RMID, NULL); // shmctl(IPC_RMID)：删除共享内存段（但真实删除需等待所有进程 detach 后）

        sleep(2);
        printf("父进程结束\n");
    }
    else if(pid == 0) // 子进程：读取共享内存
    {
        printf("子进程：读取共享内存\n");

        // 尝试占有读信号量
        sem_wait(&shareData->sem_r);

        printf("子进程接收数据:\n");
        printData(shareData);
        printf("子进程接收数据完成\n");

        // 释放写信号量
        sem_post(&(shareData->sem_w));

        shmdt(shm); // 子进程也需要 detach
        sleep(1);
        printf("子进程结束\n");
    }
    else
    {
        perror("fork failed");
        return -1;
    }

    return 0;
}
