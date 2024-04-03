COMPILER = gcc
server.out: make_directory server.o server_client.o helper.o threadpool.o myqueue.o
	${COMPILER} ./out/server.o ./out/server_client.o ./out/helper.o ./out/threadpool.o ./out/myqueue.o -o ./out/parserver.out
make_directory:
	mkdir -p ./out
server.o: make_directory server_client.o helper.o myqueue.o threadpool.o
	${COMPILER} -c server.c -o ./out/$@ -w

server_client.o: make_directory helper.o myqueue.o threadpool.o
	${COMPILER} -c server_client.c -o ./out/$@ -w
helper.o:make_directory
	${COMPILER} -c helper.c -o ./out/$@ -w
threadpool.o: make_directory myqueue.o
	${COMPILER} -c threadpool.c -o ./out/$@ -w
myqueue.o: make_directory
	${COMPILER} -c myqueue.c -o ./out/$@ -w

clean:
	#clean .o and parserver.out
	rm -r out
