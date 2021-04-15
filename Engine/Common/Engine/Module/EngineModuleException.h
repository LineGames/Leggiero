////////////////////////////////////////////////////////////////////////////////
// Module/EngineModuleException.h (Leggiero - Engine)
//
// Exceptions from Leggiero Engine Module System
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__MODULE__ENGINE_MODULE_EXCEPTION_H
#define __ENGINE__MODULE__ENGINE_MODULE_EXCEPTION_H


// Standard Library
#include <exception>

// Leggiero.Engine
#include "EngineModuleId.h"


namespace Leggiero
{
	namespace ModuleSystem
	{
		// Exception due to cyclic dependency of engine modules
		class ModuleCyclicDependencyException : public std::exception
		{
		public:
			ModuleCyclicDependencyException()
				: ModuleCyclicDependencyException(EngineModuleIdType::kINVALID) { }

			ModuleCyclicDependencyException(EngineModuleIdType cyclicDetectedModule)
				: cyclicDetectedModule(cyclicDetectedModule)
			{ }

			virtual ~ModuleCyclicDependencyException() throw() { }

		public:
			virtual const char *what() const throw() { return "Engine Modules have Cyclic Dependency"; }

		public:
			EngineModuleIdType cyclicDetectedModule;
		};


		// Exception due to cyclic dependency of engine modules
		class ModuleMissingDependencyException : public std::exception
		{
		public:
			ModuleMissingDependencyException()
				: ModuleMissingDependencyException(EngineModuleIdType::kINVALID) { }

			ModuleMissingDependencyException(EngineModuleIdType notExistingModule)
				: missingModule(notExistingModule)
			{ }

			virtual ~ModuleMissingDependencyException() throw() { }

		public:
			virtual const char *what() const throw() { return "Dependent Engine Module NOT Exists"; }

		public:
			EngineModuleIdType missingModule;
		};
	}
}

#endif
