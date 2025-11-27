
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


// 线程
void
thread()
{
    for(int i = 0; i < 30; i++)
    {
        float sleep_time = 1.0f;
        printf("I am a pthread, count: %d\n", i);
        sleep(sleep_time);
    }
}

int
main()
{
    pthread_t tid = 0;

    // 创建线程
    int ret = pthread_create(&tid, NULL, (void*)thread, NULL);
    if(ret != 0)
    {
        printf("Create thread error!\n");
        exit(1);
    }

    int count = 0;
    while(1)
    {
        if(count++ > 20) break;
        printf("I am the main process. My PID=%d\n", getpid());
        sleep(1);
    }

    printf("Bye.\n");

    return (0);
}