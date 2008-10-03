// Copyright (C) 2008 Taylor L. Riche <riche@cs.utexas.edu>
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// $Id: NetRequestDevice.cc 952 2008-09-23 21:42:37Z djwhitebread $
//

#include "NetRequestDevice.hh"

namespace requestDevices
{

int NetRequestDevice::procNum = 10;


NetRequestDevice::NetRequestDevice(std::string intf) : fromSet(false), intfName(intf), sysReady(false)
{
  listener = socket(PF_INET, SOCK_STREAM, 0);
  sockaddr_in my_addr;
  int yes = 1;
  if(listener == -1) {
    std::cerr << "Problem opening socket:" << strerror(errno) << std::endl;
    exit(1);
  }
  if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    std::cerr << "Problem setting socket options: " << strerror(errno) << std::endl;
    exit(1);
  }

  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(LAGNIAPPE_PORT);
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);
  
  unsigned int bindReturn = bind(listener, (struct sockaddr *)&my_addr, sizeof(my_addr));
  if(bindReturn != 0) {
    close(listener);
    std::cerr << "Problem with binding" << strerror(errno) << std::endl;
    exit(1);
  }

  {
    Synchronized::Scope mutex(this);
    fromSet = true;
    notifyAll();
  }
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if(pthread_create(&coreThread, &attr, startDevice,(void*) this)) {
    std::cerr << "pthread_create failed" << strerror(errno) << std::endl;
    std::exit(1);
  }
//   cpu_set_t set;
//   CPU_ZERO(&set);
//   CPU_SET(procNum, &set);
//   std::cout << "Setting affinity of thread " << coreThread << " to proc " << procNum << "." << std::endl;
//   if(pthread_setaffinity_np(coreThread, sizeof(set), &set) != 0) {
//     std::cerr << "Affinity not set: " << strerror(errno) << std::endl;
//   }
}

NetRequestDevice::~NetRequestDevice() 
{
  for(int i = 0; i <= maxFD; ++i) {
    if(FD_ISSET(i, &openSet)) {
      close(i);
    }
  }
}

void 
NetRequestDevice::process() 
{
  {
    Synchronized::Scope mutex(this);
    while(!sysReady) {
      wait();
    }
  }
  fd_set readSet;
  sockaddr_in remoteAddr;
  socklen_t addrlen;
  // setup listener socket to listen for new connections
  if(listen(listener, 500) == -1) {
    std::cerr << "Listening error: " << strerror(errno) << std::endl;
    exit(1);
  }
  FD_SET(listener, &openSet);
  maxFD = listener;
  while(true) {
    readSet = openSet;
    if(select(maxFD+1, &readSet, NULL, NULL, NULL) == -1) {
      if(errno != EINTR) {
	std::cerr << "Select error: " << strerror(errno) << std::endl;
	exit(1);
      }
    }
    for(socket_t i = 0; i <= maxFD; ++i) {
      if(FD_ISSET(i, &readSet)) {
	if (i == listener) {
	  // new connection
	  addrlen = sizeof(remoteAddr);
	  socket_t newSock;
	  if((newSock = accept(listener, (struct sockaddr *)&remoteAddr, &addrlen)) == -1) {
	    std::cerr << "Accept error: " << strerror(errno) << std::endl;
	    exit(1);
	  }
	  else {
	    int yes = 1;
	    if (setsockopt(newSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
	      std::cerr << "Problem setting socket options: " << strerror(errno) << std::endl;
	      exit(1);
	    }
	    FD_SET(newSock, &openSet);
	    if(newSock > maxFD) {
	      maxFD = newSock;
	    }
	    //std::cout << "NetRequestDevice: new connection from " << inet_ntoa(remoteAddr.sin_addr) << " on socket " << newSock << std::endl;
	  }
	}
	else {
	  // data from an established connection
	  char * buffer = new char[BUFSIZE];
	  int numBytes=0;
	  if((numBytes = recvfrom(i, buffer, BUFSIZE, 0, (sockaddr *)&remoteAddr, &addrlen)) <= 0) {
	    if(numBytes == 0) {
	      shutdown(i, SHUT_RDWR);
	      if(close(i) < 0) {perror("Close");}
	      FD_CLR(i, &openSet);
	      delete[] buffer;
	    }
	    else {
	      std::cerr << "Receive error: " << strerror(errno) << std::endl;
	      delete[] buffer;
	      //exit(1);
	    }
	  }
	  else {
	    // process data
	    std::pair<lagniappe::CoreOp *, std::string> * dest = destMap["out"];
	    if(dest == NULL) {
	      delete[] buffer;
	    }
	    else if(dest->first == NULL) {
	      delete[] buffer;
	    }
	    else {
	      // actually send out an RCon
	      assert(buffer != NULL);
	      requestTypes::NetRequest * netReq = new requestTypes::NetRequest(buffer, numBytes, i, remoteAddr.sin_addr, ntohs(remoteAddr.sin_port));
	      assert(i>= 0);
	      assert(i<=maxFD);
	      assert(netReq->getSocketID()>=0);
	      assert(netReq->getSocketID()<=maxFD);
	      lagniappe::RCon * newRCon = new lagniappe::RCon(dest->first, static_cast<lagniappe::RData *>(netReq), dest->second, sizeof(netReq));
	      dest->first->run(newRCon);
	    }
	  }
	}
      }
    }
  }
  return;
}

void 
NetRequestDevice::run(lagniappe::RCon * rcon)
{
  // simply send data out of socket
  requestTypes::NetRequest * req = static_cast<requestTypes::NetRequest *>(rcon->getRData());
  assert(req != NULL);
  socket_t reqSock = req->getSocketID();
  if(send(reqSock, req->getBuffer(), req->getNumBytes(), 0) == -1) {
    std::cerr << "Send error: " << strerror(errno) << std::endl;
  }
  // clean up
//   if(req->getBuffer() != NULL)
//     delete[] req->getBuffer();
  delete req;
  delete rcon;
  return;
}

void 
NetRequestDevice::start()
{
  Synchronized::Scope mutex(this);
  sysReady = true;
  notifyAll();
  return;
}

}
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void * 
startDevice(void * arg)
{

//  ** OLD TEMPLATE BASED CODE **	
//  MethodWrapper<ProcElement> * mw = static_cast<MethodWrapper<ProcElement> *>(arg);
//  ProcElement* obj = mw->object;
//  void(ProcElement::*mthd)() = mw->method;
//  delete mw;
//  (obj->*mthd)();
//  return NULL;
//  ** OLD TEMPLATE BASED CODE **	

  requestDevices::NetRequestDevice * nic = static_cast<requestDevices::NetRequestDevice *>(arg);
	nic->process();
	return NULL;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
