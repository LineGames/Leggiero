=======
Modules
=======

Leggiero는 엔진의 기능들을 모듈 단위로 나누어 제공합니다.
각 모듈은 기본적으로 독립성을 가지며, 다른 모듈의 구성요소에 의존성이 존재하는 경우 인터페이스를 통해 명시적으로 표시해야 합니다.


Component and Module Interface
==============================

Leggiero에서는 엔진 모듈의 구성요소에 대한 접근을 **엔진 컴퍼넌트** 와 **모듈 인터페이스** 로 표준화합니다.

엔진 컴퍼넌트
  엔진 컴퍼넌트는 내부적으로 상태를 가질 수 있는, 게임 프로그램 객체의 구성요소입니다.

모듈 인터페이스
  모듈 인터페이스는 개념적인 상태를 가지지 않는, 모듈에서 제공하는 API들을 대표하는 접근 인터페이스입니다.

각 엔진 모듈은 엔진 컴퍼넌트, 또는 모듈 인터페이스를 한 개 이상 제공할 수 있습니다.

Engine Component
----------------
엔진 컴퍼넌트는 모듈에서 제공하는, 상태를 가질 수 있는 단위 기능을 대표합니다.
Leggiero의 엔진에서 구성되는 게임 객체는 엔진 컴퍼넌트를 포함할 수 있는 *엔티티* 객체로, 소속 컴퍼넌트들이 제공하는 엔진 기능을 이용하는 게임 개발 환경을 제공하게 됩니다.

엔진 컴퍼넌트의 인터페이스는 Common/Engine 프로젝트의 *Module/EngineComponent.h* 헤더 파일에서 살펴볼 수 있습니다.

각 컴퍼넌트는 아래 인터페이스를 통해 의존하는 (인터페이스로 대표되는) 모듈과 다른 엔진 컴퍼넌트를 명시할 수 있습니다.

.. code-block:: C++
    :caption: part of EngineComponent.h
    
    // Get type Id list of dependent modules needed by this component
    virtual const std::vector<EngineModuleIdType> &GetDependentModules() const;
    
    // Get type Id list of dependent components needed by this component
    virtual const std::vector<EngineComponentIdType> &GetDependentComponents() const;
    

엔진 컴퍼넌트의 초기화 시점에, Leggiero는 컴퍼넌트에 필요한 모듈이 초기화 되지 않았거나, 의존하는 컴퍼넌트가 존재하지 않는 경우, 또는 컴퍼넌트가 순환 의존성을 가지는 경우 등의 문제가 발생할 수 있는 상황에 대해 에러를 발생시킵니다.

엔진 컴퍼넌트는, 생성 시점에 초기화 되지 않습니다. 아래의 인터페이스가 컴퍼넌트의 초기화와 정리를 담당합니다. Leggiero는 게임 객체를 생성하고 초기화할 때, 적절한 시점에 컴퍼넌트의 초기화 인터페이스를 호출합니다.

.. code-block:: C++
    :caption: part of EngineComponent.h
    
    // Initialize the Component
    virtual void InitializeComponent(Engine::GameProcessAnchor *gameAnchor) { }
    
    // Safely Shutdown Component
    virtual void ShutdownComponent() { }
    

엔진 컴퍼넌트가 의존하는 다른 컴퍼넌트는, 아래의 인터페이스를 이용해 의존성 주입 방식으로 컴퍼넌트에 전달됩니다.

.. code-block:: C++
    :caption: part of EngineComponent.h
    
    // Inject Dependency to the Component.
    // All dependency injections will be done before the initialization.
    virtual void InjectDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent) { }
    

``GetDependentComponents`` 에서 명시한 모든 의존 컴퍼넌트에 대한 의존성 주입은 컴퍼넌트의 ``InitializeComponent`` 함수가 호출되기 전에 완료되며, ``InjectDependency`` 에 ``dependentComponent`` 인자로 전달되는 의존 컴퍼넌트는 이미 초기화 ( ``InitializeComponent`` )가 완료된 상태입니다.

Module Interface
----------------
모듈 인터페이스는, 모듈에서 제공하는 개념적인 상태를 가지지 않는 API들을 대표합니다.

OpenSSL이나 FreeType과 같이, 전역 함수 호출로 동작하는 API라도 내부적으로는 (API 사용자의 입장에서 고려할 필요가 있지는 않은) 상태를 가질 수 있고, 이런 부분에 대해 초기화가 필요한 경우가 있습니다.
모듈 인터페이스는 이와 같은 전역 상태의 생명주기를 Leggiero 엔진 사용자에게 추상화하는 역할을 갖고 있습니다.

