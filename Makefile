SRC = src/main.asm
OBJ = build/main.o

all:
	nasm -felf64 src/main.asm -o build/main.o
	gcc -no-pie $(OBJ) -o main

clean:
	rm build/*
	rm main