CC = gcc
CFLAGS = -Wall -pthread

%: %.c %.h
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: all clean

all: sorter_server sorter_client

clean:
@rm sorter_server sorter_client
