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

#ifndef NETREQUEST_HH_
#define NETREQUEST_HH_

#include <string>

#include <lagniappe/Operator.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <boost/functional/hash.hpp>


typedef int socket_t;

namespace requestTypes
{

  class NetRequest
  {
    char * buffer;
    int numBytes;
    socket_t socketID;
    std::string fileName;
    bool close;
    in_addr srcAddr;
    uint16_t srcPort;
  public:
    NetRequest(NetRequest *data) {
       char *str = data->getBuffer(); 
       buffer = (char *) malloc(strlen(str) * sizeof(char));
       strncpy(buffer, str, strlen(str));
       numBytes = data->getNumBytes();
       socketID = data->getSocketID();
       fileName = data->getFileName();
       close = data->getClose();
       srcAddr = data->getSrcAddr();
       srcPort = data->getSrcPort();
    }
    NetRequest(char * b, int n, socket_t s) : buffer(b), 
					      numBytes(n), 
					      socketID(s), 
					      fileName(""), 
					      close(false) {}
    NetRequest(char * b, int n, socket_t s, in_addr saddr, uint16_t p) : buffer(b), 
							      numBytes(n), 
							      socketID(s), 
							      fileName(""), 
							      close(false),
									 srcAddr(saddr),
									 srcPort(p) {}
    NetRequest(char * b, int n, socket_t s, std::string fn) : buffer(b), 
							      numBytes(n), 
							      socketID(s), 
							      fileName(fn), 
							      close(false) {}
    virtual ~NetRequest() {if(buffer != NULL) delete[] buffer;}
    inline socket_t getSocketID() {return socketID;}
    inline lagniappe::FlowID getSourceID() {boost::hash<uint32_t> bh; return bh(socketID + srcPort + fileName.length());}
    inline char * getBuffer() {return buffer;}
    inline int getNumBytes() {return numBytes;}
    inline std::string getFileName() {return fileName;}
    inline void setFileName(std::string s) {fileName = s;}
    lagniappe::FlowID getFileInfoID();
    inline void setClose(bool c) {close = c;}
    inline bool getClose() const {return close;}
    inline in_addr getSrcAddr() const {return srcAddr;}
    inline uint16_t getSrcPort() const {return srcPort;} 
  };
  
}
#endif
