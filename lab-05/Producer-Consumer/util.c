// util

#include "util.h"

#include <stdarg.h>
#include <stdio.h>
#include <fcntl.h>    // open、O_RDONLY 等
#include <sys/mman.h> // mmap()、PROT_READ、PROT_WRITE、MAP_SHARED
#include <unistd.h>   // close 等
#include <string.h>


void
printLog(const char* label, const char* __restrict format, ...)
{
    va_list args;
    va_start(args, format);

    printf("[%s]: ", label);
    vprintf(format, args);
    printf("\n");

    va_end(args);
}

void
printShareData(const People* data)
{
    printf("\t[front: %d rear: %d]\n", data->front, data->rear);
    if(data->front == data->rear) // 如果队列空
    {
        printf("\t[empty]\n");
        return;
    }

    printf("\t[");
    int i = data->front;
    while(i != data->rear) // 遍历循环队列
    {
        printf(PRINT_NAME ", ", data->names[i]);
        i = (i + 1) % (BUFFER_SIZE + 1);
    }
    printf("\b\b]\n");
}

// 获取用户输入，返回 0 表示退出程序
int
getInput(const char* label, char* input_buffer)
{
    // 提示用户输入姓名
    printf("[%s]: ", label);
    fgets(input_buffer, NAME_SIZE, stdin);

    for(int i = 0; i < NAME_SIZE; i++) if(input_buffer[i] == '\n') input_buffer[i] = '\0'; // 去掉 \n
    if(!strcmp(input_buffer, "exit")) return 0;
    if(!strcmp(input_buffer, "quit")) return 0;
    if(!strcmp(input_buffer, "qu")) return 0;

    return 1;
}

int
shareInit(ShareData* sd, const int is_creat)
{
    const int flag = is_creat ? O_CREAT | O_RDWR | O_TRUNC : O_CREAT | O_RDWR;

    // 创建共享内存（放信号量）
    sd->sem_fd = open(SHARE_SEMAPHORE_FILE, flag, 0666); // 信号量文件描述符，这个文件在系统中的句柄（handle）
    ftruncate(sd->sem_fd, sizeof(ShareSemaphore)); // 改变文件的大小
    sd->sem = mmap(NULL, sizeof(ShareSemaphore), PROT_READ | PROT_WRITE, MAP_SHARED, sd->sem_fd, 0); // 将文件映射到进程的虚拟内存空间

    if(sd->sem == MAP_FAILED) return -1;

    // 初始化信号量
    if(is_creat)
    {
        sem_init(&sd->sem->sync, 1, 1);
        sem_init(&sd->sem->full, 1, 0);
        sem_init(&sd->sem->empty, 1, BUFFER_SIZE);
    }

    // 创建共享内存（放共享变量）
    sd->data_fd = open(SHARE_DATA_FILE, flag, 0666);
    ftruncate(sd->data_fd, sizeof(People));
    sd->data = mmap(NULL, sizeof(People), PROT_READ | PROT_WRITE, MAP_SHARED, sd->data_fd, 0);

    if(sd->data == MAP_FAILED) return -1;

    return 0;
}

int
shareCleanup(ShareData* sd)
{
    int flag = 0;

    // 销毁信号量
    flag |= sem_destroy(&sd->sem->sync);
    flag |= sem_destroy(&sd->sem->empty);
    flag |= sem_destroy(&sd->sem->full);

    // 取消地址段映射
    flag |= munmap(sd->data, sizeof(People));
    flag |= munmap(sd->sem, sizeof(ShareSemaphore));

    // 关闭文件
    flag |= close(sd->sem_fd);
    flag |= close(sd->data_fd);

    sd->sem = NULL;
    sd->data = NULL;

    return flag;
}

// 2025-11-26 kuuhaku-kzr