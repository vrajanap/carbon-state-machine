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

#include "Thread.hh"

int Thread::proc_num = 0;

void
Thread::init(void *(*startRoutine)(void*), void * arg)
{
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if(pthread_create(&coreThread, &attr, startRoutine, arg)) {
    std::cerr << "pthread_create failed" << std::endl;
    std::exit(1);
  }
   cpu_set_t set;
   CPU_ZERO(&set);
   CPU_SET(proc_num++, &set);
   /*if(pthread_setaffinity_np(coreThread, sizeof(set), &set) != 0) {
     std::cerr << "Affinity not set: " << strerror(errno) << std::endl;
   }  */
   //std::cerr << "Setting affinity of thread " << coreThread << " to proc " << proc_num - 1 << "." << std::endl;
  return;
}

void
Thread::kill()
{
  pthread_cancel(coreThread);
  return;
}
