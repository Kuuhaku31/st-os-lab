
// process_sync.c

#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_KEY 0x666
#define DATA_SIZE 10


// 定义共享数据结构
struct shareData
{
    int data[DATA_SIZE];
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

    printf("SHM Key: %d\n", SHM_KEY);
    printf("共享内存ID: %d\n", shm_id);

    //=================== shmat：attach（附加/映射） ===================
    // shmat 作用：把共享内存映射到当前进程的虚拟地址空间
    // 返回值是一个普通指针，进程可以像访问普通数组一样访问共享内存
    void* shm = shmat(shm_id, NULL, 0);
    if(shm == (void*)-1)
    {
        perror("shmat failed");
        return -1;
    }

    // 映射后的共享内存可通过结构体访问
    struct shareData* shareData = shm;

    // 初始化数据
    for(int i = 0; i < DATA_SIZE; i++) shareData->data[i] = 0;
    printf("Data 初始状况:\n");
    printData(shareData);

    //=================== 创建子进程（父进程写，子进程读） ===================
    const int pid = fork();
    if(pid > 0)
    {
        //=================== 父进程：写入共享内存 ===================
        printf("父进程开始发送数据:\n");
        for(int i = 0; i < DATA_SIZE; i++)
        {
            shareData->data[i] = i + 1;
            printf("父进程发送数据[%d]: %d\n", i, shareData->data[i]);
        }
        printf("父进程发送完成\n");

        // 回收内存
        shmdt(shm);                     // shmdt：detach（解除映射）
        shmctl(shm_id, IPC_RMID, NULL); // shmctl(IPC_RMID)：删除共享内存段（但真实删除需等待所有进程 detach 后）

        sleep(2);
        printf("父进程: 已 detach 并删除共享内存\n");
    }
    else if(pid == 0)
    {
        //=================== 子进程：读取共享内存 ===================
        sleep(1); // 确保父进程完成写入

        printf("子进程接收数据:\n");
        printData(shareData);

        printf("子进程结束\n");

        shmdt(shm); // 子进程也需要 detach
    }
    else
    {
        perror("fork failed");
        return -1;
    }

    return 0;
}
