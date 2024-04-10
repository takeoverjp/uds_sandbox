all: server client
.PHONY: all

clean:
	rm -f server client

.PHONY: clean

server: server.c
	gcc -o $@ $^ -g -Og -Werror

client: client.c
	gcc -o $@ $^ -g -Og -Werror
