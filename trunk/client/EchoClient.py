#!/lusr/bin/python
import socket

bufferSize = 100

clientSkt = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
clientSkt.connect(("pentax.cs.utexas.edu", 7775))
clientSkt.send('Hello world') 
receivedData = clientSkt.recv(bufferSize)

print 'Got ' , receivedData, " from server\n"



