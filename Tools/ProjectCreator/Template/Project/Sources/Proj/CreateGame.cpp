////////////////////////////////////////////////////////////////////////////////
// Create Game.cpp (${{ProgramName}} - ${{ProgramName}})
//
// Implementation of Game Creation Interface
////////////////////////////////////////////////////////////////////////////////


// Interface Header
#include <Engine/Application/CreateGame.h>

// Leggiero.Engine
#include <Engine/Application/IPlatformApplication.h>

// Leggiero.Graphics
#include <Graphics/GraphicResourceManagerComponent.h>

// Leggiero.Input
#include <Input/TouchInputComponent.h>

// Leggiero.FileSystem
#include <FileSystem/FileSystemPathComponent.h>
#include <FileSystem/BundleFileResourceComponent.h>

// Leggiero.Font
#include <Font/GlyphManagerComponent.h>

// Leggiero.Sound
#include <Sound/SoundMixerComponent.h>
#include <Sound/BGMPlayerComponent.h>

// Leggiero.Task
#include <Task/TaskManagerComponent.h>
#include <Task/GraphicTask/GraphicTaskSystem.h>

// Leggiero.HTTP
#include <HTTP/AsyncTaskHttpComponent.h>

// ${{ProgramName}}
#include "Game.h"


//////////////////////////////////////////////////////////////////////////////// Create Game Interface

namespace Leggiero
{
	//------------------------------------------------------------------------------
	// Create a Game Object for ${{ProgramName}}
	std::shared_ptr<IGame> CreateGame(IPlatformApplication *application, IGameLaunchOption *launchOption)
	{
		std::shared_ptr<${{ProgramName}}::Game> concreteGame = std::make_shared<${{ProgramName}}::Game>();

		std::shared_ptr<IGameInitializer> gameCreator = std::dynamic_pointer_cast<IGameInitializer>(concreteGame);
		gameCreator->_InitializeGameEngineLibrary(application);
		gameCreator->_AssembleGameEngineComponents(application);

		return concreteGame;
	}
}


//////////////////////////////////////////////////////////////////////////////// Game Creation Implementation

namespace ${{ProgramName}}
{
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
}
