// test

#include <stdio.h>
#include <string.h>
#include <ctype.h>


/*
    假设用户输入格式为 xxxx xxxxxxxxxxxxx xxx ...
    即第一个空格前为命令，后面为参数
    分别存入 cmd_buf 和 arg_buf 中
    如果输入过长，则多余部分丢弃
 */
int
GetInput(const char* prompt, char* cmd_buf, int cmd_size, char* arg_buf, int arg_size)
{
    printf("%s", prompt);

    char line[cmd_size + arg_size + 1]; // 临时缓冲区

    // 读取一整行
    if(fgets(line, sizeof(line), stdin) == NULL)
    {
        cmd_buf[0] = arg_buf[0] = '\0';
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
        arg_buf[0]            = '\0';
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

        // 拷贝参数
        strncpy(arg_buf, args, arg_size - 1);
        arg_buf[arg_size - 1] = '\0';
    }   
    return 0;
}

int
main(int argc, char* argv[])
{
    printf("This is a test file for lab-08-banker.\n");
    for(int i = 0; i < argc; i++) printf("Argument[%0d]: %s\n", i, argv[i]);

    char cmd_buf[16];
    char arg_buf[64];
    char prompt[] = "[\033[1;31mProducer\033[0m]: ";
    while(strcmp(cmd_buf, "q") != 0)
    {
        if(GetInput(prompt, cmd_buf, sizeof(cmd_buf), arg_buf, sizeof(arg_buf)) == 0)
        {
            printf("Command: %s\n", cmd_buf);
            printf("Arguments: %s\n", arg_buf);
        }
        else printf("Input error.\n");
    }
    printf("Exiting test program.\n");

    return 0;
}

// kuuhaku-kzr