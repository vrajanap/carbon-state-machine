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

#define KEYS_DIR "./keys/"
#define MAX_QUEUE_SIZE 5 
#define QUEUE_ID_REPL_1 1
#define QUEUE_ID_REPL_2 2
#define QUEUE_ID_REPL_3 3
#define QUEUE_NAME_PRIMARY "primary"
#include "lc_carbon_op_queue_repl_1_Operator.h"
#include "HMACSignVer.hh"

#include <iostream> 
using namespace std;

void 
carbon_op_queue_repl_1::handleRequest(requestTypes::NetRequest * data, unsigned long dataSize)
{
  std::string key = replicaKeyHash->getKey(QUEUE_NAME_PRIMARY); 
  if(!cripton::HMACSignVer::verifyMac(string(data->getBuffer()), key, data->getMac()))
  {
    cout<< "Repl Queue 1 :: MAC authentication with primary failed; Discarding the request" << std::endl;
    return;
  }

  data->freeMac();
  
  if(requestQueue.size() >= MAX_QUEUE_SIZE) 
  {
    cout<< "Repl Queue 1 :: Discarding the message" << std::endl;
    if( 0 ==  lockOnQueue->state() ) 
    { 
	   cout<< "Repl Queue 1 :: Sending wait"<<endl; 
           int *serverId = (int*) malloc(sizeof(int));
           *serverId = 1;
           qwait((int *) serverId, QUEUE_ID_REPL_1);
	   lockOnQueue->set() ; 
    } 
    return;
  }

  queue_log_records::RequestLog_p element
	(new queue_log_records::RequestLog(getNewMutex(), data)); 
  
  requestQueue.push(element);  

  cout << "Repl Queue 1 :: Queue size is " << requestQueue.size() << std::endl;
  cout << "Repl Queue 1 :: Sending the string to echo server" <<std::endl; 

  out(data, dataSize);
  return;
  
}

void
carbon_op_queue_repl_1::handleSignal(int*, unsigned long)
{
   cout << "Repl Queue 1 :: Deleting current elements in queue \n";
   
   while ( ! requestQueue.empty() ) 
   {
	requestQueue.pop();
   }

   lockOnQueue->reset(); 
}

requestTypes::NetRequest *
carbon_op_queue_repl_1::geninRequest()
{
  std::string request = "GET /index.html HTTP/1.1";
  request += "\r\n\r\n";
  char * request_str = new char[request.length()];
  strcpy(request_str, request.c_str());
  //std::cerr << request_str << ":" << strlen(request_str) << ":" << request.length() << std::endl;
  return new requestTypes::NetRequest(request_str,strlen(request_str),0);

}

int *
carbon_op_queue_repl_1::genqsignalRequest()
{
  std::string request = "GET /index.html HTTP/1.1";
  request += "\r\n\r\n";
  char * request_str = new char[request.length()];
  strcpy(request_str, request.c_str());
  //std::cerr << request_str << ":" << strlen(request_str) << ":" << request.length() << std::endl;
  return (int *) 1;
}

void
carbon_op_queue_repl_1::my_install(void * data)
{
}

void *
carbon_op_queue_repl_1::my_get()
{
  return NULL;
}

void
carbon_op_queue_repl_1::my_purge(void * data)
{
}

void
carbon_op_queue_repl_1::my_init()
{
   lockOnQueue = utilities::Lock_p( new utilities::Lock(getNewMutex()));
   
   //load the shared keys of replica 1
   string repl_1_shared_file("repl_1_shared_keys.txt");
   repl_1_shared_file = string(KEYS_DIR) + repl_1_shared_file;
   replicaKeyHash = utilities::ReplicaKeyHash_p(new utilities::ReplicaKeyHash(repl_1_shared_file));
}



