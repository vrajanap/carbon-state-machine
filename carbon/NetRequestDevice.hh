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

#ifndef NETREQUESTDEVICE_HH_
#define NETREQUESTDEVICE_HH_

#include <assert.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <netinet/in.h>

#include <errno.h>

#include <iostream>

#include <lagniappe/RequestDevice.h>
#include <lagniappe/RCon.h>
#include <lagniappe/RData.h>

#include <sysmodel/Synchronized.hh>

#include "NetRequest.hh"

extern int errno;

namespace requestDevices {

const uint32_t BUFSIZE = 8192;
const uint32_t LAGNIAPPE_PORT = 7775;
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void * startDevice(void * arg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

class NetRequestDevice : public lagniappe::RequestDevice, Synchronized
{
  pthread_t coreThread;
  socket_t listener;
  static int procNum;
  bool fromSet;
  std::string intfName;
  bool sysReady;
  fd_set openSet;
  socket_t maxFD;
public:
  NetRequestDevice(std::string intf);
  ~NetRequestDevice();
  void process();
  virtual void run(lagniappe::RCon * rcon);
  virtual void start();
};

}
#endif // NETREQUESTDEVICE_HH_
