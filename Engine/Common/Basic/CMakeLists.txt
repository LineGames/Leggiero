cmake_minimum_required(VERSION 3.18.1)

include("../../Platform/Platform.Android/cmake/AndroidCommon.cmake")


add_library(LE_Basic STATIC)
le_tgt_setCppOptions(LE_Basic)


target_include_directories(LE_Basic PUBLIC ..)

target_link_libraries(LE_Basic PRIVATE LE_Libraries)


target_sources(LE_Basic
    PUBLIC
        LeggieroBasic.h
        
    PRIVATE
        _Internal/_CompileSettings.h
        _Internal/_LeggieroFlags.h
        _Internal/_Warnings.h
        _Internal/_CommonTypes.h _Internal/_CommonTypes.cpp
)
