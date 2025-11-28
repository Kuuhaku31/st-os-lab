// 页面置换算法模拟

#include <iostream>
using namespace std;

#define M 3  // 物理块数
#define N 20 // 页面引用串长度
struct block
{
    int iPageNum;
    int iBlockFlag;
};

// 物理块里存储的页面号
// 算法模拟移位寄存器原理
// 在三种算法中用到的标记。例如在FIFO中为在内存中的时间
void FIFO(int iTempPage[N], int flag[N], block myBlock[M]);
void Optimal(int iTempPage[N], int flag[N], block myBlock[M]);
void LRU(int iTempPage[N], int flag[N], block myBlock[M]);
int  PageNum(int array[]);

int
main()
{
    block myBlock[M];
    int   iPageString[N] = { 7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1 };
    // 页面引用串
    int  iTempPage[N]; // 临时页面引用串
    int  flag[N];      // 缺页标记；1为缺页，0为不缺页,在统计缺页次数时用
    int  i;
    bool bExitFlag = true; // 退出标记
    char ch;               // 接收选择算法时传进来的值
    while(bExitFlag)
    {
        cout << "\n"
             << "请选择页面置换算法：\n";
        cout << "f:FIFO置换算法\to:OPT置换算法\tl:LRU置换算法\tx:退出置换算法程序.\n";
        cin >> ch;
        // 初始化数据
        if((ch == 'f') || (ch == 'o') || (ch == 'l'))
        {
            for(i = 0; i < N; i++)
            {
                iTempPage[i] = iPageString[i]; // 初始化临时页面引用串
                flag[i]      = 0;              // 初始化缺页标记为0，即不缺页
            }
        }
        switch(ch)
        {
        case 'f':
            cout << "FIFO置换算法的结果是：\n";
            FIFO(iTempPage, flag, myBlock);
            // 用PageNum(flag)统计缺页次数
            cout << "\n缺页次数为" << PageNum(flag) << endl;
            break;
        case 'o':
            cout << "OPT置换算法的结果是：\n";
            Optimal(iTempPage, flag, myBlock);
            cout << "\n缺页次数为" << PageNum(flag) << endl;
            break;
        case 'l':
            cout << "LRU置换算法的结果是：\n";
            LRU(iTempPage, flag, myBlock);
            cout << "\n缺页次数为" << PageNum(flag) << endl;
            break;
        case 'x':
            cout << "退出置换算法。\n";
            bExitFlag = false;
            break;
        default:
            cout << "输入有误，请重新选择置换算法：\n";
        }
    }
}

// 对数组中的数累加
int
PageNum(int array[])
{
    int num = 0;
    for(int j = 0; j < N; j++)
        num = num + array[j];
    return num;
}

// 定位函数，在最佳算法中用于定位；
// 定位物理块中的某一个页面在引用串中还未访问串中页面的位置
int
allocate(int iPage, int iLoc, int iTempPage[N])
{
    int i;
    for(i = iLoc; i < N; i++)
    {
        if(iPage == iTempPage[i])
            return i;
    }
    // 永远不再访问的页面位置假定为N
    return N;
}

// 找数组中最大值所在的下标，返回最大值在数组中的位置（下标）
int
max(block array[M])
{
    int j, loc;
    int temp = array[0].iBlockFlag;
    loc      = 0;
    for(j = 1; j < M; j++)
    {
        if(temp < array[j].iBlockFlag)
        {
            temp = array[j].iBlockFlag;
            loc  = j;
        }
    }
    return loc;
}

// 输出剩余的数据
// loc为页面引用串中
void
output(int iPage, int flag, block myBlock[M], int blockNum)
{
    // 如果缺页则输出缺页标志，否则不输出
    if(flag == 1) cout << "\n  " << flag;
    else cout << "\n  ";
    cout << "\t  " << iPage;
    for(int i = 0; i < blockNum; i++) cout << "\t  " << myBlock[i].iPageNum;
}

// 初始化物理块的内容，因任一种算法在物理块内容为空时，结果都一样的
// 同时将目前物理块中的内容输出
void
InitialBlock(int iTempPage[N], int flag[N], block myBlock[M])
{
    int i;
    for(i = 0; i < M; i++)
    {
        // 初始化物理块的内容，因任一种算法在物理块内容为空时，结果都一样的
        myBlock[i].iPageNum = iTempPage[i];

        // myBlock[i].iBlockFlag的值：0为最后进来的，数越大表示进来的越早
        // 在最佳置换算法中则初始化此值没有意义
        myBlock[i].iBlockFlag = (M - 1) - i;
        flag[i]               = 1; // 此时为缺页
    }
    // 输出
    cout << "\n缺页\t引用串\t物理块1\t物理块2\t物理块3";
    for(i = 0; i < M; i++) output(iTempPage[i], flag[i], myBlock, i + 1);
}

