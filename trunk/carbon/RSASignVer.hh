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

