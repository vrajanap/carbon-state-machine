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

#define SIG_LENGTH 128
#include <string>


#include "modes.h"
#include "hex.h"
#include "filters.h"
#include "osrng.h"
#include "sha.h"
#include "hmac.h"
#include <string>
#include "hex.h"
#include <lagniappe/Operator.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <boost/functional/hash.hpp>

#include <iostream>
using namespace std;

typedef int socket_t;

namespace requestTypes
{

  class NetRequest
  {
  private:
    char * buffer;
    char * signature;
    byte * mac;
    int numBytes;
    socket_t socketID;
    std::string fileName;
    bool close;
    in_addr srcAddr;
    uint16_t srcPort;
    uint32_t messageID;
    uint32_t queueServerID;
    
    inline void separateSig() {
      if(strlen(buffer) > SIG_LENGTH) {
        buffer[strlen(buffer) - SIG_LENGTH-1] = '\0';
        signature = buffer + strlen(buffer) + 1;
        setNumBytes(strlen(buffer));
      } else {
        signature = NULL;
      }
    }
  public:
    NetRequest(NetRequest *data, uint16_t queueServerID) {
       char *str = data->getBuffer(); 
       buffer = (char *) malloc((strlen(str)+1) * sizeof(char));
       strcpy(buffer, str);
       
       char *sig = data->getSignature();
       signature = (char *) malloc((strlen(sig)+1) * sizeof(char));
       strcpy(signature, sig); 

       numBytes = data->getNumBytes();
       socketID = data->getSocketID();
       fileName = data->getFileName();
       close = data->getClose();
       srcAddr = data->getSrcAddr();
       srcPort = data->getSrcPort();
       messageID = data->getMessageID();
       this->queueServerID = queueServerID; 
    }
    NetRequest(NetRequest *data, uint16_t queueServerID, const byte *macRepl) {
       char *str = data->getBuffer(); 
       if(str != NULL) {
        buffer = (char *) malloc((strlen(str)+1) * sizeof(char));
        strcpy(buffer, str);
       }
       
       char *sig = data->getSignature();
       if(sig != NULL) {
         signature = (char *) malloc((strlen(sig)+1) * sizeof(char));
         strcpy(signature, sig); 
       }
       if(macRepl != NULL) {
        mac = (byte *) malloc((CryptoPP::SHA::DIGESTSIZE+1) * sizeof(byte));
       
        int i;
        for(i=0; i<CryptoPP::SHA::DIGESTSIZE; i++)
          mac[i] = macRepl[i];
        mac[i] = '\0';
       }
       
       socketID = data->getSocketID();
       fileName = data->getFileName();
       close = data->getClose();
       srcAddr = data->getSrcAddr();
       srcPort = data->getSrcPort();
       messageID = data->getMessageID();
       this->queueServerID = queueServerID; 
    }
    
    NetRequest(char * b, int n, socket_t s) : buffer(b), 
					      numBytes(n), 
					      socketID(s), 
					      fileName(""), 
					      close(false) {
                                              separateSig(); 
                                              }
    NetRequest(char * b, int n, socket_t s, in_addr saddr, uint16_t p) : buffer(b), 
							      numBytes(n), 
							      socketID(s), 
							      fileName(""), 
							      close(false),
							      srcAddr(saddr),
							      srcPort(p) {
                                                                separateSig();
                                                              }
    NetRequest(char * b, int n, socket_t s, std::string fn) : buffer(b), 
							      numBytes(n), 
							      socketID(s), 
							      fileName(fn), 
							      close(false) {
                                                                separateSig();
                                                              }
    virtual ~NetRequest() {if(buffer != NULL) delete[] buffer;}
    
    void freeSignature() { free(signature); signature=NULL;}
    void freeMac() { free(mac); mac = NULL;}
    inline socket_t getSocketID() {return socketID;}
    inline lagniappe::FlowID getSourceID() {boost::hash<uint32_t> bh; return bh(socketID + srcPort + fileName.length());}
    inline char * getBuffer() {return buffer;}
    inline char * getSignature() {return signature; }
    inline byte * getMac() {return mac; }
    inline int getNumBytes() {return numBytes;}
    inline void setNumBytes(int len){numBytes = len;}
    inline std::string getFileName() {return fileName;}
    inline void setFileName(std::string s) {fileName = s;}
    lagniappe::FlowID getFileInfoID();
    inline void setClose(bool c) {close = c;}
    inline bool getClose() const {return close;}
    inline in_addr getSrcAddr() const {return srcAddr;}
    inline uint16_t getSrcPort() const {return srcPort;}
    inline uint16_t getMessageID() const {return messageID;}
    inline void setMessageID(uint32_t messageID) {this->messageID = messageID;} 
    inline void setQueueServerID(uint32_t queueServerID) {this->queueServerID = queueServerID;}
    inline uint32_t getQueueServerID() const{return queueServerID;}   
  };
  
}
#endif
