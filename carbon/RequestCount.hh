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

//This is a shared data object. Used by echoserver to keep track of the messages
//received from queue replicas

#ifndef REQUESTCOUNT_HH
#define REQUESTCOUNT_HH

#include <boost/shared_ptr.hpp>
#include <lagniappe/Mutex.h>
#include <assert.h>
#include <iostream>

#define NUM_QUEUE_SERVERS 3 

using namespace std;

namespace request_counter 
{
  class RequestCount
  {
    lagniappe::Mutex * mutex;
    std::map<uint32_t, uint32_t*> messageHash;  
    uint32_t counter;
  public:
    RequestCount(lagniappe::Mutex * m) : mutex(m) {}
    RequestCount() {assert(false);}

    inline bool checkIfPresent(uint32_t messageID, uint32_t queueServerID)
    {
       assert(mutex != NULL);
       mutex->lock();
       bool ret = checkIfPresentNoLock(messageID, queueServerID);
       mutex->unlock();
       return ret;
    }
    inline bool checkIfPresentNoLock(uint32_t messageID, uint32_t queueServerID)
    {
        bool ret;
        if (NULL == messageHash[messageID])
        {
          ret = false; 
        }
        else
        {
          uint32_t* queueServers = messageHash[messageID]; 
	  if(0 != queueServers[queueServerID] ) 
                ret = true;
	  else 
		ret = false;
        }
        return ret;
    }
    inline void insertMsgHash(uint32_t messageID, uint32_t queueServerID)
    { 
        assert(mutex != NULL);
	assert(queueServerID < NUM_QUEUE_SERVERS);  
	mutex->lock();    
	
	if(NULL == messageHash[messageID]) 
        {
		messageHash[messageID] = new uint32_t[NUM_QUEUE_SERVERS];
		// uint32_t *p_qservers = messageHash[messageID]; 
		for (int index=0; index< NUM_QUEUE_SERVERS; ++index) 
			messageHash[messageID][index] = 0;   
	}
	messageHash[messageID][queueServerID] = 1;
 
	mutex->unlock(); 
    }
    inline bool receivedAllRequests(uint32_t messageID)
    {
       assert(mutex != NULL) ;
       mutex->lock();
       bool ret = receivedAllRequestsNoLock(messageID);
       mutex->unlock();
       return ret;
    }
    inline bool receivedAllRequestsNoLock(uint32_t messageID) 
    {
	bool ret = true;
        int index;
	for (index =0; index < NUM_QUEUE_SERVERS; ++index) 
	{
		if(checkIfPresentNoLock(messageID, index) == false) 
		{
			ret = false; 
			break;
		}
	}
	return ret;
    }
    inline bool processedAllRequests()
    {
       assert(mutex != NULL) ;
       mutex->lock();
       bool ret = true;
       for ( std::map<uint32_t, uint32_t*>::const_iterator it = messageHash.begin();
                it != messageHash.end();
                ++it)
        {
                ret = receivedAllRequestsNoLock(it->first);
                if(false == ret)
                  break;
        }
       mutex->unlock();
       return ret;
    }

    inline void resetHash() 
    {
	for ( std::map<uint32_t, uint32_t*>::const_iterator it = messageHash.begin();
		it != messageHash.end(); 
		++it)
	{
		free(it->second);
	}
	messageHash.clear(); 
    }
 
  };
 
  typedef boost::shared_ptr<RequestCount> RequestCount_p;

}

#endif // REQUESTCOUNT_HH
