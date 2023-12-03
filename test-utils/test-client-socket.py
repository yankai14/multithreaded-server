import socket

# Server address and port
server_address = ('127.0.0.1', 8080)

# Create a socket object
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the server
client_socket.connect(server_address)

try:
    # Send data to the server
    message = "Hello, server!"
    client_socket.sendall(message.encode())

    while True:
        print()

finally:
    # Close the socket
    client_socket.close()