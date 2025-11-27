
#include <stdio.h>
#include <unistd.h>

int
main()
{
    pid_t fpid  = 0;
    int   count = 0;

    int pid = getpid();
    printf("PID = %d\n", pid);

    fpid = fork(); // 调用fork函数，复制进程

    if(fpid < 0) // 若返回值小于0，则表示复制出错
    {
        printf("error in fork!");
    }
    else if(fpid == 0) // 若返回值等于0，则表示此时为复制出的子进程在运行
    {
        printf("i am the child process, my PID is %d\n", getpid());
        printf("fpid: %d\n", fpid);
        count++;
    }
    else // 若返回值大于0，则表示此时为源进程（父进程）在运行
    {
        printf("i am the parent process, my PID is %d\n", getpid());
        printf("fpid: %d\n", fpid);
        count++;
    }

    printf("count: %d\n", count);

    return 0;
}