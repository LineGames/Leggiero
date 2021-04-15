////////////////////////////////////////////////////////////////////////////////
// Toolbox/ModuledGame/ModuledGame.cpp (Leggiero - Engine)
//
// Moduled Game Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ModuledGame.h"

// Leggiero.Engine
#include "../../Application/IPlatformApplication.h"
#include "../../Module/EngineModuleInterface.h"
#include "../../Module/EngineModuleException.h"
#include "../../Module/EngineComponent.h"
#include "../../Module/EngineComponentException.h"


namespace Leggiero
{
	namespace Toolbox
	{
		namespace Game
		{
			//////////////////////////////////////////////////////////////////////////////// Internal Implementations

			namespace _Internal
			{
				//------------------------------------------------------------------------------
				void _ResolveComponentDependencyGraphRecursive(EngineComponentIdType componentToResolve, EngineComponentHolder *componentHolder, 
					std::vector<EngineComponentIdType> &orderedComponentInitQueue, std::set<EngineComponentIdType> &resolvingComponents, std::set<EngineComponentIdType> &resolvedComponents)
				{
					if (resolvedComponents.find(componentToResolve) != resolvedComponents.end())
					{
						// Already Resolved
						return;
					}

					if (resolvingComponents.find(componentToResolve) != resolvingComponents.end())
					{
						// Cyclic Dependency
						throw ModuleSystem::ComponentCyclicDependencyException(componentToResolve);
					}

					EngineComponent *checkingComponent = componentHolder->GetComponentByTypeId(componentToResolve);
					if (checkingComponent == nullptr)
					{
						// Not exists
						throw ModuleSystem::ComponentMissingDependencyException(componentToResolve);
					}

					// Resolve Dependency
					resolvingComponents.insert(componentToResolve);
					const std::vector<EngineComponentIdType> &dependencies = checkingComponent->GetDependentComponents();
					for (EngineComponentIdType dependentComponentType : dependencies)
					{
						_ResolveComponentDependencyGraphRecursive(dependentComponentType, componentHolder, orderedComponentInitQueue, resolvingComponents, resolvedComponents);
					}

					// Resolve Itself
					orderedComponentInitQueue.push_back(componentToResolve);
					resolvedComponents.insert(componentToResolve);
					resolvingComponents.erase(componentToResolve);
				}

				//------------------------------------------------------------------------------
				std::vector<EngineComponentIdType> ResolveComponentDependencyGraph(const std::vector<EngineComponentIdType> &requestedComponents, EngineComponentHolder *componentHolder)
				{
					std::vector<EngineComponentIdType> orderedComponentInitQueue;
					orderedComponentInitQueue.reserve(requestedComponents.size());

					std::set<EngineComponentIdType> resolvedComponents;
					for (EngineComponentIdType resolvingComponentType : requestedComponents)
					{
						std::set<EngineComponentIdType> resolvingComponents;
						_ResolveComponentDependencyGraphRecursive(resolvingComponentType, componentHolder, orderedComponentInitQueue, resolvingComponents, resolvedComponents);
					}

					return orderedComponentInitQueue;
				}
			}


			//////////////////////////////////////////////////////////////////////////////// ModuledGame

			//------------------------------------------------------------------------------
			ModuledGame::ModuledGame()
				: m_moduleInterfaceTable(_ListAllModules())
			{
			}

			//------------------------------------------------------------------------------
			ModuledGame::~ModuledGame()
			{
			}

			//------------------------------------------------------------------------------
			void ModuledGame::_InitializeGameEngineLibrary(IPlatformApplication *application)
			{
				_InitializeUsingModules(application);
			}

			//------------------------------------------------------------------------------
			void ModuledGame::_FinalizeGameEngineLibrary()
			{
				// Shutdown Modules as reverse initialization order
				size_t moduleCount = m_orderedModuleList.size();
				for (int i = (int)moduleCount - 1; i >= 0; --i)
				{
					EngineModuleIdType currentType = m_orderedModuleList[i];

					std::map<EngineModuleIdType, EngineModuleInterface *>::iterator interfaceFindIt = m_moduleInterfaceTable.find(currentType);
					if (interfaceFindIt == m_moduleInterfaceTable.end())
					{
						// Cannot find module interface
						continue;
					}

					interfaceFindIt->second->FinalizeModule();
				}
				m_moduleInitilizationTable.clear();
				m_orderedModuleList.clear();
			}

