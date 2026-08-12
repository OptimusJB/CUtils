all: list.a list.so

list.a: list.o
	ar rcs list.a list.o

list.so: list.o
	gcc -shared list.o -o list.so

list.o: list.c
	gcc -c -fPIC list.c -o list.o

clean:
	rm *.a
	rm *.o
	rm *.so
