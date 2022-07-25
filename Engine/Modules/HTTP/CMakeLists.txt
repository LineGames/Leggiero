cmake_minimum_required(VERSION 3.18.1)

include("../../Platform/Platform.Android/cmake/AndroidCommon.cmake")


add_library(LE_M_HTTP STATIC)
le_tgt_setCppOptions(LE_M_HTTP)


target_include_directories(LE_M_HTTP PUBLIC ..)

target_link_libraries(LE_M_HTTP
    PRIVATE
        LE_Libraries
        LE_Basic LE_Utility LE_Engine
        LE_PlatformA
        LE_M_Task
)


target_sources(LE_M_HTTP
    PUBLIC
        HttpModuleInterface.h HttpRequest.h HttpResult.h HttpUtility.h AsyncTaskHttpComponent.h
        Async/AsyncUtility.h Async/AsyncHttpRequestTask.h Async/AsyncHttpDownloadTask.h
        cURL/cURLUtility.h cURL/cURLAsyncHttpRequestTask.h cURL/cURLAsyncHttpDownloadTask.h
        
    PRIVATE
        HttpModuleInterface.cpp HttpResult.cpp HttpUtility.cpp AsyncTaskHttpComponent.cpp
        Async/AsyncUtility.cpp Async/AsyncHttpTasks.cpp
        cURL/cURLUtility.cpp cURL/cURLHttpRequest.cpp cURL/cURLAsyncTaskHttpComponent.cpp cURL/cURLAsyncHttpRequestTask.cpp cURL/cURLAsyncHttpDownloadTask.cpp

)
