cmake_minimum_required(VERSION 3.16)
project(lab C)

set(CMAKE_C_STANDARD 11)

# 设置输出目录
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# Producer-Consumer 问题
add_executable(
        producer
        lab-05/Producer-Consumer/producer.c
        lab-05/Producer-Consumer/util.h
        lab-05/Producer-Consumer/util.c
)
add_executable(
        consumer
        lab-05/Producer-Consumer/consumer.c
        lab-05/Producer-Consumer/util.h
        lab-05/Producer-Consumer/util.c
)


# 分别为每个源文件创建可执行文件
add_executable(pthread_join lab-04/pthread_join.c)
add_executable(thread_test lab-04/thread_test.c)
add_executable(fork lab-05/fork.c)
add_executable(thread lab-05/thread.c)
add_executable(fork_test process/fork_test.c)
add_executable(process_sleep process/process_sleep.c)
add_executable(process_sync lab-05/process_sync.c)
add_executable(process_sync_sem lab-05/process_sync_sem.c)

# 链接 pthread 库
target_link_libraries(pthread_join PRIVATE pthread)
target_link_libraries(thread_test PRIVATE pthread)
target_link_libraries(thread PRIVATE pthread)
target_link_libraries(process_sync_sem PRIVATE pthread)
target_link_libraries(producer PRIVATE pthread)
target_link_libraries(consumer PRIVATE pthread)

