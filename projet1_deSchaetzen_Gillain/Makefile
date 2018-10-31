CFLAGS += -std=gnu99  -Wall -Werror -Wshadow -O2 -D_FORTIFY_SOURCE=2 -fstack-protector-all -D_POSIX_C_SOURCE=201112L -D_XOPEN_SOURCE -g
CC = gcc
# Default target
all: clean sender receiver test_queue_node
# If we run make debug instead, keep the debug symbols for gdb
# and define the DEBUG macro.
#debug: CFLAGS += -g -DDEBUG -Wno-unused-parameter -fno-omit-frame-pointer

# We use an implicit rule to build an executable named 'chat'
sender: src/sender.o src/tools/pkt.o src/tools/connect.o src/tools/queue_pkt.o
	@$(CC) -o  $@ $^ -lz

receiver: src/receiver.o  src/tools/pkt.o src/tools/connect.o src/tools/queue_pkt.o
	@$(CC) -o  $@ $^ -lz

test_queue_node: tests/test_queue_node.o src/tools/pkt.o src/tools/connect.o src/tools/queue_pkt.o
	@$(CC) -o  $@ $^ -lz
src/tools/pkt.o: src/tools/pkt.c
	@$(CC) $(CFLAGS)  -c -o  $@ $<

src/tools/connect.o: src/tools/connect.c
	@$(CC) $(CFLAGS) -c -o  $@ $<

src/tools/queue_pkt.o: src/tools/queue_pkt.c
	@$(CC) $(CFLAGS) -c -o  $@ $<

src/receiver.o: src/receiver.c
		@$(CC) $(CFLAGS) -c -o  $@ $<

src/sender.o: src/sender.c
		@$(CC) $(CFLAGS) -c -o  $@ $<

tests/test_queue_node.o: tests/test_queue_node.c
		@$(CC) $(CFLAGS) -c -o  $@ $<

.PHONY: clean

receive:
	./receiver :: 9090 > fileout.txt

send:
	./sender ::1 9090 < file.txt

valgrind_receive:
	valgrind --leak-check=full --show-leak-kinds=all ./receiver :: 9090 > fileout.txt

valgrind_send:
	valgrind --leak-check=full --show-leak-kinds=all ./sender ::1 9090 < file.txt

test_queue:
	./test_queue_node

test_encode_decode:
	./encode_decode

test:
	./test.sh

clean:
	@rm -rf src/tools/pkt.o src/tools/connect.o src/tools/queue_pkt.o tests/test_queue_node.o

mrproper:
	@rm -rf sender receiver

rebuild: clean mrproper sender receiver

.PHONY: clean mrproper rebuild
	# Default target
