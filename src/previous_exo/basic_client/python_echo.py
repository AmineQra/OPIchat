#!/usr/bin/env python3

import socket
import sys


def main():
    # Check if the correct number of arguments has been passed
    if len(sys.argv) != 3:
        print(
            "Usage: python {} SERVER_IP SERVER_PORT".format(sys.argv[0]),
            file=sys.stderr,
        )
        sys.exit(1)

    # Create a server socket and set its options
    listening_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listening_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    # Bind the socket to a port and start listening for connections
    listening_socket.bind((sys.argv[1], int(sys.argv[2])))
    listening_socket.listen(10)

    while True:
        # Accept a client and communicate with it
        client_socket, _ = listening_socket.accept()
        msg = client_socket.recv(4096)

        # Communicate while the client does not close the connection
        while len(msg):
            print(msg)
            client_socket.send(b"<OK> " + msg)
            msg = client_socket.recv(4096)

        client_socket.close()


if __name__ == "__main__":
    main()
