CC = gcc
CFLAGS = -I. -pthread -Wall -Wextra

vm: vm.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f vm vm_notlb vm256 vm_seq *.o
