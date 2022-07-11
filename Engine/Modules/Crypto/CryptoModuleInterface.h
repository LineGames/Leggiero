////////////////////////////////////////////////////////////////////////////////
// CryptoModuleInterface.h (Leggiero/Modules - Crypto)
//
// Cryptography Module Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_CRYPTO__CRYPTO_MODULE_ITNERFACE_H
#define __LM_CRYPTO__CRYPTO_MODULE_ITNERFACE_H


// Leggiero.Engine
#include <Engine/Module/EngineModuleInterface.h>


namespace Leggiero
{
	namespace Crypto
	{
		// Cryptography Module Interface
		class CryptoModule
			: public EngineModuleInterface
			, public Utility::DesignPattern::Singleton<CryptoModule>
		{
			LEGGIERO_MAKE_SINGLETON_UNIQUE(CryptoModule);

		public:	// EngineModuleInterface
			// Get Type Id of the Module
			virtual EngineModuleIdType GetModuleType() const override { return EngineModuleIdType::kCrypto; };

		protected:	// EngineModuleInterface
			virtual void _InitializeCurrentModule(IPlatformApplication *app, Engine::GameProcessAnchor *gameAnchor) override;
			virtual void _FinalizeCurrentModule() override;
		};
	}
}

LEGGIERO_DECLARE_SINGLETON_MEMBERS(Leggiero::Crypto::CryptoModule);


DECLARE_GET_MODULE_INTERFACE(Leggiero::Crypto::CryptoModule, Leggiero::EngineModuleIdType::kCrypto);

#endif
