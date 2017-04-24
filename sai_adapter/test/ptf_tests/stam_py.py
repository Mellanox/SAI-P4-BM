import socket

print socket.getaddrinfo('localhost', 9092, socket.AF_UNSPEC, socket.SOCK_STREAM,
                         0, socket.AI_PASSIVE | socket.AI_ADDRCONFIG)