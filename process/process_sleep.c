// process_sleep.c

#include <stdio.h>
#include <unistd.h>


int
main(void)
{
    int count = 0;
    // 死循环，每1s间隔打印自此自身PID
    while(1)
    {
        int pid = getpid();
        printf("my pid is [%d], count = %d\n", pid, count++);
        sleep(1);
    }
    return 0;
}