한편, 어떤 엔진 모듈은 다른 모듈의 기능에 의존성을 가질 수도 있습니다.
모듈 인터페이스는 모듈 간의 의존성 관계를 명시적으로 정리하고, 의존성을 가지는 모듈 간의 초기화 순서를 조정하는 역할도 가집니다.

모듈 인터페이스의 기반 클래스는 Common/Engine 프로젝트의 *Module/EngineModuleInterface.h* 헤더 파일에 ``Leggiero::EngineModuleInterface`` 로 정의되어 있습니다.

.. code-block:: C++
    :caption: part of EngineModuleInterface.h
    
    // Get type Id list of dependent modules needed by this module
    virtual const std::vector<EngineModuleIdType> &GetDependentModules() const;
    
    void InitializeModule(IPlatformApplication *app, Engine::GameProcessAnchor *gameAnchor);
    void FinalizeModule();
    


Game Creation
=============
Leggiero는 엔진 모듈의 구성요소들이 모여 구성되는 게임에 대한 기본 구현인 ``Leggiero::Toolbox::Game::ModuledGame`` 클래스를 제공합니다.
해당 클래스는 Common/Engine 프로젝트의 *ToolBox/ModuledGame/ModuledGame.h* 헤더 파일에서 찾아볼 수 있습니다.

.. Note:: ``ModuledGame`` 클래스를 이용하지 않고 ``Leggiero::Engine::BaseGame`` 클래스를 상속받아 게임 객체를 생성할 수도 있기는 하지만, Leggiero의 기능들을 이용하기 위해서는 엔진 ToolBox의 ``ModuledGame`` 을 사용하시기를 권장합니다.

``ModuledGame`` 을 상속한 게임 클래스에서의 초기화 과정은 LegToy 프로젝트, 또는 Project Creator 툴에 의해 생성된 새 프로젝트에서 확인하실 수 있습니다.

우선, 게임 프로젝트의 *GameModules.cpp* 파일에서 소스 코드를 살펴봅시다.

.. code-block:: C++
    :caption: part of GameModules.cpp
    
    std::map<EngineModuleIdType, EngineModuleInterface *> ModuledGame::_ListAllModules()
    {
        std::map<EngineModuleIdType, EngineModuleInterface *> moduleTable;
    
        moduleTable.insert(std::make_pair(EngineModuleIdType::kLog, &GetModuleInterface<EngineModuleIdType::kLog>()));
        moduleTable.insert(std::make_pair(EngineModuleIdType::kHTTP, &GetModuleInterface<EngineModuleIdType::kHTTP>()));
        moduleTable.insert(std::make_pair(EngineModuleIdType::kCrypto, &GetModuleInterface<EngineModuleIdType::kCrypto>()));
        moduleTable.insert(std::make_pair(EngineModuleIdType::kGraphics, &GetModuleInterface<EngineModuleIdType::kGraphics>()));
        moduleTable.insert(std::make_pair(EngineModuleIdType::kFont, &GetModuleInterface<EngineModuleIdType::kFont>()));
    
        return moduleTable;
    }
    

Leggiero의 계층 구조 상, 아래 계층인 Common 계층의 엔진 코드(Engine 프로젝트)는 상위 계층인 Modules의 각 모듈에 대해 알 수 없습니다.
따라서, Leggiero 엔진 전체보다 상위의 각 게임 프로젝트에서 사용 가능한 전체 모듈을 명시해 주는 작업이 필요합니다.

실제 게임 객체의 생성 및 초기화 과정은 **CreateGame.cpp** 에 정의되어 있습니다.

.. code-block:: C++
    :caption: part of CreateGame.cpp
    
    // Create a Game Object for LegToy
    std::shared_ptr<IGame> CreateGame(IPlatformApplication *application, IGameLaunchOption *launchOption)
    {
        std::shared_ptr<LegToy::Game> concreteGame = std::make_shared<LegToy::Game>();

        std::shared_ptr<IGameInitializer> gameCreator = std::dynamic_pointer_cast<IGameInitializer>(concreteGame);
        gameCreator->_InitializeGameEngineLibrary(application);
        gameCreator->_AssembleGameEngineComponents(application);

        return concreteGame;
    }
    

