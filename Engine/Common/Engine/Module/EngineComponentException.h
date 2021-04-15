////////////////////////////////////////////////////////////////////////////////
// Module/EngineComponentException.h (Leggiero - Engine)
//
// Exceptions from Leggiero Engine Component System
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__MODULE__ENGINE_COMPONENT_EXCEPTION_H
#define __ENGINE__MODULE__ENGINE_COMPONENT_EXCEPTION_H


// Standard Library
#include <exception>

// Leggiero.Engine
#include "EngineModuleId.h"
#include "EngineComponentId.h"


namespace Leggiero
{
	namespace ModuleSystem
	{
		// Exception due to cyclic dependency of engine components
		class ComponentCyclicDependencyException : public std::exception
		{
		public:
			ComponentCyclicDependencyException()
				: ComponentCyclicDependencyException(EngineComponentIdType::kINVALID) { }

			ComponentCyclicDependencyException(EngineComponentIdType cyclicDetectedComponent)
				: cyclicDetectedComponent(cyclicDetectedComponent)
			{ }

			virtual ~ComponentCyclicDependencyException() throw() { }

		public:
			virtual const char *what() const throw() { return "Engine Components have Cyclic Dependency"; }

		public:
			EngineComponentIdType cyclicDetectedComponent;
		};


		// Missing Dependent Component
		class ComponentMissingDependencyException : public std::exception
		{
		public:
			ComponentMissingDependencyException()
				: ComponentMissingDependencyException(EngineComponentIdType::kINVALID) { }

			ComponentMissingDependencyException(EngineComponentIdType missingComponent)
				: missingComponentType(missingComponent)
			{ }

			virtual ~ComponentMissingDependencyException() throw() { }

		public:
			virtual const char *what() const throw() { return "Dependent Engine Component NOT Exists"; }

		public:
			EngineComponentIdType missingComponentType;
		};


		// Missing Component Dependent Module
		class ComponentModuleMissingDependencyException : public std::exception
		{
		public:
			ComponentModuleMissingDependencyException()
				: ComponentModuleMissingDependencyException(EngineModuleIdType::kINVALID) { }

			ComponentModuleMissingDependencyException(EngineModuleIdType notInitializedModule)
				: missingModuleType(notInitializedModule)
			{ }

			virtual ~ComponentModuleMissingDependencyException() throw() { }

		public:
			virtual const char *what() const throw() { return "Dependent Engine Module need by Component NOT Initialized"; }

		public:
			EngineModuleIdType missingModuleType;
		};
	}
}

#endif
