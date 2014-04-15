all: server client

server: server.c map.c
	gcc server.c map.c -o server

client: client.c
	gcc client.c -o client

clean: server client
	rm server client
