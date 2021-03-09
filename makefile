main: main.o tokenizer.o
	gcc -o main main.o tokenizer.o
main.o: main.c tokenizer.h
	gcc -c -g main.c
tokenizer.o: tokenizer.c tokenizer.h
	gcc -c -g tokenizer.c