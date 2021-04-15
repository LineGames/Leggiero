////////////////////////////////////////////////////////////////////////////////
// AsyncTaskHttpComponent.h (Leggiero/Modules - HTTP)
//
// Engine Component to Manage Task-based Asynchronous HTTP Communication
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_HTTP__ASYNC_TASK_HTTP_COMPONENT_H
#define __LM_HTTP__ASYNC_TASK_HTTP_COMPONENT_H


// Standard Library
#include <memory>

// Leggiero.Engine
#include <Engine/Module/EngineComponent.h>
#include <Engine/Module/EngineComponentHolder.h>

// Leggiero.HTTP
#include "HttpCommonType.h"
#include "HttpUtility.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Task
	{
		class TaskManagerComponent;
	}


	namespace HTTP
	{
		namespace Async
		{
			// Forward Declarations
			class HTTPRequestTask;
			class HTTPDownloadTask;


			// Task Based Asynchronous HTTP Communication Component
			class AsyncTaskHttpComponent
				: public EngineComponent
			{
			public:
				AsyncTaskHttpComponent();
				virtual ~AsyncTaskHttpComponent();

			public:	// EngineComponent
				// Get Type Id of the Component
				virtual EngineComponentIdType GetComponentType() const override { return EngineComponentIdType::kAsyncTaskHTTP; };

				// Initialize the Component
				virtual void InitializeComponent(Engine::GameProcessAnchor *gameAnchor) override;

				// Safely Shutdown Component
				virtual void ShutdownComponent() override;

				// Get type Id list of dependent modules needed by this component
				virtual const std::vector<EngineModuleIdType> &GetDependentModules() const override;

				// Get type Id list of dependent components needed by this component
				virtual const std::vector<EngineComponentIdType> &GetDependentComponents() const override;

				// Inject Dependency to the Component.
				// All dependency injections will be done before the initialization.
				virtual void InjectDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent) override;

			public:
				std::shared_ptr<HTTPRequestTask> Request(const std::string &url, 
					RequestMethod method = RequestMethod::kGet, const POSTParameterMap &parameters = kEmptyPOSTParameterMap, long connectTimeout = HTTP::Settings::GetHTTPRequestDefaultTimeoutInSec());

				std::shared_ptr<HTTPDownloadTask> Download(const std::string &downloadFilePath, const std::string &url, 
					RequestMethod method = RequestMethod::kGet, const POSTParameterMap &parameters = kEmptyPOSTParameterMap, 
					bool isBackgroundTask = true, long connectTimeout = HTTP::Settings::GetHTTPRequestDefaultTimeoutInSec());

			protected:
				Task::TaskManagerComponent *m_taskManager;
			};
		}
	}
}


DECLARE_GET_COMPONENT_INTERFACE(Leggiero::HTTP::Async::AsyncTaskHttpComponent, Leggiero::EngineComponentIdType::kAsyncTaskHTTP);

#endif
