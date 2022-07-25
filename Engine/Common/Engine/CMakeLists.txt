cmake_minimum_required(VERSION 3.18.1)

include("../../Platform/Platform.Android/cmake/AndroidCommon.cmake")


add_library(LE_Engine STATIC)
le_tgt_setCppOptions(LE_Engine)


target_include_directories(LE_Engine PUBLIC ..)

target_link_libraries(LE_Engine PRIVATE LE_Libraries LE_Basic LE_Utility)


target_sources(LE_Engine
    PUBLIC
        Application/IGame.h
        Application/IGameInitializer.h Application/IGameLaunchOption.h Application/CreateGame.h
        Application/BaseGame.h Application/GameProcessAnchor.h Application/GameProcessAnchorObserver.h Application/IPlatformApplication.h
        Module/EngineComponentId.h Module/EngineComponent.h Module/EngineComponentHolder.h
        Module/EngineModuleId.h Module/EngineModuleInterface.h Module/EngineModuleException.h
        Toolbox/ModuledGame/ModuledGame.h
        Toolbox/Scene/IGameScene.h Toolbox/Scene/IGameSceneContext.h Toolbox/Scene/GameSceneTypes.h Toolbox/Scene/GameSceneHolder.h
        
    PRIVATE
        Application/BaseGame.cpp Application/GameProcessAnchor.cpp
        Module/EngineComponent.cpp Module/EngineComponentHolder.cpp Module/EngineModuleInterface.cpp
        Toolbox/ModuledGame/ModuledGame.cpp Toolbox/Scene/GameSceneHolder.cpp
)
