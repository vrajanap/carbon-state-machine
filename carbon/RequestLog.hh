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

//This is a shared data object used by queue servers
//This stores the log of all the requests received and also the hashmap
//containing client ip address and the public key file name

#ifndef REQUESTLOG_HH
#define REQUESTLOG_HH

#include <fstream>
#include <boost/shared_ptr.hpp>
#include <lagniappe/Mutex.h>
#include <assert.h>

using namespace std;

namespace queue_log_records 
{
  class RequestLog
  {
    lagniappe::Mutex * mutex;
    requestTypes::NetRequest *dataElement;
  public:
    RequestLog(lagniappe::Mutex * m, requestTypes::NetRequest *data) : mutex(m), dataElement(data) {}
    RequestLog() {assert(false);}
    inline void recordRequest (requestTypes::NetRequest *newRequest) { 
	assert(mutex != NULL); 
	mutex->lock();    
	dataElement = newRequest; 
	mutex->unlock(); 
    } 
    inline requestTypes::NetRequest *  playRequest () { 
	assert(mutex != NULL); 
	mutex->lock(); 
	requestTypes::NetRequest *ret = dataElement; 
 	mutex->unlock(); 
	return ret; 
    }
 
  };

  typedef boost::shared_ptr<RequestLog> RequestLog_p;
}

namespace utilities 
{
  class Lock
  {
    lagniappe::Mutex * mutex;
    uint32_t lockVariable;
  public:
    Lock(lagniappe::Mutex * m) : mutex(m), lockVariable(0) {}
    Lock() {assert(false);}
    inline void set () { 
	assert(mutex != NULL); 
	mutex->lock();    
	lockVariable = 1 ; 
	mutex->unlock(); 
    } 
    inline uint32_t state () { 
	assert(mutex != NULL); 
	mutex->lock(); 
	uint32_t ret = lockVariable; 
 	mutex->unlock(); 
	return ret; 
    } 
   inline void reset() { 
        assert(mutex != NULL);
     	mutex->lock(); 
	lockVariable = 0;
  	mutex->unlock();
   }  
  };
  typedef boost::shared_ptr<Lock> Lock_p;
  
  class ClientKeyHash 
  {
    std::map<std::string, std::string> clientKeyHash; 
    public:
      ClientKeyHash(std::string keyMapFile) {
        try {
          char ipAddress[200], clientFileName[200];
          ifstream input;
          input.open(keyMapFile.c_str());
          cout<<std::endl<<"IP Address \t FileName"<<std::endl;
          while(input >> ipAddress && input >> clientFileName) 
          {
            cout <<ipAddress<<"\t"<<clientFileName<<std::endl;
            clientKeyHash[ipAddress] = clientFileName;
          }
        } catch(...) {
          std::cerr <<"Error while opening keyMapFile";
        }
      }
      ClientKeyHash() {assert(false);}
      std::string getKeyFileName(std::string ipAddress) {
        return clientKeyHash[ipAddress];
      }
  };
  typedef boost::shared_ptr<ClientKeyHash> ClientKeyHash_p;
}

#endif // REQUESTLOG_HH
