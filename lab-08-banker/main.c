//
// 模拟银行家算法


#include <stdio.h>
#include <string.h>

#define TRUE  1
#define FALSE 0

#define M 3 // 资源的种类数
#define N 5 // 进程的个数

void output(int iMax[N][M], int iAllocation[N][M], int iNeed[N][M], int iAvailable[M], char cName[N]);

// 统一的输出格式
int safety(int iAllocation[N][M], int iNeed[N][M], int iAvailable[M], char cName[N]);
int banker(int iAllocation[N][M], int iNeed[N][M], int iAvailable[M], char cName[N]);

int
main()
{
    int i, j;
    // 当前可用每类资源的资源数
    int iAvailable[M] = { 3, 3, 2 };
    // 系统中N个进程中的每一个进程对M类资源的最大需求
    int iMax[N][M] = { { 7, 5, 3 }, { 3, 2, 2 }, { 9, 0, 2 }, { 2, 2, 2 }, { 4, 3, 3 } };
    // iNeed[N][M]每一个进程尚需的各类资源数
    // iAllocation[N][M]为系统中每一类资源当前已分配给每一进程的资源数
    int iNeed[N][M], iAllocation[N][M] = { { 0, 1, 1 }, { 2, 0, 0 }, { 3, 0, 2 }, { 2, 1, 1 }, { 0, 0, 2 } };
    // 进程名
    char cName[N]  = { 'a', 'b', 'c', 'd', 'e' };
    int bExitFlag = TRUE; // 退出标记
    char ch;               // 接收选择是否继续提出申请时传进来的值

    int bSafe; // 存放安全与否的标志
    // 计算iNeed[N][M]的值
    for(i = 0; i < N; i++)
        for(j = 0; j < M; j++)
            iNeed[i][j] = iMax[i][j] - iAllocation[i][j];
    // 输出初始值
    output(iMax, iAllocation, iNeed, iAvailable, cName);
    // 判断当前状态是否安全
    bSafe = safety(iAllocation, iNeed, iAvailable, cName);

    // 是否继续提出申请
    while(bExitFlag)
    {
        printf("\n继续提出申请？\ny为是；n为否。\n");
        scanf(" %c", &ch);
        
        switch(ch)
        {
        case 'y':
            // printf("调用银行家算法");
             bSafe = banker(iAllocation, iNeed, iAvailable, cName);
            if(bSafe) // 安全，则输出变化后的数据
                output(iMax, iAllocation, iNeed, iAvailable, cName);
            break;
        case 'n':
            printf("退出。\n");
            
            bExitFlag = FALSE;
            break;
        default:
            printf("输入有误，请重新输入：\n");
        }
    }
}

// 输出
void 
output(int iMax[N][M], int iAllocation[N][M], int iNeed[N][M], int iAvailable[M], char cName[N])
{
    int i, j;

    printf("\n\t   Max  \tAllocation\t  Need  \t Available\n");
    printf("\tA   B   C\tA   B   C\tA   B   C\t A   B   C\n");

    for(i = 0; i < N; i++)
    {
        printf("%c\t", cName[i]);

        for(j = 0; j < M; j++) printf("%d   ", iMax[i][j]);
        printf("\t");

        for(j = 0; j < M; j++) printf("%d   ", iAllocation[i][j]);
        printf("\t");

        for(j = 0; j < M; j++) printf("%d   ", iNeed[i][j]);
        printf("\t ");

        /* Available只需要输出一次 */
        if(i == 0) for(j = 0; j < M; j++) printf("%d   ", iAvailable[j]);

        printf("\n");
    }
}

// 安全性算法，进行安全性检查；安全返回true，并且输出安全序列，不安全返回false，并输出不安全的提示
int safety(int iAllocation[N][M], int iNeed[N][M], int iAvailable[M], char cName[N])
{
    // TODO: 安全性算法，进行安全性检查；安全返回true，并且输出安全序列，不安全返回false，并输出不安全的提示
    return TRUE;
}

// 安全返回true，不安全返回false
int banker(int iAllocation[N][M], int iNeed[N][M], int iAvailable[M], char cName[N])
{
    // TODO: 安全返回true，不安全返回false
    return TRUE;
}

// kuuhaku-kzr