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
//

#define MAX_QUEUE_SIZE 5 

#include "lc_carbon_op_queue_Operator.h"

#include <iostream> 
using namespace std;

void 
carbon_op_queue::handleRequest(requestTypes::NetRequest * data, unsigned long dataSize)
{
  if(requestQueue.size() >= MAX_QUEUE_SIZE) 
  {
    cout<< "Queue :: Discarding the message" << std::endl;
    if( 0 ==  lockOnQueue->state() ) 
    { 
	   cout<< "Queue :: Sending wait"<<endl; 
           qwait((int *) 1, 0);
	   lockOnQueue->set() ; 
    } 
    return;
  }

  queue_log_records::RequestLog_p element
	(new queue_log_records::RequestLog(getNewMutex())); 
  
  requestQueue.push(element);  

  cout << "Queue :: Queue size is " << requestQueue.size() << std::endl;
  cout << "Queue :: Sending the string to echo server" <<std::endl; 

  out(data, dataSize);
  return;
  
}

void
carbon_op_queue::handleSignal(int*, unsigned long)
{
   cout << "Queue :: Deleting current elements in queue \n";
   
   while ( ! requestQueue.empty() ) 
   {
	requestQueue.pop();
   }

   lockOnQueue->reset(); 
}

requestTypes::NetRequest *
carbon_op_queue::geninRequest()
{
  std::string request = "GET /index.html HTTP/1.1";
  request += "\r\n\r\n";
  char * request_str = new char[request.length()];
  strcpy(request_str, request.c_str());
  //std::cerr << request_str << ":" << strlen(request_str) << ":" << request.length() << std::endl;
  return new requestTypes::NetRequest(request_str,strlen(request_str),0);

}

int *
carbon_op_queue::genqsignalRequest()
{
  std::string request = "GET /index.html HTTP/1.1";
  request += "\r\n\r\n";
  char * request_str = new char[request.length()];
  strcpy(request_str, request.c_str());
  //std::cerr << request_str << ":" << strlen(request_str) << ":" << request.length() << std::endl;
  return (int *) 1;
}

void
carbon_op_queue::my_install(void * data)
{
}

void *
carbon_op_queue::my_get()
{
  return NULL;
}

void
carbon_op_queue::my_purge(void * data)
{
}

void
carbon_op_queue::my_init()
{
   lockOnQueue = utilities::Lock_p( new utilities::Lock(getNewMutex()));
}



