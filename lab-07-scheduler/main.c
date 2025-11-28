//

#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void*
thread_fun()
{
    printf("thread pid:%d, tid:%lu\n", getpid(), pthread_self());
    while(1);
    return NULL;
}

int
main()
{
    pthread_t tid;

    printf("main pid:%d, tid:%lu\n", getpid(), pthread_self());
    int ret = pthread_create(&tid, NULL, thread_fun, NULL);
    if(ret == -1)
    {
        perror("cannot create new thread");
        return 1;
    }
    if(pthread_join(tid, NULL) != 0)
    {
        perror("call pthread_join function fail");
        return 1;
    }
    return 0;
}

// kuuhaku-kzr