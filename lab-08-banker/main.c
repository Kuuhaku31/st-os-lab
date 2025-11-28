// 模拟银行家算法
//

#include "banker.h"

#include <stdio.h>
#include <string.h>

int
main()
{

    PrintLog(TRUE, BANKER_LABEL, "=== 银行家算法 ===");

    // 当前可用每类资源的资源数
    int iAvailable[M] = { 3, 3, 2 };

    // 系统中每一类资源当前已分配给每一进程的资源数
    int iAllocation[N][M] = {
        { 0, 1, 1 },
        { 2, 0, 0 },
        { 3, 0, 2 },
        { 2, 1, 1 },
        { 0, 0, 2 }
    };

    // 系统中 N 个进程中的每一个进程对 M 类资源的最大需求
    int iMax[N][M] = {
        { 7, 5, 3 },
        { 3, 2, 2 },
        { 9, 0, 2 },
        { 2, 2, 2 },
        { 4, 3, 3 }
    };

    // 进程名称
    char cName[N] = { 'A', 'B', 'C', 'D', 'E' };

    // 每一个进程尚需的各类资源数
    int iNeed[N][M];

    char cmd[10]; // 接收选择是否继续提出申请时传进来的值

    int bSafe; // 存放安全与否的标志

    // 计算iNeed[N][M]的值
    for(int i = 0; i < N; i++) for(int j = 0; j < M; j++) iNeed[i][j] = iMax[i][j] - iAllocation[i][j];

    // 输出初始值
    PrintStatus(iMax, iAllocation, iNeed, iAvailable, cName);
    
    // 判断当前状态是否安全
    bSafe = IsSafety(iAllocation, iNeed, iAvailable, cName);

    // 是否继续提出申请
    while(1)
    {
        GetInput(BANKER_LABEL, cmd, sizeof(cmd), NULL, 0);

        if(strcmp(cmd, "a") == 0)
        {
            PrintLog(TRUE, BANKER_LABEL, "调用银行家算法");
            int bSafe = Banker(iAllocation, iNeed, iAvailable, cName);
            if(bSafe) PrintStatus(iMax, iAllocation, iNeed, iAvailable, cName);
        }
        else if(strcmp(cmd, "q") == 0)
        {
            PrintLog(TRUE, BANKER_LABEL, "quit.");
            break;
        }
        else if(strcmp(cmd, "") == 0); // 空命令，忽略
        else if (strcmp(cmd, "pd") == 0) PrintStatus(iMax, iAllocation, iNeed, iAvailable, cName);
        else PrintLog(TRUE, BANKER_LABEL, "未知命令: %s", cmd);
    }
}


// kuuhaku-kzr