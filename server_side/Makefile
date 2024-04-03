COMPILER = gcc
server.out: make_directory server.o server_client.o helper.o
	${COMPILER} ./out/server.o ./out/server_client.o ./out/helper.o -o ./out/server.out
make_directory:
	mkdir -p ./out
server.o: make_directory server_client.o helper.o
	${COMPILER} -c server.c -o ./out/$@ -w

server_client.o:make_directory helper.o
	${COMPILER} -c server_client.c -o ./out/$@ -w 
helper.o:make_directory
	${COMPILER} -c helper.c -o ./out/$@ -w

clean:
	#clean .o and server.out
	rm -f *.o ./out
	
