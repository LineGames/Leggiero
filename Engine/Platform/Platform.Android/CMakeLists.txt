cmake_minimum_required(VERSION 3.18.1)

include("cmake/AndroidCommon.cmake")


add_library(LE_PlatformA STATIC)
le_tgt_setCppOptions(LE_PlatformA)


target_include_directories(LE_PlatformA PUBLIC .)

target_link_libraries(LE_PlatformA
    PRIVATE 
        LE_Libraries
        LE_Basic LE_Utility LE_Engine
        LE_M_Application
)


target_sources(LE_PlatformA
    PUBLIC
        AndroidCommon.h AndroidPlatformSetting.h AndroidPlatformApplication.h
        AndroidPlatform/AndroidJNISupport.h AndroidPlatform/ApplicationJNIInterface.cpp
        AndroidPlatform/TouchJNIInterface.cpp AndroidPlatform/OpenGL/OpenGLJNI.cpp
        Service/AndroidTextInputDialog.h Service/AndroidWebView.h
        
    PRIVATE
        AndroidCommon.cpp
        AndroidPlatformApplication.cpp AndroidPlatformApplication_AppControl.cpp AndroidPlatformApplication_Events.cpp AndroidPlatformApplication_Touch.cpp
        AndroidApplicationComponent.cpp
        AndroidPlatform/_JNIInternal.h
        AndroidPlatform/AndroidJNISupport.cpp
        AndroidPlatform/OpenGL/PlatformGLThreadContext.cpp
        Service/AndroidTextInputDialog.cpp Service/AndroidWebView.cpp
)
