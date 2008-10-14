#include "lc_carbon_op_echoserver_Operator.h"

#include <iostream> 
using namespace std;

void 
carbon_op_echoserver::handleRequest(requestTypes::NetRequest * data, unsigned long dataSize)
{ 
  // check if already present.
  // If present, raise warning?
  // else, add to the map along with queueServerID.
  if (false == waitReqCount->checkIfPresent(data->getMessageID(), data->getQueueServerID()))
  {     
   	waitReqCount->insertMsgHash(data->getMessageID(), data->getQueueServerID());
  }
  
  // Check if all messages have been received.
  if (true == waitReqCount->receivedAllRequests(data->getMessageID())) 
  {
       // If yes. reply to client. 
     cout << "EchoServer:: Sending " << data->getBuffer() << " to client" << std::endl; 
     out(data, dataSize);
  }
  // Else return.
  return;
}

requestTypes::NetRequest *
carbon_op_echoserver::geninRequest()
{
  std::string request = "GET /index.html HTTP/1.1";
  request += "\r\n\r\n";
  char * request_str = new char[request.length()];
  strcpy(request_str, request.c_str());
  return new requestTypes::NetRequest(request_str,strlen(request_str),0);

}

void
carbon_op_echoserver::handleWait(int *i, unsigned long)
{
  if (false == waitReqCount->checkIfPresent(-1, *i ))
  {
        waitReqCount->insertMsgHash(-1, *i);
  }

  // Check if all wait messages have been received.
  if (true == waitReqCount->processedAllRequests())
  {
  	cout << "EchoServer:: Sending signal to Queue Servers"<< std::endl;
  	waitReqCount->resetHash() ;
  	esignal_primary((int *) 1, 0);
  	esignal_repl_1((int *) 1, 0);  
  }
}
int *
carbon_op_echoserver::genewaitRequest()
{
  std::string request = "GET /index.html HTTP/1.1";
  request += "\r\n\r\n";
  char * request_str = new char[request.length()];
  strcpy(request_str, request.c_str());

  return (int *) 1;

}

void
carbon_op_echoserver::my_install(void * data)
{
}

void *
carbon_op_echoserver::my_get()
{
  return NULL;
}

void
carbon_op_echoserver::my_purge(void * data)
{
}

void
carbon_op_echoserver::my_init()
{
	waitReqCount = 
	request_counter::RequestCount_p(  	
		new request_counter::RequestCount(getNewMutex()));
}


