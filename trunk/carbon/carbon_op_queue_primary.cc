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
#define KEYS_DIR "./keys/"
#define TMP_DIR "./tmp/"
#define QUEUE_ID_REPL_1 1
#define QUEUE_ID_REPL_2 2
#define QUEUE_ID_REPL_3 3
#define QUEUE_NAME_REPL_1 "repl_1"
#define QUEUE_NAME_REPL_2 "repl_2"
#define QUEUE_NAME_REPL_3 "repl_3"
#include "lc_carbon_op_queue_primary_Operator.h"
#include "RSASignVer.hh"
#include "HMACSignVer.hh"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <fstream>
#include <cstdlib> 
#include <iostream> 
using namespace std;

std::string 
genRandFileName(string srcAddress)
{
  stringstream fileName;
  fileName << TMP_DIR;
  fileName << srcAddress;
  fileName << rand();
  return fileName.str();
}

void 
populateSigFile(std::string fileName, char *signature)
{
  ofstream fp;
  fp.open(fileName.c_str());
  if(signature != NULL)
    fp << signature;
  fp.close();
}

void 
carbon_op_queue_primary::handleRequest(requestTypes::NetRequest * data, unsigned long dataSize)
{
  //verify if the client is legitimate
  in_addr ipAddress = data->getSrcAddr();
  string srcAddress = inet_ntoa(ipAddress);
  char *sig = data->getSignature();
  
  std::string pbKeyFileName = string(KEYS_DIR) + clientKeyHash->getKeyFileName(srcAddress);
  
  std::string sigFileName = genRandFileName(srcAddress);
  populateSigFile(sigFileName, sig);
  if(!cripton::RSASignVer::verifySignature(pbKeyFileName, sigFileName, data->getBuffer())) {
    cout<< "\nPrimary Queue :: Authentication failed";
   return;
  }

  data->setQueueServerID(0);
  uint32_t id = requestQueue.size();
  data->setMessageID(id);

  //forward the request to replica 1 along with hmac signature
  std::string key = replicaKeyHash->getKey(QUEUE_NAME_REPL_1);
  byte *hmac = cripton::HMACSignVer::genMac(string(data->getBuffer()), key);
  requestTypes::NetRequest *data_repl_1 = new requestTypes::NetRequest(data, QUEUE_ID_REPL_1, hmac);
  out_repl_1(data_repl_1, dataSize);

  //forward the request to replica 2
  std::string key1 = replicaKeyHash->getKey(QUEUE_NAME_REPL_2);
  byte *hmac1 = cripton::HMACSignVer::genMac(string(data->getBuffer()), key1);
  requestTypes::NetRequest *data_repl_2 = new requestTypes::NetRequest(data, QUEUE_ID_REPL_2, hmac1);
  out_repl_2(data_repl_2, dataSize);

  //forward the request to replica 3
  std::string key2 = replicaKeyHash->getKey(QUEUE_NAME_REPL_3);
  byte *hmac2 = cripton::HMACSignVer::genMac(string(data->getBuffer()), key2);
  requestTypes::NetRequest *data_repl_3 = new requestTypes::NetRequest(data, QUEUE_ID_REPL_3, hmac2);
  out_repl_3(data_repl_3, dataSize);

  free(hmac);
  free(hmac1);
  free(hmac2);

  if(requestQueue.size() >= MAX_QUEUE_SIZE) 
  {
    cout<< "\nPrimary Queue :: Discarding the message" << std::endl;
    if( 0 ==  lockOnQueue->state() ) 
    { 
	   cout<< "Primary Queue :: Sending wait"<<endl; 
           int *serverId = (int*) malloc(sizeof(int));
           *serverId = 0;
           qwait((int *) serverId, 1);
	   lockOnQueue->set() ; 
    } 
    return;
  }

  queue_log_records::RequestLog_p element
	(new queue_log_records::RequestLog(getNewMutex(), data)); 
  
  requestQueue.push(element);  
  cout << "Primary Queue :: Queue size is " << requestQueue.size() << std::endl;
  cout << "Primary Queue :: Sending the string to echo server" <<std::endl; 

  out(data, dataSize);
  return;
}

void
carbon_op_queue_primary::handleSignal(int*, unsigned long)
{
   cout << "Primary Queue :: Deleting current elements in queue \n";
   
   while ( ! requestQueue.empty() ) 
   {
	requestQueue.pop();
   }

   lockOnQueue->reset(); 
}

requestTypes::NetRequest *
carbon_op_queue_primary::geninRequest()
{
  std::string request = "GET /index.html HTTP/1.1";
  request += "\r\n\r\n";
  char * request_str = new char[request.length()];
  strcpy(request_str, request.c_str());
  //std::cerr << request_str << ":" << strlen(request_str) << ":" << request.length() << std::endl;
  return new requestTypes::NetRequest(request_str,strlen(request_str),0);

}

int *
carbon_op_queue_primary::genqsignalRequest()
{
  std::string request = "GET /index.html HTTP/1.1";
  request += "\r\n\r\n";
  char * request_str = new char[request.length()];
  strcpy(request_str, request.c_str());
  //std::cerr << request_str << ":" << strlen(request_str) << ":" << request.length() << std::endl;
  return (int *) 1;
}

void
carbon_op_queue_primary::my_install(void * data)
{
}

void *
carbon_op_queue_primary::my_get()
{
  return NULL;
}

void
carbon_op_queue_primary::my_purge(void * data)
{
}

void
carbon_op_queue_primary::my_init()
{
   lockOnQueue = utilities::Lock_p( new utilities::Lock(getNewMutex()));
   
   //initialize client's public keys by loading them into hashmap
   string client_pb_file("client_pb_keys.txt");
   client_pb_file = string(KEYS_DIR) + client_pb_file;
   clientKeyHash = utilities::ClientKeyHash_p(new utilities::ClientKeyHash(client_pb_file));

   //load the shared keys that are present with the primary replica
   string primary_shared_file("primary_shared_keys.txt");
   primary_shared_file = string(KEYS_DIR) + primary_shared_file;
   replicaKeyHash = utilities::ReplicaKeyHash_p(new utilities::ReplicaKeyHash(primary_shared_file));
}
