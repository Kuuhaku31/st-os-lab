// Producer

#include "util.h"

#include <string.h>


// 生产函数
int
product(const ShareData* sd, char* name)
{
    // 尝试占用满信号量和同步信号量
    sem_wait(&sd->sem->empty);
    sem_wait(&sd->sem->sync);

    // 生产者生产
    memcpy(sd->data->names[sd->data->rear], name, strlen(name) + 1);         // 将生产者投放信息放入共享内存
    sd->data->rear = (sd->data->rear + 1) % (BUFFER_SIZE + 1);               // 后位置 + 1
    printLog(PRODUCER_LABEL, "producer has produce name:" PRINT_NAME, name); // 输出生产者投放信息

    // 释放同步信号量及空信号量（+1）
    sem_post(&sd->sem->sync);
    sem_post(&sd->sem->full);

    return 0;
}


int
main()
{
    printLog(PRODUCER_LABEL, "Hello Producer");

    // 初始化
    ShareData share_data;
    if(shareInit(&share_data, 1)) printLog(PRODUCER_LABEL, "初始化失败");
    else
    {
        // 循环获取用户输入姓名，直到用户输入exit则退出循环
        // 将用户输入传递给生产者函数
        char input_buffer[NAME_SIZE] = "";
        while(getInput(PRODUCER_LABEL, input_buffer))
        {
            if(!strcmp(input_buffer, "pd")) // 打印数据
            {
                printLog(PRODUCER_LABEL, "print data: ");
                printShareData(share_data.data);
            }
            else
            {
                printLog(PRODUCER_LABEL, "name is " PRINT_NAME " length:%zu", input_buffer, strlen(input_buffer)); // 输出用户输入的姓名
                printLog(PRODUCER_LABEL, "product...");
                product(&share_data, input_buffer);
            }
        }
        printLog(PRODUCER_LABEL, "exit...");
    }

    // 清理
    if(shareCleanup(&share_data))
    {
        printLog(PRODUCER_LABEL, "清理出现问题");
        return -1;
    }

    printLog(PRODUCER_LABEL, "Bye.");
    return 0;
}

// 2025-11-26 kuuhaku-kzr