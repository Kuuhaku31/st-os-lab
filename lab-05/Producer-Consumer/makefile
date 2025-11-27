
# ALL
make, all: producer consumer

# 可执行文件
producer: producer.o util.o
	gcc -pthread producer.o util.o -o producer
consumer: consumer.o util.o
	gcc -pthread consumer.o util.o -o consumer

# 二进制文件
producer.o: util.h producer.c
	gcc -c producer.c
consumer.o: util.h consumer.c
	gcc -c consumer.c
util.o: util.h util.c
	gcc -c util.c

# 运行
rp: producer
	./producer
rc: consumer
	./consumer

# 清理
clean, cl, c:
	rm -f producer.o consumer.o util.o producer consumer util

# 2025-11-26 kuuhaku-kzr