각 게임 프로젝트는 이 팩토리 함수를 구현해 구체 게임 클래스의 게임 객체를 인스턴스화 하고, 초기화 과정을 수행합니다.
``Leggiero::IGameInitializer`` 클래스는 게임의 초기화 과정을 추상화한 공통 인터페이스이며, 해당 인터페이스를 상속한 ``ModuledGame`` 은 ``IGameInitializer`` 의 인터페이스에 맞춰 엔진 모듈 구성요소를 포함한 게임 프로그램의 초기화를 수행합니다.

``ModuledGame`` 의 ``_InitializeUsingModules`` 와 ``_RegisterUsingComponents`` 함수에서 실제 게임의 구성 정보를 정의합니다.

.. code-block:: C++
    :caption: part of CreateGame.cpp
    
    //------------------------------------------------------------------------------
    void Game::_InitializeUsingModules(Leggiero::IPlatformApplication *application)
    {
        _InitializeModule(Leggiero::EngineModuleIdType::kLog, application);
        _InitializeModule(Leggiero::EngineModuleIdType::kCrypto, application);
        _InitializeModule(Leggiero::EngineModuleIdType::kHTTP, application);
        _InitializeModule(Leggiero::EngineModuleIdType::kGraphics, application);
        _InitializeModule(Leggiero::EngineModuleIdType::kFont, application);
    }

    //------------------------------------------------------------------------------
    void Game::_RegisterUsingComponents()
    {
        _RegisterEngineComponent(new Leggiero::Graphics::GraphicResourceManagerComponent());
        _RegisterEngineComponent(new Leggiero::Input::TouchInputComponent());
        _RegisterEngineComponent(Leggiero::FileSystem::FileSystemPathComponent::CreateComponentObject());
        _RegisterEngineComponent(Leggiero::FileSystem::BundleFileResourceComponent::CreateComponentObject());
        _RegisterEngineComponent(new Leggiero::Font::GlyphManagerComponent());
        _RegisterEngineComponent(Leggiero::Sound::SoundMixerComponent::CreateComponentObject());
        _RegisterEngineComponent(Leggiero::Sound::BGMPlayerComponent::CreateComponentObject());

        Leggiero::Task::TaskManagerComponent *taskManager = Leggiero::Task::TaskManagerComponent::CreateComponentObject();
        Leggiero::Task::GraphicTask::EnableGraphicTaskSystem(taskManager);
        _RegisterEngineComponent(taskManager);

        _RegisterEngineComponent(new Leggiero::HTTP::Async::AsyncTaskHttpComponent());
    }
    

``_InitializeUsingModules`` 함수 내에서, 게임에서 사용할 모듈을 초기화해야 합니다.
이때, ``_ListAllModules`` 함수에서 나열한 모듈 중, 특정 모듈의 초기화 과정에서 의존하나 아직 초기화 되지 않은 모듈이 있다면, 의존성을 탐색하여 자동으로 초기화를 수행해 줍니다.
단, 모듈은 순환 의존성을 가질 수 없으며, 순환 의존성을 가지는 모듈을 초기화 하는 순간, 에러가 발생하며 게임은 시작되지 않습니다.

``_RegisterUsingComponents`` 함수 내에서는, 게임에서 사용할 엔진 컴퍼넌트를 직접 생성하여 등록합니다.
일부 컴퍼넌트는 생성 과정에서 게임에 맞는 설정을 수행할 수 있는 경우가 있고, 이 경우 필요한 설정을 마친 후 컴퍼넌트를 등록해야 합니다.
등록된 컴퍼넌트는 ``_RegisterUsingComponents`` 함수의 반환 후, 의존성 탐색 및 해결을 수행하며 적절한 순서로 초기화됩니다.
단, 엔진 컴퍼넌트는 순환 의존성을 가질 수 없고, 엔진 컴퍼넌트에서 필요로 하는 모듈은 ``_InitializeUsingModules`` 함수에서 직간접적으로 초기화 된 상태이어야 합니다.

.. Important:: Application 컴퍼넌트는 ``_RegisterUsingComponents`` 함수에서 등록하지 않습니다. Application 컴퍼넌트는 게임의 어플리케이션 그 자체를 추상화하는 컴퍼넌트로, Leggiero 엔진에 의해 등록됩니다.

게임 종료 시, 엔진 컴퍼넌트 및 초기화된 모듈의 종료 순서는 초기화 순서의 역순에 준합니다.
즉, 어떤 모듈이 다른 모듈에 의존한다면, 해당 모듈이 정리되는 시점에 의존 대상 모듈은 아직 정리 과정이 수행되지 않았음을 보장합니다.
