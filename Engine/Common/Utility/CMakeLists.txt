cmake_minimum_required(VERSION 3.18.1)

include("../../Platform/Platform.Android/cmake/AndroidCommon.cmake")


add_library(LE_Utility STATIC)
le_tgt_setCppOptions(LE_Utility)


target_include_directories(LE_Utility PUBLIC ..)

target_link_libraries(LE_Utility PRIVATE LE_Libraries LE_Basic)


target_sources(LE_Utility
    PUBLIC
        Data/BufferReader.h Data/MemoryBuffer.h
        Encoding/Base64.h Encoding/HexString.h Encoding/URLEncoding.h
        Math/BasicRect.h Math/BitMath.h Math/Easing.h Math/SimpleGeometry.h Math/SimpleMath.h Math/Vector.h
        Object/PointerHolder.h Object/VerySimpleObjectPool.h
        String/AsciiStringUtility.h String/IStringBag.h
        Sugar/EnumClass.h Sugar/EventNotifier.h Sugar/Finally.h Sugar/NonCopyable.h Sugar/SingletonPattern.h
        Threading/ManagedThreadPrimitives.h Threading/ThreadSleep.h
        
    PRIVATE
        Data/BufferReader.cpp Data/MemoryBuffer.cpp
        Encoding/Base64.cpp Encoding/HexString.cpp Encoding/URLEncoding.cpp
        Math/Easing.cpp
        Object/PointerHolder.cpp
        String/AsciiStringUtility.cpp
        Threading/ManagedThreadPrimitives.cpp Threading/ThreadSleep.cpp Threading/ThreadSleep_Android.cpp
)
