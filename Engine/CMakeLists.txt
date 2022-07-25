cmake_minimum_required(VERSION 3.18.1)

# Leggiero
project(Leggiero
    LANGUAGES CXX
    VERSION 0.10.0
)

add_library(Leggiero INTERFACE)


# Engine Modules
add_subdirectory(Libraries)

add_subdirectory(Common/Basic)
add_subdirectory(Common/Utility)
add_subdirectory(Common/Engine)

add_subdirectory(Modules/Application)
add_subdirectory(Platform/Platform.Android)

add_subdirectory(Modules/FileSystem)
add_subdirectory(Modules/Graphics)
add_subdirectory(Modules/Input)
add_subdirectory(Modules/Sound)
add_subdirectory(Modules/Log)
add_subdirectory(Modules/Task)
add_subdirectory(Modules/Font)
add_subdirectory(Modules/Crypto)
add_subdirectory(Modules/HTTP)
add_subdirectory(Modules/LUI)


# Dependency
add_dependencies(LE_Basic
    LE_Libraries
)

add_dependencies(LE_Utility
    LE_Libraries LE_Basic
)

add_dependencies(LE_Engine
    LE_Libraries LE_Basic LE_Utility
)

add_dependencies(LE_M_Application
    LE_Libraries LE_Basic LE_Utility LE_Engine
)

add_dependencies(LE_PlatformA
    LE_Libraries LE_Basic LE_Utility LE_Engine LE_M_Application
)

add_dependencies(LE_M_FileSystem
    LE_Libraries LE_Basic LE_Utility LE_Engine LE_PlatformA LE_M_Application
)

add_dependencies(LE_M_Graphics
    LE_Libraries LE_Basic LE_Utility LE_Engine LE_PlatformA LE_M_Application LE_M_FileSystem
)

add_dependencies(LE_M_Input
    LE_Libraries LE_Basic LE_Utility LE_Engine LE_PlatformA LE_M_Application
)

add_dependencies(LE_M_Sound
    LE_Libraries LE_Basic LE_Utility LE_Engine LE_PlatformA LE_M_Application LE_M_FileSystem
)

add_dependencies(LE_M_Log
    LE_Libraries LE_Basic LE_Utility LE_Engine LE_PlatformA
)

add_dependencies(LE_M_Task
    LE_Libraries LE_Basic LE_Utility LE_Engine LE_PlatformA LE_M_Graphics
)

add_dependencies(LE_M_Font
    LE_Libraries LE_Basic LE_Utility LE_Engine LE_PlatformA LE_M_Graphics
)

add_dependencies(LE_M_Crypto
    LE_Libraries LE_Basic LE_Utility LE_Engine LE_PlatformA
)

add_dependencies(LE_M_HTTP
    LE_Libraries LE_Basic LE_Utility LE_Engine LE_PlatformA LE_M_Task
)

add_dependencies(LE_M_LegacyUI
    LE_Libraries LE_Basic LE_Utility LE_Engine LE_PlatformA LE_M_Application LE_M_FileSystem LE_M_Graphics LE_M_Input LE_M_Task LE_M_Font
)


# Engine Includes
target_include_directories(Leggiero
    INTERFACE
        Common
        Platform/Platform.Android
        Modules
)


target_link_libraries(Leggiero
    INTERFACE
        LE_Libraries
        LE_Basic LE_Utility LE_Engine
        LE_M_Application LE_PlatformA
        LE_M_FileSystem LE_M_Graphics LE_M_Input LE_M_Sound LE_M_Log LE_M_Task
        LE_M_Font LE_M_Crypto LE_M_HTTP
        LE_M_LegacyUI
)