// FIFO置换算法
void
FIFO(int iTempPage[N], int flag[N], block myBlock[M])
{
    int  i, j, k, loc;
    bool ExistFlag = false;
    // 初始化物理块的内容，因任一种算法在物理块内容为空时，结果都一样的
    // 同时将目前物理块中的内容输出
    InitialBlock(iTempPage, flag, myBlock);
    // 从引用串中的第4个页面开始
    for(i = 3; i < N; i++)
    {
        ExistFlag = false;
        for(j = 0; j < M; j++)
        {
            // 物理块中存在
            if(myBlock[j].iPageNum == iTempPage[i])
            {
                // 模拟移位寄存器
                for(k = 0; k < M; k++) myBlock[k].iBlockFlag++;
                ExistFlag = true;
                flag[i]   = 0;
                break;
            }
        }
        // 物理块中不存在
        if(!ExistFlag)
        {
            // 查找最先进来的页面，也就是block中iBlockFlag最大的物理块
            loc                   = max(myBlock);
            myBlock[loc].iPageNum = iTempPage[i];
            // 置缺页标志
            flag[i] = 1;
            // 模拟移位寄存器
            for(k = 0; k < M; k++)
                if(k != loc)
                    myBlock[k].iBlockFlag++;
                else
                    myBlock[k].iBlockFlag = 0;
        }
        
        // 输出
        output(iTempPage[i], flag[i], myBlock, M);
    }
    cout << endl;
}

// Optimal置换算法
void Optimal(int iTempPage[N], int flag[N], block myBlock[M])
{
    // 初始化物理块与输出表头
    InitialBlock(iTempPage, flag, myBlock);

    // 从第4个引用开始模拟
    for(int i = 3; i < N; i++)
    {
        bool exist = false;
        // 命中检测
        for(int j = 0; j < M; j++)
        {
            if(myBlock[j].iPageNum == iTempPage[i])
            {
                // 命中：不缺页
                flag[i] = 0;
                exist   = true;
                break;
            }
        }

        if(!exist)
        {
            // 未命中：寻找未来最晚再次使用或不再使用的页面
            int farthest = -1; // 最大的未来位置
            int victim   = 0;  // 要替换的物理块下标
            for(int j = 0; j < M; j++)
            {
                // allocate 搜索 (i+1) 之后的下一次使用位置，若返回 N 代表不再使用
                int nextUse = allocate(myBlock[j].iPageNum, i + 1, iTempPage);
                if(nextUse > farthest)
                {
                    farthest = nextUse;
                    victim   = j;
                }
            }
            // 进行替换
            myBlock[victim].iPageNum = iTempPage[i];
            flag[i]                  = 1; // 缺页
        }
        // 输出当前状态
        output(iTempPage[i], flag[i], myBlock, M);
    }
    cout << endl;
}

// LRU 置换算法
void LRU(int iTempPage[N], int flag[N], block myBlock[M])
{
    // 初始化
    InitialBlock(iTempPage, flag, myBlock);

    // 从第4个引用开始模拟
    for(int i = 3; i < N; i++)
    {
        int hitIndex = -1;
        // 查找是否命中
        for(int j = 0; j < M; j++)
        {
            if(myBlock[j].iPageNum == iTempPage[i])
            {
                hitIndex = j;
                break;
            }
        }

        if(hitIndex != -1)
        {
            // 命中：缺页标记为0，命中的块最近使用，重置为0，其他块 age++
            flag[i] = 0;
            for(int j = 0; j < M; j++)
            {
                if(j == hitIndex) myBlock[j].iBlockFlag = 0;
                else               myBlock[j].iBlockFlag++;
            }
        }
        else
        {
            // 未命中：选择 age 最大（最久未使用）的块进行替换
            int victim    = 0;
            int maxAge    = myBlock[0].iBlockFlag;
            for(int j = 1; j < M; j++)
            {
                if(myBlock[j].iBlockFlag > maxAge)
                {
                    maxAge = myBlock[j].iBlockFlag;
                    victim = j;
                }
            }
            // 替换并设置 age=0，其他 age++
            myBlock[victim].iPageNum   = iTempPage[i];
            myBlock[victim].iBlockFlag = 0;
            for(int j = 0; j < M; j++)
                if(j != victim) myBlock[j].iBlockFlag++;
            flag[i] = 1; // 缺页
        }
        // 输出当前状态
        output(iTempPage[i], flag[i], myBlock, M);
    }
    cout << endl;
}

// kuuhaku-kzr