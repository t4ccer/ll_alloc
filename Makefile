CC?=gcc

fs_alloc.o: fs_alloc.h
	$(CC) -Wall -Wextra -nostdlib -c fs_alloc.h -o fs_alloc.o

ll_alloc.o: ll_alloc.h
	$(CC) -Wall -Wextra -nostdlib -c ll_alloc.h -o ll_alloc.o

example: ll_alloc.o fs_alloc.o example.c
	$(CC) -Wall -Wextra example.c -o example

clean:
	rm -f *.o example

format:
	nix fmt
	clang-format -i --style=LLVM *.c *.h
