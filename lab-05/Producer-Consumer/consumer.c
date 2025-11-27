// Consumer

#include "util.h"

#include <string.h>


// 消费函数
int
consume(const ShareData* sd)
{
    // 尝试占用空信号量和同步信号量
    sem_wait(&sd->sem->full);
    sem_wait(&sd->sem->sync);

    // 消费者消费
    char* product = sd->data->names[sd->data->front];            // 取出产品
    sd->data->front = (sd->data->front + 1) % (BUFFER_SIZE + 1); // 前位置 + 1
    printLog(CONSUMER_LABEL, "consumed: " PRINT_NAME, product);

    // 释放同步信号量及满信号量（+1）
    sem_post(&sd->sem->sync);
    sem_post(&sd->sem->empty);

    return 0;
}


int
main()
{
    printLog(CONSUMER_LABEL, "Hello Consumer");

    // 初始化
    ShareData share_data;
    if(shareInit(&share_data, 0)) printLog(CONSUMER_LABEL, "初始化失败");
    else
    {
        // 循环获取用户输入姓名，直到用户输入exit则退出循环
        // 将用户输入传递给生产者函数
        char input_buffer[NAME_SIZE] = "";
        while(getInput(CONSUMER_LABEL, input_buffer))
        {
            if(!strcmp(input_buffer, "pd")) // 打印数据
            {
                printLog(CONSUMER_LABEL, "print data: ");
                printShareData(share_data.data);
            }
            if(!strcmp(input_buffer, "c"))
            {
                printLog(CONSUMER_LABEL, "consume...");
                consume(&share_data);
            }
        }
        printLog(CONSUMER_LABEL, "exit...");
    }

    // 清理
    if(shareCleanup(&share_data))
    {
        printLog(CONSUMER_LABEL, "清理出现问题");
        return -1;
    }

    printLog(CONSUMER_LABEL, "Bye.");
    return 0;
}

// 2025-11-26 kuuhaku-kzr