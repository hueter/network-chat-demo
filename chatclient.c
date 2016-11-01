/**
 * Michael Hueter
 * CS 372 Networking Fall 2016
 * Program 1 - Server Component
 * 30 October 2016
 * References: http://www.linuxhowtos.org/C_C++/socket.htm
 *			   http://www.gnu.org/software/libc/manual/html_node/Host-Names.html
 *			   http://beej.us/guide/bgnet/
 *			   http://man7.org/linux/man-pages/man3/bcopy.3.html
 **/

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>

int initiateContact(char *host, int portNumber) {
	/**
	* This function takes a host name and port number and interfaces with
	*  the socket API to establish a socket and return a socket file descriptor 'newSocket'
	*/
	int newSocket;
	struct sockaddr_in serverAddress;
	struct hostent *server;

	// establish server exists
	server = gethostbyname(host);
	if (server == NULL) {
		fprintf(stderr, "Connection Error: Host not reachable.\n");
		exit(1);
	}

	// allocate memory for server
   	bzero((char *) &serverAddress, sizeof(serverAddress));
	// IPv4
    serverAddress.sin_family = AF_INET;
	// bcopy - copy byte sequence of server address 
    bcopy((char *)server->h_addr, (char *)&serverAddress.sin_addr.s_addr, server->h_length);
	// Convert multi-byte integer types from host byte order to network byte order 
	serverAddress.sin_port = htons(portNumber);

	// build a socket stream, returning a file descriptor
	newSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (newSocket == -1)  {
		fprintf(stderr, "Connection Error: Failed to establish socket stream.\n");
		exit(1);
	}

    // attempt to establish a connection to the server
    if(connect(newSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) == -1) {
		fprintf(stderr, "Connection Error: Failed to complete connection to host.\n");
		exit(1);
    }    

    return newSocket;
}


int sendMessage(int socketf, char handle[11]) {
	/**
	* This function takes the socket file descriptor and user handle as arguments
	*  It then prompts the user for input and forwards it to the server.
	*  Returns -1 if the message fails to send, 
	*  Returns 1 if the user types '/quit'
	*  Returns 0 if the message sends successfully
	*/	
    char userInput[501]; // 513 with handle and '> '
    char message[513]; // 501 with newline
	int writeResponse = 0;

    // initialize to empty
    memset((char *)userInput, '\0', sizeof(userInput));
    memset((char *)message, '\0', sizeof(message));
	
	// show handle and prompt user for message
	strcpy(message, handle); // append handle to message
	strcat(message, "> ");
	printf("\n%s", message); // print handle and entry line
    fgets(userInput, 501, stdin); 	// max 501 chars inclding newline
	userInput[strlen(userInput)-1] = '\0';  	// trim the newline
	strcat(message, userInput);   // append user input into message

	// if we want to quit
	if(strcmp(userInput, "\\quit") == 0) {
		writeResponse = send(socketf, &message, sizeof(message), 0);
		if (writeResponse < 0) {
	    	fprintf(stderr, "\nSocket Error: Error writing to socket.");
			return -1;
		}
		return 1;
	}

	// we don't want to quit, write a normal message to the server
	writeResponse = send(socketf, &message, sizeof(message), 0);
	if (writeResponse < 0) {
		fprintf(stderr, "\nSocket Error: Error writing to socket.");
		return -1;
	}

	return 0;
}


int receiveMessage(int socketf) {
    /**
	* This function takes the socket file descriptor as its only argument
	*  It then reads messages from the server
	*  Returns -1 if the message read fails
	*  Returns 1 if the server user types '/quit'
	*  Returns 0 if the message reads successfully
	*/	
    char serverMessage[513];
	int readStatus = 0;

    // allocate memory for server message
    memset((char *)serverMessage, '\0', sizeof(serverMessage));

    readStatus = recv(socketf, &serverMessage, sizeof(serverMessage), 0);
	if (readStatus < 0) {
		fprintf(stderr, "\nServer Error: Error reading message from server.");
		return -1;
    }

	// Check if \\quit is in the serverMessage
	if(strcmp(serverMessage, "\\quit") == 0) {
		return 1;
	}

	printf("\n%s", serverMessage);
	return 0;
}


int main(int argc, char **argv) {
    char handle[11];
	int activeSocket;
	int writeResponse = 0;
	char *serverPort;
	bool connected = true;
	int messageStatus = 0;

   	// Server hostname and port must be specified in the command line arguments
    if (argc < 3) {
		fprintf(stderr, "\nUsage: ./chatclient <server-hostname> <port#>");
		exit(1);
    }

	// set up the active socket connection
	activeSocket = initiateContact(argv[1], atoi(argv[2]));
	printf("Connected.\nNote: Server must send message before you can respond.");

	// server port as a string
	serverPort = argv[2];

	// grab user handle from input
	printf("\nEnter User Handle: ");
    fgets(handle, 11, stdin);
	// trim newline from handle
	handle[strlen(handle)-1] = '\0';
	printf("Chosen User Handle is: %s\n", handle);

	// send initial message with port number to server
	writeResponse = write(activeSocket, serverPort, strlen(serverPort));
    if (writeResponse < 0) {
		fprintf(stderr, "\nSocket Error: Error writing to socket.");
		exit(1);
	}

	while (connected) {
		messageStatus = receiveMessage(activeSocket);
		if (messageStatus == -1) {
			// server message read failure
			exit(1);
		} else if (messageStatus == 1) {
			// server initiates quit
			printf("The server user has closed the connection. Exiting.");
			connected = false;
			break;
		}
		messageStatus = sendMessage(activeSocket, handle);
		if (messageStatus == -1) {
			// client message send failure
			exit(1);
		} else if (messageStatus == 1) {
			// client initiates quit
			printf("Exiting.");
			connected = false;
			break;
		}
	}

	return 0;
}	