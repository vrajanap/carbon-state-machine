//  
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//   
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//   
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 

#include "modes.h"
#include "hex.h"
#include "filters.h"
#include "osrng.h"
#include "sha.h"
#include "hmac.h"
#include <string>
#include <iostream>
#define KEY_LENGTH 256

using namespace CryptoPP;
using namespace std;

namespace cripton 
{
  class HMACSignVer 
  {
    public:
      static byte* genMac(std::string plainText, std::string secretKey)
      {
        int counter = 0;
        std::string::size_type stringSize;
        std::string::const_iterator strBegin;
        std::string::const_iterator strEnd;
        std::string::const_iterator keyBegin;
        std::string::const_iterator keyEnd;


        // Initialize crypto stuff
        byte *hmacOutputBuffer = (byte *) malloc(CryptoPP::SHA::DIGESTSIZE * sizeof(byte));
        memset( hmacOutputBuffer, 0x0, CryptoPP::SHA::DIGESTSIZE );
	
        // Find the size of the string
	stringSize = plainText.size();

	// Initialize the input buffer
	byte inputData[stringSize+1];
        byte key[KEY_LENGTH+1];

        // Setup interators for the beginning and end of the string
	strBegin = plainText.begin();
	strEnd = plainText.end();

	// Loop through the string assigning each character to the buffer
	while ( strBegin != strEnd )
	{
	    inputData[ counter++ ] = *strBegin++;
	}
        inputData[stringSize] = '\0';

        counter = 0;
        keyBegin = secretKey.begin();
        keyEnd = secretKey.end();

        while ( keyBegin != keyEnd )
        {
          key[ counter++ ] = *keyBegin++;
        }
        key[KEY_LENGTH] = '\0';

	// Run the HMAC calculation
	CryptoPP::HMAC<SHA >( key, KEY_LENGTH).CalculateDigest( hmacOutputBuffer, inputData, stringSize);
        return hmacOutputBuffer;
      }
      
      static bool verifyMac(std::string message, std::string key, byte *mac)
      {
        byte *hmac = genMac(message, key);
        for(int i=0; i<CryptoPP::SHA::DIGESTSIZE; i++)
          if(hmac[i] != mac[i])
            return false;
        return true;
      }

  };
}
