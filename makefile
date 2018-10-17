CFLAGS += -std=c99 # Define which version of the C standard to use
CFLAGS += -Wall # Enable the 'all' set of warnings
CFLAGS += -Werror # Treat all warnings as error
CFLAGS += -Wshadow # Warn when shadowing variables
CFLAGS += -Wextra # Enable additional warnings
CFLAGS += -O2 -D_FORTIFY_SOURCE=2 # Add canary code, i.e. detect buffer overflows
CFLAGS += -fstack-protector-all # Add canary code to detect stack smashing
CFLAGS += -D_POSIX_C_SOURCE=201112L -D_XOPEN_SOURCE # feature_test_macros for getpot and getaddrinfo


CC = gcc


# Default target
all: clean receiver sender

# If we run make debug instead, keep the debug symbols for gdb
# and define the DEBUG macro.
debug: CFLAGS += -g -DDEBUG -Wno-unused-parameter -fno-omit-frame-pointer
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

.PHONY: clean

clean:
	@rm -rf out/*

mrproper:
	@rm -rf sender receiver

rebuild: clean mrproper sender receiver

.PHONY: clean mrproper rebuild
