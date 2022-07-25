cmake_minimum_required(VERSION 3.18.1)

include("../../Platform/Platform.Android/cmake/AndroidCommon.cmake")


add_library(LE_M_Input STATIC)
le_tgt_setCppOptions(LE_M_Input)


target_include_directories(LE_M_Input PUBLIC ..)

target_link_libraries(LE_M_Input
    PRIVATE
        LE_Libraries
        LE_Basic LE_Utility LE_Engine
        LE_PlatformA LE_M_Application
)


target_sources(LE_M_Input
    PUBLIC
        InputCommonTypes.h TouchInputComponent.h IImmediateTouchInputEventObserver.h
        Touch/TouchEvent.h Touch/LastTouchState.h Touch/EventBasedTouchContext.h
        
    PRIVATE
        TouchInputComponent.cpp
        Touch/EventBasedTouchContext.cpp
)
