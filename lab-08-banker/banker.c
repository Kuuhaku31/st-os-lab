// 

#include "banker.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

void
PrintLog(int chage_line, const char* label, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    printf("%s", label);
    vprintf(format, args);
    if(chage_line) printf("\n");

    va_end(args);
}

int
GetInput(const char* prompt, char* cmd_buf, int cmd_size, char* arg_buf, int arg_size)
{
    printf("%s", prompt);

    if(cmd_buf == NULL || cmd_size <= 0 ) return -1;

    char line[cmd_size + arg_size + 1]; // 临时缓冲区

    // 读取一整行
    if(fgets(line, sizeof(line), stdin) == NULL)
    {
        cmd_buf[0] = '\0';
        if(arg_buf != NULL && arg_size > 0) arg_buf[0] = '\0';
        return -1;
    }

    // 如果未读到换行，说明输入太长，返回错误
    char* p;
    if((p = strchr(line, '\n')) == NULL)
    {
        // 清空多余输入
        int ch;
        while((ch = getchar()) != '\n' && ch != EOF);
        return -1;
    }
    else *p = '\0'; // 去掉换行符

    // 去掉前导空格
    p = line;
    while(*p && isspace((unsigned char)*p)) p++;

    // 查找第一个空格（命令与参数分隔处）
    char* space = strchr(p, ' ');
    if(space == NULL)
    {
        // 只有命令，没有参数
        strncpy(cmd_buf, p, cmd_size - 1);
        cmd_buf[cmd_size - 1] = '\0';
        if(arg_buf != NULL && arg_size > 0) arg_buf[0] = '\0';
    }
    else
    { 
        // 拷贝命令
        *space = '\0';
        strncpy(cmd_buf, p, cmd_size - 1);
        cmd_buf[cmd_size - 1] = '\0';

        // 跳过多个空格
        char* args = space + 1;
        while(*args && isspace((unsigned char)*args)) args++;

        if(arg_buf != NULL && arg_size > 0)
        {
            // 拷贝参数
            strncpy(arg_buf, args, arg_size - 1);
            arg_buf[arg_size - 1] = '\0';
        }
    }
    return 0;
}


// 打印系统当前资源分配情况
// M 资源种类数
// N 进程个数
// iMax[][] 每个进程对每类资源的最大需求
// iAllocation[][] 每个进程当前已分配的各类资源数
// iNeed[][] 每个进程尚需的各类资源数
// iAvailable[] 当前可用每类资源数
// cName[] 进程名称
void
PrintStatus(int iMax[N][M], int iAllocation[N][M], int iNeed[N][M], int iAvailable[M], char cName[N])
{
    // Available
    printf("\nAvailable: ");
    for(int i = 0; i < M; i++) printf("%d ", iAvailable[i]);
    printf("\n");

    // 表头
    printf("Process\t\tMax\t\tAllocation\tNeed\n");

    // 每个进程的 Max / Allocation / Need
    for(int i = 0; i < N; i++)
    {
        printf("%c\t\t", cName[i]);

        // Max
        for(int j = 0; j < M; j++) printf("%d ", iMax[i][j]);
        printf("\t\t");
        
        // Allocation
        for(int j = 0; j < M; j++) printf("%d ", iAllocation[i][j]);
        printf("\t\t");
        
        // Need
        for(int j = 0; j < M; j++) printf("%d ", iNeed[i][j]);
        printf("\n");
    }

   printf("\n");
}


// 安全性算法，进行安全性检查；安全返回true，并且输出安全序列，不安全返回false，并输出不安全的提示
int
IsSafety(int iAllocation[N][M], int iNeed[N][M], int iAvailable[M], char cName[N])
{
    int work[M];
    int finish[N];
    int safeSeq[N];
    int count = 0;

    /* 初始化 work 为可用资源，finish 全部置 0 */
    for(int i = 0; i < M; i++) work[i] = iAvailable[i];
    for(int i = 0; i < N; i++) finish[i] = 0;

    /* 找到一个进程其 need <= work */
    while(count < N)
    {
        int found = 0;
        for(int i = 0; i < N; i++)
        {
            if(finish[i]) continue;

            int can = 1;
            for(int j = 0; j < M; j++)
            {
                if(iNeed[i][j] > work[j])
                {
                    can = 0;
                    break;
                }
            }

            if(can)
            {
                /* 模拟该进程完成并释放资源 */
                for(int j = 0; j < M; j++) work[j] += iAllocation[i][j];
                safeSeq[count++] = i;
                finish[i]        = 1;
                found            = 1;
            }
        }

        if(!found) break; /* 无可运行进程，退出 */
    }

    if(count == N)
    {
        PrintLog(FALSE, BANKER_LABEL, "系统处于安全状态，安全序列为: ");
        for(int i = 0; i < N; i++)
        {
            printf("%c", cName[safeSeq[i]]);
            if(i < N - 1) printf(" -> ");
        }
        printf("\n");
        return TRUE;
    }
    else
    {
        PrintLog(TRUE, BANKER_LABEL, "系统处于不安全状态，未找到完整的安全序列");
        return FALSE;
    }
}


// 安全返回true，不安全返回false
int 
Banker(int iAllocation[N][M], int iNeed[N][M], int iAvailable[M], char cName[N])
{
    int pid;
    int req[M];

    PrintLog(FALSE, BANKER_LABEL, "请输入申请资源的进程序号（ 0 - %d ）: ", N - 1);
    if(scanf("%d", &pid) != 1)
    {
        while(getchar() != '\n');
        PrintLog(TRUE, BANKER_LABEL, "读取进程序号失败");
        return FALSE;
    }
    if(pid < 0 || pid >= N)
    {
        PrintLog(TRUE, BANKER_LABEL, "进程序号超出范围");
        return FALSE;
    }

    PrintLog(FALSE, BANKER_LABEL, "请输入请求的资源数（ A B C ）: ");
    if(scanf("%d %d %d", &req[0], &req[1], &req[2]) != 3)
    {
        while(getchar() != '\n');
        PrintLog(TRUE, BANKER_LABEL, "读取请求向量失败");
        return FALSE;
    }
    while(getchar() != '\n');

    /* 检查请求是否超过该进程的最大需要 */
    for(int i = 0; i < M; i++)
    {
        if(req[i] > iNeed[pid][i])
        {
            PrintLog(TRUE, BANKER_LABEL, "请求的资源数超过该进程的最大需求");
            return FALSE;
        }
    }

    /* 检查是否有足够的可用资源 */
    for(int i = 0; i < M; i++)
    {
        if(req[i] > iAvailable[i])
        {
            PrintLog(TRUE, BANKER_LABEL, "当前可用资源不足");
            return FALSE;
        }
    }

    /* 尝试分配（暂时分配） */
    for(int i = 0; i < M; i++)
    {
        iAvailable[i]       -= req[i];
        iAllocation[pid][i] += req[i];
        iNeed[pid][i]       -= req[i];
    }

    /* 检查分配后是否处于安全态 */
    if(IsSafety(iAllocation, iNeed, iAvailable, cName))
    {
        PrintLog(TRUE, BANKER_LABEL, "资源分配后系统仍安全，分配成功");
        return TRUE;
    }
    else
    {
        /* 回滚 */
        for(int i = 0; i < M; i++)
        {
            iAvailable[i]       += req[i];
            iAllocation[pid][i] -= req[i];
            iNeed[pid][i]       += req[i];
        }
        PrintLog(TRUE, BANKER_LABEL, "资源分配会使系统不安全，分配被拒绝");
        return FALSE;
    }
}

// kuuhaku-kzr