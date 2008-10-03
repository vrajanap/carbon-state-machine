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
// $Id: NetRequest.cc 952 2008-09-23 21:42:37Z djwhitebread $
//

#include "NetRequest.hh"

using namespace requestTypes;

lagniappe::FlowID
NetRequest::getFileInfoID()
{
  lagniappe::FlowID ret = 0;

  boost::hash<std::string> sh;
//   const char * name_string = fileName.c_str();
//   for(unsigned int i = 0; i < sizeof(*name_string); ++i) {
//     ret += *(unsigned short *)(name_string + i);
//   }
  ret = sh(fileName);
  //std::cerr<< fileName << ":" << ret << std::endl;
  return ret;
}
