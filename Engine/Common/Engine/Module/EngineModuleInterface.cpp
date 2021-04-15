////////////////////////////////////////////////////////////////////////////////
// Module/EngineModuleInterface.cpp (Leggiero - Engine)
//
// Implement Base Interface Functions for Engine Modules
////////////////////////////////////////////////////////////////////////////////


// My Header
#include "EngineModuleInterface.h"

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>


namespace Leggiero
{
	//////////////////////////////////////////////////////////////////////////////// EngineModuleInterface

	//------------------------------------------------------------------------------
	EngineModuleInterface::EngineModuleInterface()
		: m_isInitialized(false)
	{
		pthread_mutex_init(&m_initializeStateLock, NULL);
	}

	//------------------------------------------------------------------------------
	EngineModuleInterface::~EngineModuleInterface()
	{		
		pthread_mutex_destroy(&m_initializeStateLock);
	}

	//------------------------------------------------------------------------------
	// Get dependent module list
	const std::vector<EngineModuleIdType> &EngineModuleInterface::GetDependentModules() const
	{
		static std::vector<EngineModuleIdType> dependentModuleList { };
		return dependentModuleList;
	}

	//------------------------------------------------------------------------------
	// Initialize the module
	void EngineModuleInterface::InitializeModule(IPlatformApplication *app, Engine::GameProcessAnchor *gameAnchor)
	{
		int lockResult = pthread_mutex_lock(&m_initializeStateLock);
		if (lockResult == 0)
		{
			pthread_mutex_t *lockCopy = &m_initializeStateLock;
			auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
			FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

			if (m_isInitialized)
			{
				// Already Initialized;
				return;
			}

			_InitializeCurrentModule(app, gameAnchor);

			m_isInitialized = true;
		}
		else
		{
			// Something Wrong
			return;
		}
	}

	//------------------------------------------------------------------------------
	// Safely shutdown the module
	void EngineModuleInterface::FinalizeModule()
	{
		m_isInitialized = false;
		_FinalizeCurrentModule();
	}
}

// Dummy Module
LEGGIERO_DEFINE_SINGLETON_MEMBERS(Leggiero::ModuleSystem::DummyModuleInterface);
