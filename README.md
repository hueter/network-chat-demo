# CS 372 Intro to Computer Networking | Project 1 - Simple Chat Program
#### by Michael Hueter

## Instructions

### Quick Setup

1. First, compile the chat client using the following command `gcc -Wall -o chatclient chatclient.c`
2. Second, run the Python server by typing `python chatserve.py -p 1234` where `1234` is the port number in this case.
3. Third, run the chat client by typing `./chatclient 192.168.4.247 1234` where `192.168.4.247` is the host address and `1234` is the host port.

### Details
* The python server takes two arguments, `-p` / `--port` is required, it specifies the port number on which the server runs, and
`-ha` / `--handle` is an optional argument that specifies the user handle for the server. If no handle is specified, 
the server user automatically defaults to `Demo` as the handle.

* In order to get the `hostname` argument for the C client, the Python server will tell you when you start it up the address on which it's starting, like so:
> Chat server listening on host '192.168.4.247' port 1234...

* The clients have to chat one at a time. The server client gets to chat first.

* The python server will tell you when a client has connected like so:
> ! Connection established using port 1234...

In summary, the python server is run by `python chatserve.py -p <portNumber> [-ha <handle]` and the C client is `./chatclient <hostname> <portNumber>`
