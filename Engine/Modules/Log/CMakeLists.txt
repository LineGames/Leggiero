cmake_minimum_required(VERSION 3.18.1)

include("../../Platform/Platform.Android/cmake/AndroidCommon.cmake")


add_library(LE_M_Log STATIC)
le_tgt_setCppOptions(LE_M_Log)


target_include_directories(LE_M_Log PUBLIC ..)


find_library(nlib-log log)

target_link_libraries(LE_M_Log
    PRIVATE
        ${nlib-log}
        LE_Libraries
        LE_Basic LE_Utility LE_Engine
        LE_PlatformA
)


target_sources(LE_M_Log
    PUBLIC
        LogTypes.h LogModuleInterface.h Logger.h DebugLogger.h
        ILogWriter.h ThreadedLogWriter.h FileLogWriter.h PlatformDefaultLogWriter.h
        
    PRIVATE
        LogModuleInterface.cpp Logger.cpp DebugLogger.cpp
        ThreadedLogWriter.cpp FileLogWriter.cpp PlatformDefaultLogWriter_Android.cpp
        _Internal/_DebugLoggerInterface.h
)
