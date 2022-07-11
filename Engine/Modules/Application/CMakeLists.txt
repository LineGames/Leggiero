cmake_minimum_required(VERSION 3.18.1)

include("../../Platform/Platform.Android/cmake/AndroidCommon.cmake")


add_library(LE_M_Application STATIC)
le_tgt_setCppOptions(LE_M_Application)


target_include_directories(LE_M_Application PUBLIC ..)

target_link_libraries(LE_M_Application
    PRIVATE 
        LE_Libraries
        LE_Basic LE_Utility LE_Engine
)


target_sources(LE_M_Application
    PUBLIC
        AppPlatformType.h DeviceCommonTypes.h
        ApplicationComponent.h
        IAppInformationProvider.h IDeviceInformationProvider.h
        IPlatformAppControl.h
        SystemEventDispatcher.h SystemEventObserver.h RawTouchEventDispatcher.h RawTouchEventObserver.h
        IAppTextInputController.h IAppWebViewController.h
        
    PRIVATE
        ApplicationComponent.cpp
        SystemEventDispatcher.cpp RawTouchEventDispatcher.cpp
)
