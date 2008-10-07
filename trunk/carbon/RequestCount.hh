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

#ifndef REQUESTCOUNT_HH
#define REQUESTCOUNT_HH

#include <boost/shared_ptr.hpp>
#include <lagniappe/Mutex.h>
#include <assert.h>

namespace request_counter 
{
  class RequestCount
  {
    lagniappe::Mutex * mutex;
    uint32_t counter;
  public:
    RequestCount(lagniappe::Mutex * m) : mutex(m), counter(0) {}
    RequestCount() {assert(false);}
    inline void setCount (uint32_t newCount) { 
	assert(mutex != NULL); 
	mutex->lock();    
	counter = newCount; 
	mutex->unlock(); 
    } 
    inline uint32_t getCount () { 
	assert(mutex != NULL); 
	mutex->lock(); 
	uint32_t ret = counter; 
 	mutex->unlock(); 
	return ret; 
    } 
    inline void incrementCounter() {
	assert(mutex != NULL); 
	mutex->lock();
	counter ++ ;
	mutex->unlock();
    }
   inline void resetCount() { 
        assert(mutex != NULL);
     	mutex->lock(); 
	counter = 0;
  	mutex->unlock();
   }  
  };
  typedef boost::shared_ptr<RequestCount> RequestCount_p;

}

#endif // REQUESTCOUNT_HH
