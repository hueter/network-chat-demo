#!/usr/bin/python

"""
Michael Hueter
CS 372 Networking Fall 2016
Program 1 - Server Component
30 October 2016

References:
[1] https://docs.python.org/2/howto/sockets.html
"""

import argparse
import signal
import socket
import sys


def close_server(signal, frame):
    """
    This function is a more polite way to handle Ctrl + C,
    presumably invoked while the server 'while True:' was running
    """
    print("\nGracefully shutting down server...\nBye!\n")
    sys.exit(0)


def user_input():
    """
    Using the argparse module, get/validate command-line input from the user
    """
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", "-p", help="specify the port number for server to run on",
                        type=int, required=True)
    parser.add_argument("--handle", "-ha", help="specify your user chat handle",
                        type=str, default="Demo")
    args = parser.parse_args()
    port = args.port
    handle = args.handle
    setup_socket(socket_port=port, handle=handle)


def receive_message(connection=None):
    """
    This function takes a connection object. It allows the server to receive_message
    messages from the client and checks to make sure the message is not a 'quit' instruction
    """
    # client message up to 500 characters, plus 10-character handle and '> '
    client_message = connection.recv(513)

    # break the connection if client says \quit
    if "\\quit" in client_message:
        return False

    return client_message


def send_message(connection=None, handle=None):
    """
    This function takes a connection object and enables the server user to enter input
    and send it to the client
    """
    # get our message (we're the server) then format it with our handle
    server_message = raw_input("{0}> ".format(handle))
    message_with_handle = "{0}> {1}".format(handle, server_message)

    if server_message == "\\quit":
        connection.send(server_message)
        return False

    connection.send(message_with_handle)
    return True


def setup_socket(socket_port=None, handle=None):
    """
    This function configures and builds our socket using the user-specified port
    and the gethostname() method
    """
    if not socket_port:
        print("Error, no socket port was specified.")
        sys.exit(1)

    # instantiate INET streaming socket
    my_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    hostname = socket.gethostname()

    # bind accepts a two-tuple of host and port number
    my_socket.bind((hostname, socket_port))
    # setup listening for up to 5 connections
    my_socket.listen(5)

    # when finished start the server
    serve(server_socket=my_socket, handle=handle)


def serve(server_socket=None, handle=None):
    """
    This function sets up the server to accept connections
    """
    if not server_socket:
        print("Server Error, no socket was given.\n")
        sys.exit(1)

    if not handle:
        print("You need a user handle to chat.\n")
        sys.exit(1)

    # Our server is an infinite loop, until someone says to shut it down
    while True:

        print("Chat server listening on host '{0}' port {1}...\n"
              .format(server_socket.getsockname()[0], server_socket.getsockname()[1]))
        connection, address = server_socket.accept()
        new_connection = True

        while new_connection:
            # Listen for an initial message from the client containing a port number
            initial_message_with_port = connection.recv(10)
            print("! Connection established using port {0}...\n".format(initial_message_with_port))
            continue_connection = True

            while continue_connection:
                # send message to client and check if we quit
                sent_message = send_message(connection=connection, handle=handle)
                if not sent_message:
                    print("You have ended the connection. Exiting.")
                    new_connection = False
                    break

                # receive message from client and check if they quit
                message_received = receive_message(connection=connection)
                if message_received:
                    print(message_received)
                else:
                    print("Client closed the connection. Exiting.")
                    new_connection = False
                    break

        # server is closing
        connection.close()


if __name__ == "__main__":
    """
    This is the main function, aka entrypoint to the whole script
    """
    # setup signal to listen to ctrl + c for exit
    signal.signal(signal.SIGINT, close_server)
    # call user input to get started
    user_input()
