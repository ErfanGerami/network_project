COMPILER = gcc
server.out: server.o server_client.o helper.o threadpool.o myqueue.o
	${COMPILER} $^ -o parserver.out
server.o: server_client.o helper.o myqueue.o threadpool.o
	${COMPILER} -c server.c -o $@ -w

server_client.o: helper.o myqueue.o threadpool.o
	${COMPILER} -c server_client.c -o $@ -w
helper.o:
	${COMPILER} -c helper.c -o $@ -w
threadpool.o: myqueue.o
	${COMPILER} -c threadpool.c -o $@
myqueue.o:
	${COMPILER} -c myqueue.c -o $@

clean:
	#clean .o and parserver.out
	rm -f *.o parserver.out
