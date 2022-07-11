cmake_minimum_required(VERSION 3.18.1)

# Project App
project("${{ProgramName}}")

add_library(${{ProgramName}} SHARED
        jni/Android.cpp
        ${{ProgramName}}AndroidSetting.h ${{ProgramName}}AndroidSetting.cpp
        ${{ProgramName}}Android.cpp
)


# Android system library
find_library(nlib-z z)
find_library(nlib-log log)
find_library(nlib-android android)
find_library(nlib-GLESv3 GLESv3)
find_library(nlib-EGL EGL)
find_library(nlib-OpenSLES OpenSLES)

# App library
find_package(oboe REQUIRED CONFIG)

# Leggiero
add_subdirectory("../../../../../../../../Engine" "../../../../../../../../.Build/Android/Leggiero")


# Project Logic
add_subdirectory("../../../../../../${{ProgramName}}" "../../../../../../../../.Build/${{ProgramName}}")
add_dependencies(${{ProgramName}}_Game Leggiero)


target_link_libraries(${{ProgramName}}
        ${nlib-z} ${nlib-log} ${nlib-android} ${nlib-GLESv3} ${nlib-EGL} ${nlib-OpenSLES}
        oboe::oboe
        Leggiero
        ${{ProgramName}}_Game
)