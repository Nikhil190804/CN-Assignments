compile: concurrent_server.c concurrent_client.c single_thread_client.c single_thread_server.c
	gcc -w concurrent_server.c -o concurrent_server
	gcc -w concurrent_client.c -o concurrent_client
	gcc -w single_thread_client.c -o single_thread_client
	gcc -w single_thread_server.c -o single_thread_server

clean:
	rm -f concurrent_server concurrent_client single_thread_client single_thread_server