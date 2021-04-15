////////////////////////////////////////////////////////////////////////////////
// Module/EngineModuleInterface.h (Leggiero - Engine)
//
// Base interface class for Engine Modules
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__MODULE__ENGINE_MODULE_INTERFACE_H
#define __ENGINE__MODULE__ENGINE_MODULE_INTERFACE_H


// Standard Library
#include <vector>

// External Library
#include <errno.h>
#include <pthread.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>
#include <Utility/Sugar/SingletonPattern.h>

// Leggiero.Engine
#include "EngineModuleId.h"


namespace Leggiero
{
	// Forward Declaration
	class IPlatformApplication;

	namespace Engine
	{
		class GameProcessAnchor;
	}


	// Leggiero Engine Module Interface
	class EngineModuleInterface
		: private Utility::SyntacticSugar::NonCopyable
	{
	public:
		EngineModuleInterface();
		virtual ~EngineModuleInterface();

	public:
		// Get Type Id of the Module
		virtual EngineModuleIdType GetModuleType() const { return EngineModuleIdType::kINVALID; };

		// Get type Id list of dependent modules needed by this module
		virtual const std::vector<EngineModuleIdType> &GetDependentModules() const;

		void InitializeModule(IPlatformApplication *app, Engine::GameProcessAnchor *gameAnchor);
		void FinalizeModule();

	protected:
		virtual void _InitializeCurrentModule(IPlatformApplication *app, Engine::GameProcessAnchor *gameAnchor) = 0;
		virtual void _FinalizeCurrentModule() = 0;

	protected:
		bool m_isInitialized;
		pthread_mutex_t m_initializeStateLock;
	};


	namespace ModuleSystem
	{
		// Dummy Interface represents an Invalid Module
		class DummyModuleInterface
			: public EngineModuleInterface
			, public Utility::DesignPattern::Singleton<DummyModuleInterface>
		{
			LEGGIERO_MAKE_SINGLETON_UNIQUE(DummyModuleInterface);

		public:
			virtual ~DummyModuleInterface() { }

		protected:
			virtual void _InitializeCurrentModule(IPlatformApplication *app, Engine::GameProcessAnchor *gameAnchor) override { }
			virtual void _FinalizeCurrentModule() override { }
		};
	}


	// Function to get module interface
	template <EngineModuleIdType MODULE_ID>
	EngineModuleInterface &GetModuleInterface()
	{
		// Need to Warn INVALID Module Access
		return ModuleSystem::DummyModuleInterface::GetInstance();
	}
}


// Template specialization for Engine Module Access Functions
#define DECLARE_GET_MODULE_INTERFACE(CLASS_NAME, MODULE_ID) \
namespace Leggiero \
{ \
	template<> \
	EngineModuleInterface &GetModuleInterface<MODULE_ID>(); \
}

#define DEFINE_GET_SINGLETON_MODULE_INTERFACE(CLASS_NAME, MODULE_ID) \
namespace Leggiero \
{ \
	template<> \
	EngineModuleInterface &GetModuleInterface<MODULE_ID>() \
	{ \
		return CLASS_NAME::GetInstance(); \
	} \
}

#endif
