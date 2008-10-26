#!/lusr/bin/python
import socket
def main():
  bufferSize = 1000

  clientSkt = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
  clientSkt.connect(("nikon.cs.utexas.edu", 7775))
  #clientSkt.send('Google on Tuesday released the open-source code which powers its Android mobile operating system and invited outside programmers to tinker with the software to develop their own features.VC5635771B2A84F8764ACE0D252BD93977C3BB2A9EC96DF730742DA4C507130B31EEC6F9D5289F1350532A765017145B0DD894C1A043755D7093FE5CE88FF3002') 
  clientSkt.send('dum aloo, rajma, tomato onion curry, sambar, pulav, raita, curd rice, ice-cream, strawberries. How does that sound for a dinner? :DV455D04D344B54A57E5781E6DF66CD5853E7357E682534944A21E45D536EED5606E625075535C0FE490EFC1C4CC1CBDDFF9BA1336436E18694C1F37FC4D27F95C');
  receivedData = clientSkt.recv(1000)
  print 'Got ' , receivedData, " from server\n"

main()


