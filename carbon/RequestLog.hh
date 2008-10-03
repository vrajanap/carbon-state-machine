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

#ifndef REQUESTLOG_HH
#define REQUESTLOG_HH

#include <boost/shared_ptr.hpp>
#include <lagniappe/Mutex.h>
#include <assert.h>

namespace queue_log_records 
{
  class RequestLog
  {
    lagniappe::Mutex * mutex;
    std::string dataElement; 
    uint32_t counter;
  public:
    RequestLog(lagniappe::Mutex * m) : mutex(m), dataElement(""), counter(0) {}
    RequestLog() {assert(false);}
    inline void recordRequest (std::string newRequest) { 
	assert(mutex != NULL); 
	mutex->lock();    
	dataElement = newRequest; 
	mutex->unlock(); 
    } 
    inline std::string playRequest () { 
	assert(mutex != NULL); 
	mutex->lock(); 
	std::string ret = dataElement; 
 	mutex->unlock(); 
	return ret; 
    } 
 
  };

  typedef boost::shared_ptr<RequestLog> RequestLog_p;

}

#endif // REQUESTLOG_HH
