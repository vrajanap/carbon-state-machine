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

#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <errno.h>
#include <sched.h>

class Thread
{
  static int proc_num;
  pthread_t coreThread;
 public:
  void init(void *(*startRoutine)(void*), void * arg);
  void kill();
  pthread_t getThread() {return coreThread;}
};

#endif // THREAD_H_
