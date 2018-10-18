CFLAGS += -std=gnu99  -Wall -Werror -Wshadow -O2 -D_FORTIFY_SOURCE=2 -fstack-protector-all -D_POSIX_C_SOURCE=201112L -D_XOPEN_SOURCE -lz
CC = gcc
# Default target
all: clean receiver sender
# If we run make debug instead, keep the debug symbols for gdb
# and define the DEBUG macro.
#debug: CFLAGS += -g -DDEBUG -Wno-unused-parameter -fno-omit-frame-pointer
debug: clean sender receiver

# We use an implicit rule to build an executable named 'chat'
sender: out/sender.o out/pkt.o out/connect.o out/queue_pkt.o
	@$(CC) -o $@ $^

receiver: out/receiver.o  out/pkt.o out/connect.o out/queue_pkt.o
	@$(CC) -o $@ $^

out/pkt.o: src/tools/pkt.c
	@$(CC) $(CFLAGS) -c -o $@ $<

out/connect.o: src/tools/connect.c
	@$(CC) $(CFLAGS) -c -o $@ $<

out/queue_pkt.o: src/tools/queue_pkt.c
	@$(CC) $(CFLAGS) -c -o $@ $<

out/receiver.o: src/receiver.c
		@$(CC) $(CFLAGS) -c -o $@ $<

out/sender.o: src/sender.c
		@$(CC) $(CFLAGS) -c -o $@ $<


clean:
	rm -rf *.o

mrproper:
	rm -rf sender receiver

rebuild: clean mrproper sender receiver

.PHONY: clean mrproper rebuild
