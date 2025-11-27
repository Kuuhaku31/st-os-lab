项目结构:

有以下文件:

pac.h
pac.c

pro.c
cons.c

其中 pro.c 和 cons.c：

都包含有在 pac.h 中声明，pac.c 中定义的方法
都有 main() 函数

要编译成两个可执行文件

如何设计MakeFile

```c++
semfd =             open("/tmp/sem",    O_CREAT | O_RDWR | O_TRUNC, 0666);
ssem = (sharesem*)  mmap(NULL, sizeof(sharesem),PROT_READ | PROT_WRITE,MAP_SHARED, semfd, 0);
datafd =            open("/tmp/datapc", O_CREAT | O_RDWR | O_TRUNC, 0666);
data = (people*)    mmap(NULL, sizeof(people),PROT_READ | PROT_WRITE,MAP_SHARED, datafd, 0);

semfd =             open("/tmp/sem",    O_CREAT | O_RDWR, 0666);
ssem = (sharesem*)  mmap(NULL, sizeof(sharesem),PROT_READ | PROT_WRITE,MAP_SHARED, semfd, 0);
datafd =            open("/tmp/datapc", O_CREAT | O_RDWR, 0666);
data = (people*)    mmap(NULL, sizeof(people),PROT_READ | PROT_WRITE,MAP_SHARED, datafd, 0);
```