////////////////////////////////////////////////////////////////////////////////
// CryptoModuleInterface.cpp (Leggiero/Modules - Crypto)
//
// Basic Implementation related to Crypto Module
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "CryptoModuleInterface.h"

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Crypto
#ifndef _LEGGIERO_IOS
#include "OpenSSLCrypto/OpenSSLUtility.h"
#endif


LEGGIERO_DEFINE_SINGLETON_MEMBERS(Leggiero::Crypto::CryptoModule);
DEFINE_GET_SINGLETON_MODULE_INTERFACE(Leggiero::Crypto::CryptoModule, Leggiero::EngineModuleIdType::kCrypto);


namespace Leggiero
{
	namespace Crypto
	{
		//////////////////////////////////////////////////////////////////////////////// CryptoModule

		//------------------------------------------------------------------------------
		void CryptoModule::_InitializeCurrentModule(IPlatformApplication *app, Engine::GameProcessAnchor *gameAnchor)
		{
			#ifndef _LEGGIERO_IOS
				OpenSSL::InitializeOpenSSLCryptoSystem();
			#endif
		}

		//------------------------------------------------------------------------------
		void CryptoModule::_FinalizeCurrentModule()
		{
			#ifndef _LEGGIERO_IOS
				OpenSSL::FinalizeOpenSSLCryptoSystem();
			#endif
		}
	}
}
