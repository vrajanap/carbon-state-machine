#include "lc_carbon_op_echoserver_Operator.h"

#include <iostream> 
using namespace std;

void 
carbon_op_echoserver::handleRequest(requestTypes::NetRequest * data, unsigned long dataSize)
{
  cout << "EchoServer::Sending " << data->getBuffer() << " to client" << std::endl; 
  waitReqCount->incrementCounter();
  out(data, dataSize);
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

