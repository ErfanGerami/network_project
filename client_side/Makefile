COMPILER = gcc
client.out: make_directory client.o client_server.o helper.o
	${COMPILER} ./out/client.o ./out/client_server.o ./out/helper.o -o ./out/client.out -w
make_directory:
	mkdir -p ./out
client.o:  make_directory client_server.o helper.o
	${COMPILER} -c client.c -o ./out/$@ -w
 
client_server.o: make_directory helper.o
	${COMPILER} -c client_server.c -o ./out/$@ -w
helper.o: make_directory
	${COMPILER} -c helper.c -o ./out/$@ -w

clean:
	#clean .o and client.out 
	rm -r ./out
