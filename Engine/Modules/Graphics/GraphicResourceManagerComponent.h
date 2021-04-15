////////////////////////////////////////////////////////////////////////////////
// GraphicResourceManagerComponent.h (Leggiero/Modules - Graphics)
//
// Graphic Resource Manager
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__GRAPHIC_RESOURCE_MANAGER_COMPONENT_H
#define __LM_GRAPHICS__GRAPHIC_RESOURCE_MANAGER_COMPONENT_H


// Standard Library
#include <atomic>
#include <functional>
#include <list>
#include <memory>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Engine
#include <Engine/Module/EngineComponent.h>
#include <Engine/Module/EngineComponentHolder.h>
#include <Engine/Application/GameProcessAnchorObserver.h>

// Leggiero.Application
#include <Application/SystemEventObserver.h>


namespace Leggiero
{
	// Forward Declaration
	namespace Utility
	{
		namespace Object
		{
			class PointerHolder;
		}
	}


	// Forward Declarations for Dependent Components
	namespace Application
	{
		class ApplicationComponent;
	}

	namespace FileSystem
	{
		class BundleFileResourceComponent;
	}


	namespace Graphics
	{
		// Forward Declarations
		class IGLGraphicResource;
		class TextureManager;
		class RuntimeTextureAtlasManager;
		class ProgramManager;
		class TextureCommitter;


		// Graphics Resource Manager Component
		class GraphicResourceManagerComponent
			: public EngineComponent
			, public Application::IApplicationEventObserver
			, public Application::IGraphicEventObserver
			, public Engine::GameProcessAnchorObserver::IGraphicPrepareHandler
			, public Engine::GameProcessAnchorObserver::IBeforeFrameHandler
		{
			friend class TextureManager;
			friend class ProgramManager;

		public:
			GraphicResourceManagerComponent(bool isUseBundleFileSystem = true);
			virtual ~GraphicResourceManagerComponent();

		public:	// EngineComponent
			// Get Type Id of the Component
			virtual EngineComponentIdType GetComponentType() const override { return EngineComponentIdType::kGraphicResourceManager; };

			// Initialize the Component
			virtual void InitializeComponent(Engine::GameProcessAnchor *gameAnchor) override;

			// Safely Shutdown Component
			virtual void ShutdownComponent() override;

			// Get type Id list of dependent components needed by this component
			virtual const std::vector<EngineComponentIdType> &GetDependentComponents() const override;

			// Inject Dependency to the Component.
			// All dependency injections will be done before the initialization.
			virtual void InjectDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent) override;

		public:	// IApplicationEventObserver
			virtual void OnReturnFromBackground() override;

		public:	// IGraphicEventObserver
			virtual void OnGraphicInitialized() override;

		public:	// IGraphicPrepareHandler
			virtual void GameProcess_OnGraphicPrepare(bool isFirstGraphicPrepare) override;

		public:	// IBeforeFrameHandler
			virtual void GameProcess_OnBeforeFrame(GameFrameNumberType frameNumber, GameTimeClockType::time_point frameReferenceTime) override;

		public:
			TextureManager &GetTextureManager();
			RuntimeTextureAtlasManager &GetRuntimeTextureAtlasManager();
			ProgramManager &GetProgramManager();

			TextureCommitter &GetTextureCommitter();

		public:
			void TrashGLGarbageResources(std::function<void()> selfCollector);
			void CollectGLGarbageResources();

			std::shared_ptr<Utility::Object::PointerHolder> GetPointerHolder() { return m_myPointerHolder; }

		protected:
			void _DoRefresh();
			void _CheckAndRestoreResources();

			void _RegisterResource(std::shared_ptr<IGLGraphicResource> resource);
			void _CompactManagedResources();

		protected:
			Application::ApplicationComponent		*m_applicationComponent;
			FileSystem::BundleFileResourceComponent *m_bundleFileSystemComponent;
			bool									m_isUseBundleFileSystem;

			Engine::GameProcessAnchor			*m_gameAnchor;

		protected:
			TextureManager					*m_textureManager;
			RuntimeTextureAtlasManager		*m_runtimeTextureAtlasManager;
			ProgramManager					*m_programManager;

			TextureCommitter *m_textureCommitter;

		protected:
			std::list<std::weak_ptr<IGLGraphicResource> >	m_managedResources;
			Utility::Threading::SafePthreadLock				m_managedResourcesLock;

			std::shared_ptr<std::vector<std::function<void()> > >	m_resourceBin;
			std::shared_ptr<std::vector<std::function<void()> > >	m_resourceBinBack;
			Utility::Threading::SafePthreadLock						m_resourceBinLock;
			std::atomic_bool										m_isResourceBackBinCollecting;
			std::atomic<int>										m_collectedTrashCount;

			bool m_isRefreshRequested;

			std::shared_ptr<Utility::Object::PointerHolder> m_myPointerHolder;
		};
	}
}


DECLARE_GET_COMPONENT_INTERFACE(Leggiero::Graphics::GraphicResourceManagerComponent, Leggiero::EngineComponentIdType::kGraphicResourceManager);

#endif
