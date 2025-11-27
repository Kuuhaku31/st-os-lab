
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


// 线程
void
thread()
{
    float sleep_time = 1.0;
    // for(int i = 0; i < 30; i++)
    // {
    //     printf("I am a pthread, count: %d\n", i);
    //     sleep(sleep_time);
    // }
    while(1)
    {
        pthread_t tid = 0;
        printf("process PID = %d\n", getpid());
        pthread_create(&tid, NULL, (void*)thread, NULL);
        sleep(sleep_time);
    }
}

int
main()
{
    printf("I am the main process. My PID=%d\n", getpid());

    sleep(10);

    pthread_t tid = 0;

    // 创建线程
    int ret = pthread_create(&tid, NULL, (void*)thread, NULL);
    if(ret != 0)
    {
        printf("Create thread error!\n");
        exit(1);
    }

    while(1)
    {
        printf("I am the main process. My PID=%d\n", getpid());
        sleep(1);
    }

    printf("Bye.\n");

    return (0);
}