fork:
	gcc ./fork.c -o fork.a
	./fork.a

th:
	gcc ./thread.c -lpthread  -o thread.a
	./thread.a

ps:
	gcc process_sync.c -o process_sync.a
	./process_sync.a

pss:
	gcc -pthread process_sync_sem.c -o process_sync_sem.a
	./process_sync_sem.a