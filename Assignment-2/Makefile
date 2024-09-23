compile: server.c client.c
	gcc -w server.c -o server
	gcc -w client.c -o client

run-s: server
	taskset -c 0 ./server

run-c: client
	taskset -c 1 ./client 3

clean:
	rm -f server client