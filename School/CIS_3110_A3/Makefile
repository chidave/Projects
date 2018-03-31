all: dine holes

dine: dine.o
	gcc -o bin/dine bin/dine.o -pthread

dine.o: src/dine.c
	gcc -c src/dine.c -o bin/dine.o -Wall -g
	
holes: holes.o LinkedListAPI.o
	gcc -o bin/holes bin/holes.o bin/LinkedListAPI.o

holes.o: src/holes.c
	gcc -Iinclude -c src/holes.c -o bin/holes.o -Wall -g

LinkedListAPI.o: src/LinkedListAPI.c
	gcc -Iinclude -c src/LinkedListAPI.c -o bin/LinkedListAPI.o -Wall -g
	
clean:
	rm -f bin/*
	
