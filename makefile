CC = gcc
CFLAGS = -I. -pthread -Wall -Wextra -pthread

vm: vm.o
	$(CC) $(CFLAGS) -o $@ $^

vm.o: vm.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f vm *.o