			//------------------------------------------------------------------------------
			void ModuledGame::_AssembleGameEngineComponents(IPlatformApplication *application)
			{
				_RegisterEngineComponent(application->GetApplicationEngineComponent(), true);
				_RegisterUsingComponents();

				// Resolve Dependency
				std::vector<EngineComponentIdType> resolvedOrder = _Internal::ResolveComponentDependencyGraph(m_orderedComponentList, this);
				m_orderedComponentList.swap(resolvedOrder);

				// Do initialization as resolved order
				for (EngineComponentIdType currentComponentId : m_orderedComponentList)
				{
					EngineComponent *currentComponent = GetComponentByTypeId(currentComponentId);

					// Check Module Dependencies
					const std::vector<EngineModuleIdType> &dependentModules = currentComponent->GetDependentModules();
					for (EngineModuleIdType dependentModuleId : dependentModules)
					{
						std::map<EngineModuleIdType, bool>::iterator initializationStateIt = m_moduleInitilizationTable.find(dependentModuleId);
						if (initializationStateIt == m_moduleInitilizationTable.end() || !initializationStateIt->second)
						{
							// Module needed by this component not initialized
							throw ModuleSystem::ComponentModuleMissingDependencyException(dependentModuleId);
						}
					}

					// Inject Dependencies
					const std::vector<EngineComponentIdType> &dependencies = currentComponent->GetDependentComponents();
					for (EngineComponentIdType dependentComponentId : dependencies)
					{
						currentComponent->InjectDependency(dependentComponentId, GetComponentByTypeId(dependentComponentId));
					}

					// Initialization
					currentComponent->InitializeComponent(m_processEventAnchor);
				}
			}

			//------------------------------------------------------------------------------
			void ModuledGame::_FinalizeGameEngineComponents()
			{
				// Shutdown Components as reverse initialization order
				size_t componentCount = m_orderedComponentList.size();
				for (int i = (int)componentCount - 1; i >= 0; --i)
				{
					EngineComponentIdType currentType = m_orderedComponentList[i];
					EngineComponent *currentComponent = GetComponentByTypeId(currentType);
					if (currentComponent == nullptr)
					{
						continue;
					}
					UnRegisterComponent(currentType);
					currentComponent->ShutdownComponent();

					// Delete components not managed by other system
					std::set<EngineComponentIdType>::const_iterator findIt = m_unManagedComponentSet.find(currentType);
					if (findIt != m_unManagedComponentSet.end())
					{
						delete currentComponent;
					}
				}
				m_unManagedComponentSet.clear();
				m_orderedComponentList.clear();
			}

			//------------------------------------------------------------------------------
			void ModuledGame::_InitializeModule(EngineModuleIdType moduleId, IPlatformApplication *application)
			{
				// Check Duplicated Initialization
				std::map<EngineModuleIdType, bool>::iterator initializationStateIt = m_moduleInitilizationTable.find(moduleId);
				if (initializationStateIt != m_moduleInitilizationTable.end())
				{
					if (initializationStateIt->second)
					{
						// Already Initialized
						return;
					}
					else
					{
						// Recursive Initialization due to Cyclic Dependency
						throw ModuleSystem::ModuleCyclicDependencyException(moduleId);
					}
				}
				m_moduleInitilizationTable.insert(std::make_pair(moduleId, false));

				std::map<EngineModuleIdType, EngineModuleInterface *>::iterator interfaceFindIt = m_moduleInterfaceTable.find(moduleId);
				if (interfaceFindIt == m_moduleInterfaceTable.end())
				{
					// Cannot find module interface
					throw ModuleSystem::ModuleMissingDependencyException(moduleId);
				}

				// Initialize Dependent Modules
				const std::vector<EngineModuleIdType> &dependentModules = interfaceFindIt->second->GetDependentModules();
				for (EngineModuleIdType dependentModuleId : dependentModules)
				{
					_InitializeModule(dependentModuleId, application);
				}
				
				// Do Initialization
				interfaceFindIt->second->InitializeModule(application, m_processEventAnchor);

				m_moduleInitilizationTable[moduleId] = true;
				m_orderedModuleList.push_back(moduleId);
			}

			//------------------------------------------------------------------------------
			void ModuledGame::_RegisterEngineComponent(EngineComponent *component, bool isManagedByOther)
			{
				if (component == nullptr)
				{
					return;
				}

				RegisterComponent(component);

				Leggiero::EngineComponentIdType componentType = component->GetComponentType();
				m_orderedComponentList.push_back(componentType);

				if (!isManagedByOther)
				{
					m_unManagedComponentSet.insert(componentType);
				}
			}
		}
	}
}
