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
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
//  USA.
// 


#include "rsa.h"
#include "osrng.h"
#include "hex.h" 
#include "files.h"
#include <iostream>
using namespace std;
namespace cripton 
{
  class RSASignVer
  {
    public:
    static bool verifySignature(std::string PublicKeyFile, std::string SignedFile, std::string message)
    {
      cout << "message " <<message<<endl;
      try
      {
        // Load Public Key
        CryptoPP::FileSource pubFile( PublicKeyFile.c_str(), true, new CryptoPP::HexDecoder );
        
        // Verifier Object
        CryptoPP::RSASSA_PKCS1v15_SHA_Verifier pub(pubFile);

        // Intialize File Source
        CryptoPP::FileSource SignatureFile( SignedFile.c_str(), true, new CryptoPP::HexDecoder);

        // Sanity Check
        if (SignatureFile.MaxRetrievable() != pub.SignatureLength())
         { throw std::string( "Signature File Size Problem" ); }

        CryptoPP::SecByteBlock signature( pub.SignatureLength() );
        SignatureFile.Get( signature, signature.size() );

        // Prepare Verifier
        CryptoPP::VerifierFilter *verifierFilter = new CryptoPP::VerifierFilter(pub);
        verifierFilter->Put(signature, pub.SignatureLength());

        // Invoke Verifier
        CryptoPP::StringSource( message, true, verifierFilter );

        // Paydirt
        if( false == verifierFilter->GetLastResult() )
         { 
           return false;
         }

        return true;
      } // try

      catch( CryptoPP::Exception& e ) {
        std::cerr << "Error: " << e.what() << std::endl;
      }

      catch (...) {
        std::cerr << "Unknown Error" << std::endl;
      }
      return false;
    }
  };
}

