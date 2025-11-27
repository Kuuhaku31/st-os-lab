
# ALL
make, all: producer.out consumer.out

# 可执行文件
producer.out: producer.o util.o
	gcc -pthread producer.o util.o -o producer.out
consumer.out: consumer.o util.o
	gcc -pthread consumer.o util.o -o consumer.out

# 二进制文件
producer.o: util.h producer.c
	gcc -c producer.out.c
consumer.o: util.h consumer.c
	gcc -c consumer.out.c
util.o: util.h util.c
	gcc -c util.c

# 运行
rp: producer.out
	./producer.out
rc: consumer.out
	./consumer.out

# 清理
clean, cl, c:
	rm -f producer.o consumer.o util.o producer.out consumer.out util

# 2025-11-26 kuuhaku-kzr