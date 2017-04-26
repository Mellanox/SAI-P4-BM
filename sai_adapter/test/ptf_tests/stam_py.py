import socket

print socket.getaddrinfo('127.0.0.1', 9092, socket.AF_INET, socket.SOCK_STREAM,
                         0, socket.AI_PASSIVE | socket.AI_ADDRCONFIG)