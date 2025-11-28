// util

#ifndef LAB_PAC_H
#define LAB_PAC_H

#include <semaphore.h>


#define BUFFER_SIZE 10 // BUFFER_SIZE 为缓冲池大小，本实验设定大小为10
#define NAME_SIZE   16 // 单次可输入的最大字符数（外加\0）

// #define SHARE_PATH           "/home/lyz/def/shm/"
// #define SHARE_PATH           "/mnt/d/WSL/shm/"
#define SHARE_PATH           "/mnt/hgfs/.share/shm/"
#define SEMAPHORE_FILE       "st-sem"
#define DATA_FILE            "st-data"
#define SHARE_SEMAPHORE_FILE SHARE_PATH SEMAPHORE_FILE
#define SHARE_DATA_FILE      SHARE_PATH DATA_FILE

#define PRODUCER_LABEL "\033[1;31mProducer\033[0m"
#define CONSUMER_LABEL "\033[1;34mConsumer\033[0m"

#define CONCAT(a, b) a ## b
#define PRINT_NAME "'\033[0;32m%s\033[0m'"

// 定义信号量
typedef struct
{
    sem_t sync; // 同步信号量
    sem_t empty; // 空信号量
    sem_t full; // 满信号量
} ShareSemaphore;

// 定义有界缓冲池，为满足空满判定，需要记录前后位置
typedef struct
{
    char names[BUFFER_SIZE + 1][NAME_SIZE];
    int front;
    int rear;
} People;

// 共享数据
typedef struct
{
    int sem_fd;
    int data_fd;

    ShareSemaphore* sem;
    People* data;
} ShareData;


void printLog(const char* label, const char* __restrict format, ...); // 带有标签的打印
void printShareData(const People* data); // 打印数据

int getInput(const char* label, char* input_buffer); // 获取用户输入，返回 0 表示退出程序

int shareInit(ShareData* sd, int is_creat); // 创建（获取）共享内存，初始化信号量
int shareCleanup(ShareData* sd); // 取消地址段映射


#endif // LAB_PAC_H

// 2025-11-26 kuuhaku-kzr