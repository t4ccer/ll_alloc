fs_alloc.o: fs_alloc.h
	gcc -c fs_alloc.h -o fs_alloc.o

example: example.c
	gcc example.c -o example

clean:
	rm -f *.o example

format:
	clang-format -i --style=LLVM *.c *.h
