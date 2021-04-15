////////////////////////////////////////////////////////////////////////////////
// GraphicResourceManagerComponent.cpp (Leggiero/Modules - Graphics)
//
// Graphic Resource Manager Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GraphicResourceManagerComponent.h"

// Leggiero.Utility
#include <Utility/Object/PointerHolder.h>
#include <Utility/Sugar/Finally.h>

// Leggiero.Engine
#include <Engine/Application/GameProcessAnchor.h>

// Leggiero.Application
#include <Application/ApplicationComponent.h>
#include <Application/SystemEventDispatcher.h>

// Leggiero.FileSystem
#include <FileSystem/BundleFileResourceComponent.h>

// Leggiero.Graphics
#include "GraphicsThreadContext.h"
#include "_Internal/_InternalInitializer.h"
#include "_Internal/_InternalUpdater.h"
#include "Common/IGLGraphicResource.h"
#include "Texture/TextureManager.h"
#include "Texture/RuntimeTextureAtlasManager.h"
#include "Texture/TextureCommitter.h"
#include "Shader/ProgramManager.h"


DEFINE_GET_COMPONENT_INTERFACE(Leggiero::Graphics::GraphicResourceManagerComponent, Leggiero::EngineComponentIdType::kGraphicResourceManager);


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// Module Internal Implementations

		namespace _Internal
		{
			//------------------------------------------------------------------------------
			// Common Code-Level Constants
			constexpr int kCommitTargetTextureCacheCount = 6;
			constexpr int kThrashCollectThreshold = 256;
		}


		//////////////////////////////////////////////////////////////////////////////// GraphicResourceManagerComponent

		//------------------------------------------------------------------------------
		GraphicResourceManagerComponent::GraphicResourceManagerComponent(bool isUseBundleFileSystem)
			: m_textureManager(nullptr)
			, m_bundleFileSystemComponent(nullptr), m_isUseBundleFileSystem(isUseBundleFileSystem)
			, m_isResourceBackBinCollecting(false), m_collectedTrashCount(0)
			, m_isRefreshRequested(false)
			, m_myPointerHolder(std::make_shared<Utility::Object::PointerHolder>(this))
		{

		}

		//------------------------------------------------------------------------------
		GraphicResourceManagerComponent::~GraphicResourceManagerComponent()
		{
			m_myPointerHolder->NotifyTargetDeleted();
		}

		//------------------------------------------------------------------------------
		// Initialize the Component
		void GraphicResourceManagerComponent::InitializeComponent(Engine::GameProcessAnchor *gameAnchor)
		{
			m_textureManager = new TextureManager(this);
			m_programManager = new ProgramManager(this);

			m_resourceBin = std::make_shared<std::vector<std::function<void()> > >();
			m_resourceBinBack = std::make_shared<std::vector<std::function<void()> > >();

			m_textureCommitter = new TextureCommitter(this);

			m_runtimeTextureAtlasManager = new RuntimeTextureAtlasManager(this);

			m_applicationComponent->SystemEventCenter().RegisterApplicationEventObserver(this);
			m_applicationComponent->SystemEventCenter().RegisterGraphicEventObserver(this);

			m_gameAnchor = gameAnchor;
			gameAnchor->RegisterOnBeforeFrame(this);
		}

		//------------------------------------------------------------------------------
		// Safely Shutdown Component
		void GraphicResourceManagerComponent::ShutdownComponent()
		{
			m_gameAnchor->UnRegisterOnBeforeFrame(this);

			m_applicationComponent->SystemEventCenter().UnRegisterApplicationEventObserver(this);
			m_applicationComponent->SystemEventCenter().UnRegisterGraphicEventObserver(this);

			if (m_runtimeTextureAtlasManager != nullptr)
			{
				m_runtimeTextureAtlasManager->_ShutdownManager();
				delete m_runtimeTextureAtlasManager;
				m_runtimeTextureAtlasManager = nullptr;
			}

			if (m_textureCommitter != nullptr)
			{
				m_textureCommitter->_FinalizeAllTextureCommitResources();
				delete m_textureCommitter;
				m_textureCommitter = nullptr;
			}

			m_resourceBin = nullptr;
			m_resourceBinBack = nullptr;

			if (m_textureManager != nullptr)
			{
				delete m_textureManager;
				m_textureManager = nullptr;
			}
			if (m_programManager != nullptr)
			{
				delete m_programManager;
				m_programManager = nullptr;
			}
		}

		//------------------------------------------------------------------------------
		// Get type Id list of dependent components needed by this component
		const std::vector<EngineComponentIdType> &GraphicResourceManagerComponent::GetDependentComponents() const
		{
			static std::vector<EngineComponentIdType> dependentComponentListWithBundle {
				EngineComponentIdType::kApplication, 
				EngineComponentIdType::kBundleFileResource
			};
			static std::vector<EngineComponentIdType> dependentComponentListNoBundle{
				EngineComponentIdType::kApplication
			};
			return (m_isUseBundleFileSystem ? dependentComponentListWithBundle : dependentComponentListNoBundle);
		}

		//------------------------------------------------------------------------------
		// Inject Dependency to the Component.
		// All dependency injections will be done before the initialization.
		void GraphicResourceManagerComponent::InjectDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent)
		{
			switch (componentId)
			{
				case EngineComponentIdType::kApplication:
					{
						m_applicationComponent = dynamic_cast<Application::ApplicationComponent *>(dependentComponent);
					}
					break;

				case EngineComponentIdType::kBundleFileResource:
					{
						m_bundleFileSystemComponent = dynamic_cast<FileSystem::BundleFileResourceComponent *>(dependentComponent);
					}
					break;
			}
		}

		//------------------------------------------------------------------------------
		void GraphicResourceManagerComponent::OnReturnFromBackground()
		{
			m_isRefreshRequested = true;
		}

		//------------------------------------------------------------------------------
		void GraphicResourceManagerComponent::OnGraphicInitialized()
		{
			_DoRefresh();
		}

		//------------------------------------------------------------------------------
		void GraphicResourceManagerComponent::GameProcess_OnGraphicPrepare(bool isFirstGraphicPrepare)
		{
			if (isFirstGraphicPrepare)
			{
				if (m_textureCommitter != nullptr)
				{
					m_textureCommitter->_CheckAndPrepareTextureCommitShaderProgram();
					m_textureCommitter->_PrepareTextureCommitTarget(_Internal::kCommitTargetTextureCacheCount);
				}
			}
		}

		//------------------------------------------------------------------------------
		void GraphicResourceManagerComponent::GameProcess_OnBeforeFrame(GameFrameNumberType frameNumber, GameTimeClockType::time_point frameReferenceTime)
		{
			if (m_isRefreshRequested)
			{
				_DoRefresh();
			}

			//NOTE: For now, we do resource garbage collection at the frame in main thread context. But this can be done in background graphic thread.
			CollectGLGarbageResources();
		}

		//------------------------------------------------------------------------------
		TextureManager &GraphicResourceManagerComponent::GetTextureManager()
		{
			return *m_textureManager;
		}

		//------------------------------------------------------------------------------
		RuntimeTextureAtlasManager &GraphicResourceManagerComponent::GetRuntimeTextureAtlasManager()
		{
			return *m_runtimeTextureAtlasManager;
		}

		//------------------------------------------------------------------------------
		ProgramManager &GraphicResourceManagerComponent::GetProgramManager()
		{
			return *m_programManager;
		}

		//------------------------------------------------------------------------------
		TextureCommitter &GraphicResourceManagerComponent::GetTextureCommitter()
		{
			return *m_textureCommitter;
		}

		//------------------------------------------------------------------------------
		void GraphicResourceManagerComponent::TrashGLGarbageResources(std::function<void()> selfCollector)
		{
			auto lock = m_resourceBinLock.Lock();	// Try to lock, but go even it failed
			
			if (!m_resourceBin)
			{
				// No-Bin
				return;
			}
			m_resourceBin->push_back(selfCollector);
		}

		//------------------------------------------------------------------------------
		void GraphicResourceManagerComponent::CollectGLGarbageResources()
		{
			{
				auto lock = m_resourceBinLock.Lock();
				if (!lock)
				{
					// Something Wrong
					return;
				}

				bool expectedNotCollection = false;
				if (!m_isResourceBackBinCollecting.compare_exchange_strong(expectedNotCollection, true))
				{
					// Already Collecting
					return;
				}

				m_resourceBin.swap(m_resourceBinBack);
			}
			
			int collectedTrashCount = 0;
			{
				auto backBinReleaseFunc = [this]() mutable { this->m_isResourceBackBinCollecting.store(false); };
				FINALLY_OF_BLOCK(_END_COLLECTING_BACK_BIN, backBinReleaseFunc);

				if (!m_resourceBinBack)
				{
					// No-Bin
					return;
				}

				for (std::vector<std::function<void()> >::iterator it = m_resourceBinBack->begin(); it != m_resourceBinBack->end(); ++it)
				{
					(*it)();
				}
				int collectedCount = static_cast<int>(m_resourceBinBack->size());

				collectedTrashCount = m_collectedTrashCount.fetch_add(collectedCount);

				m_resourceBinBack->clear();
			}

			if (collectedTrashCount >= _Internal::kThrashCollectThreshold)
			{
				m_collectedTrashCount.store(0);
				_CompactManagedResources();
			}
		}

		//------------------------------------------------------------------------------
		void GraphicResourceManagerComponent::_DoRefresh()
		{
			m_isRefreshRequested = false;
			CollectGLGarbageResources();
			if (m_textureCommitter != nullptr)
			{
				m_textureCommitter->_CheckAndRestoreTextureCommitResources();
			}
			if (m_runtimeTextureAtlasManager != nullptr)
			{
				m_runtimeTextureAtlasManager->_CheckAndRestoreManagedResources();
			}
			_CheckAndRestoreResources();
		}

		//------------------------------------------------------------------------------
		void GraphicResourceManagerComponent::_CheckAndRestoreResources()
		{
			auto lock = m_managedResourcesLock.Lock();
			if (!lock)
			{
				// Something Wrong
				return;
			}

			// Collect Resources to be restored
			std::vector<std::shared_ptr<IGLGraphicResource> > refreshingResources;
			for (std::list<std::weak_ptr<IGLGraphicResource> >::iterator it = m_managedResources.begin(); it != m_managedResources.end(); ++it)
			{
				std::shared_ptr<IGLGraphicResource> currentResource = it->lock();
				if (currentResource)
				{
					if (!currentResource->IsValid())
					{
						refreshingResources.push_back(currentResource);
					}
				}
			}

			// Do Restore
			for (std::vector<std::shared_ptr<IGLGraphicResource> >::iterator it = refreshingResources.begin(); it != refreshingResources.end(); ++it)
			{
				(*it)->Restore();
			}

			// Compact
			m_managedResources.erase(std::remove_if(m_managedResources.begin(), m_managedResources.end(), [](std::weak_ptr<IGLGraphicResource> &cachedResource) {
				std::shared_ptr<IGLGraphicResource> currentResource = cachedResource.lock();
				if (!currentResource)
				{
					return true;
				}
				if (!currentResource->IsValid())
				{
					return true;
				}
				return false;
				}), m_managedResources.end());
		}

		//------------------------------------------------------------------------------
		void GraphicResourceManagerComponent::_RegisterResource(std::shared_ptr<IGLGraphicResource> resource)
		{
			auto lock = m_managedResourcesLock.Lock();
			if (!lock)
			{
				// Something Wrong
				return;
			}

			m_managedResources.push_back(resource);
		}

		//------------------------------------------------------------------------------
		void GraphicResourceManagerComponent::_CompactManagedResources()
		{
			auto lock = m_managedResourcesLock.Lock();
			if (!lock)
			{
				// Something Wrong
				return;
			}

			m_managedResources.erase(std::remove_if(m_managedResources.begin(), m_managedResources.end(), [](std::weak_ptr<IGLGraphicResource> &cachedResource) {
				return cachedResource.expired();
				}), m_managedResources.end());
		}
	}
}
