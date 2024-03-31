COMPILER = gcc
client.out: client.o client_server.o helper.o
	${COMPILER} $^ -o client.out

client.o: client_server.o helper.o
	${COMPILER} -c client.c -o $@ -w
 
client_server.o:helper.o
	${COMPILER} -c client_server.c -o $@ -w
helper.o:
	${COMPILER} -c helper.c -o $@ -w

clean:
	#clean .o and client.out 
	rm -f *.o client.out
