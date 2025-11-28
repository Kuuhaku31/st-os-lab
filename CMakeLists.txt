cmake_minimum_required(VERSION 4.1)
project(st_os_lab C)

set(CMAKE_C_STANDARD 11)

add_executable(st_os_lab lab-06-RCULock/library.c)
add_executable(lab07 lab-07-scheduler/main.c)


include_directories(
        #/home/kuuhaku-ubez/def/linux-source-6.8.0/include/
        #/home/kuuhaku-ubez/def/linux-source-6.8.0/include/uapi/
        #/home/kuuhaku-ubez/def/linux-source-6.8.0/include/generated/
        #/home/kuuhaku-ubez/def/linux-source-6.8.0/arch/x86/include/
        #/home/kuuhaku-ubez/def/linux-source-6.8.0/arch/x86/include/uapi/
        #/home/kuuhaku-ubez/def/linux-source-6.8.0/arch/x86/include/generated/

        /usr/src/linux-hwe-6.14-headers-6.14.0-36/include/
        /usr/src/linux-hwe-6.14-headers-6.14.0-36/include/uapi/
        /usr/src/linux-hwe-6.14-headers-6.14.0-36/arch/x86/include/
        /usr/src/linux-headers-6.14.0-36-generic/include/
        /usr/src/linux-headers-6.14.0-36-generic/include/uapi/
        /usr/src/linux-headers-6.14.0-36-generic/arch/x86/include/
)
add_executable(test lab-06-RCULock/test.c)
