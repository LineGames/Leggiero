cmake_minimum_required(VERSION 3.18.1)

include("../../../Engine/Platform/Platform.Android/cmake/AndroidCommon.cmake")


add_library(${{ProgramName}}_Game STATIC)
le_tgt_setCppOptions(${{ProgramName}}_Game)


target_include_directories(${{ProgramName}}_Game PUBLIC .)

target_link_libraries(${{ProgramName}}_Game
    PRIVATE
        Leggiero
)


target_sources(${{ProgramName}}_Game
    PUBLIC
        Game.h
        
    PRIVATE
        Game.cpp CreateGame.cpp GameModules.cpp
        
        Scene/SceneIds.h Scene/Game_Scenes.cpp
        Scene/DefaultUIScene/DefaultUIScene.h Scene/DefaultUIScene/DefaultUIScene.cpp
)
