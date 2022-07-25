cmake_minimum_required(VERSION 3.18.1)

include("../../Platform/Platform.Android/cmake/AndroidCommon.cmake")


add_library(LE_M_FileSystem STATIC)
le_tgt_setCppOptions(LE_M_FileSystem)


target_include_directories(LE_M_FileSystem PUBLIC ..)

target_link_libraries(LE_M_FileSystem
    PRIVATE
        LE_Libraries
        LE_Basic LE_Utility LE_Engine
        LE_PlatformA LE_M_Application
)


target_sources(LE_M_FileSystem
    PUBLIC
        FileSystemPathComponent.h FileSystemPathComponent_NPO.h
        BundleFileResourceComponent.h BundleFileResourceComponent_NPO.h
        FileSystemUtility.h
        _Internal/_AndroidFileSystemJNIInterface.cpp
        
    PRIVATE
        FileSystemPathComponent.cpp BundleFileResourceComponent.cpp
        FileSystemUtility.cpp FileSystemUtility_Android.cpp
        _Internal/_AndroidFileSystemPathComponent.h _Internal/_AndroidFileSystemPathComponent.cpp
        _Internal/_AndroidBundleFileResourceComponent.h _Internal/_AndroidBundleFileResourceComponent.cpp
)
