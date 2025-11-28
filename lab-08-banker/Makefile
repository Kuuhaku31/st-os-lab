
b: banker.out
	./banker.out 
t: test.out
	./test.out

clean:
	rm -f *.o *.out

banker.out: banker.o main.o
	gcc banker.o main.o -o banker.out
test.out: test.o
	gcc test.o -o test.out -g

banker.o: banker.c banker.h
	gcc -c banker.c -o banker.o
main.o: main.c banker.h
	gcc -c main.c -o main.o
test.o: test.c
	gcc -c test.c -o test.o -g