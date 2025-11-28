# 编译
	make

# 运行
	sudo insmod kylin_rcu.ko

# 观测
	dmesg
	或连接串口到另外一台设备观看输出信息

[  607.091002] lab-manager1(4436) is going to print books!!!
[  607.096456] id : 0, name : KylinOS-lab0, author : kylin edu, borrow : 1, addr : ffff9191ba14f840
[  607.098899] callback free : ffff9191ba3593c0, preempt_count : 257
[  607.105275] id : 1, name : KylinOS-lab1, author : kylin edu, borrow : 1, addr : ffff919354ea93c0
[  607.120226] id : 2, name : KylinOS-lab2, author : kylin edu, borrow : 0, addr : ffff919354ea9600
[  607.129025] id : 3, name : KylinOS-lab3, author : kylin edu, borrow : 1, addr : ffff9191ba14fcc0
[  607.137832] id : 4, name : KylinOS-lab4, author : kylin edu, borrow : 1, addr : ffff919354ea9d80
[  607.146637] id : 5, name : KylinOS-lab5, author : kylin edu, borrow : 0, addr : ffff9191ba14f9c0
[  607.155443] id : 6, name : KylinOS-lab6, author : kylin edu, borrow : 0, addr : ffff9191ba359480
[  607.158922] student3(4440) is going to borrow book 2
[  607.164244] id : 7, name : KylinOS-lab7, author : kylin edu, borrow : 0, addr : ffff9191ba14fb40
[  607.164254] id : 8, name : KylinOS-lab8, author : kylin edu, borrow : 0, addr : ffff9191ba14f480
[  607.169231] borrow_book,student3, old_b: 0xffff919354ea9600, new_b:0xffff9191ba14fd80
[  607.178036] id : 9, name : KylinOS-lab9, author : kylin edu, borrow : 1, addr : ffff919354ea9b40
[  607.203466] borrow success 2
[  607.226970] callback free : ffff919354ea9600, preempt_count : 257

