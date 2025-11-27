
// pthread_join

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void
thread(void)
{
    int i;
    for(i = 0; i < 10; i++)
    {
        printf("I am a pthread.\n");
        sleep(1);
    }
    printf("子线程结束\n");
}

int
main(void)
{
    pthread_t tid;
    int       ret;
    ret = pthread_create(&tid, NULL, (void*)thread, NULL);
    // pthread_detach(tid); // 在创建完成后将其改变为分离状态
    if(ret != 0)
    {
        printf("Create thread error!\n");
        exit(1);
    }


    printf("I am the main process. My PID=%d\n", getpid());
    sleep(1);


    pthread_join(tid, NULL); // 等待创建的线程结束，tid为创建的线程编号
    printf("主线程结束\n");
    return (0);
}