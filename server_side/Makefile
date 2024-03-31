COMPILER = gcc
server.out: server.o server_client.o helper.o
	${COMPILER} $^ -o server.out
server.o: server_client.o helper.o
	${COMPILER} -c server.c -o $@ -w

server_client.o:helper.o
	${COMPILER} -c server_client.c -o $@ -w 
helper.o:
	${COMPILER} -c helper.c -o $@ -w

clean:
	#clean .o and server.out
	rm -f *.o server.out
