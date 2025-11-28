//

#pragma once

#define TRUE  1
#define FALSE 0

#define RESOURCE_TYPE_COUNT 3 // 资源的种类数
#define PROCESS_COUNT 5       // 进程的个数

#define M RESOURCE_TYPE_COUNT
#define N PROCESS_COUNT

#define BANKER_LABEL "[\033[1;34mBanker\033[0m]: "

void PrintLog(int chage_line, const char* label, const char* format, ...); // 带有标签的打印
int  GetInput(const char* prompt, char* cmd_buf, int cmd_size, char* arg_buf, int arg_size);
void PrintStatus(int iMax[N][M], int iAllocation[N][M], int iNeed[N][M], int iAvailable[M], char cName[N]);

// 统一的输出格式
int IsSafety(int iAllocation[N][M], int iNeed[N][M], int iAvailable[M], char cName[N]);
int Banker(int iAllocation[N][M], int iNeed[N][M], int iAvailable[M], char cName[N]);

// kuuhaku-kzr