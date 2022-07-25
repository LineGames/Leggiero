cmake_minimum_required(VERSION 3.18.1)

include("../../Platform/Platform.Android/cmake/AndroidCommon.cmake")


add_library(LE_M_Sound STATIC)
le_tgt_setCppOptions(LE_M_Sound)


target_include_directories(LE_M_Sound PUBLIC ..)


find_library(nlib-OpenSLES OpenSLES)
find_package(oboe REQUIRED CONFIG)

target_link_libraries(LE_M_Sound
    PUBLIC
        oboe::oboe

    PRIVATE
        ${nlib-OpenSLES}
        LE_Libraries
        LE_Basic LE_Utility LE_Engine
        LE_PlatformA LE_M_Application
        LE_M_FileSystem
)


target_sources(LE_M_Sound
    PUBLIC
        SoundMixerComponent.h BGMPlayerComponent.h
        Common/SoundTypes.h Common/ISoundPlayingHandle.h Common/SoundPlayingContext.h Common/IBGMPlayingHandle.h
        Platform/AndroidBGMPlayer.h Platform/AndroidSoundJNIInterface.cpp Platform/AndroidBGMJNIInterface.cpp
        OboeBackend/OboeCommon.h OboeBackend/OboeSoundMixer.h OboeBackend/IOboeStreamRendering.h OboeBackend/OboeStream.h OboeBackend/OboeSoundPlayingContext.h OboeBackend/OboeBufferedSoundPlayingContext.h OboeBackend/OboeLoopingSoundPlayingContext.h
        Provider/ISoundProvider.h Provider/BakedSoundProvider.h Provider/LoopHoldSoundProvider.h Provider/MP3SoundProvider.h Provider/OggSoundProvider.h Provider/WavSoundProvider.h Provider/TestSoundProviders.h
        
    PRIVATE
        SoundMixerComponent.cpp BGMPlayerComponent.cpp
        Common/SoundTypes.cpp
        Platform/AndroidPlatformSound.cpp Platform/AndroidBGMPlayer.cpp Platform/_AndroidBGMPlayingContext.h Platform/_AndroidBGMPlayingContext.cpp
        OboeBackend/_OboeInternal.h OboeBackend/_OboeInternal.cpp OboeBackend/OboeCommon.cpp OboeBackend/OboeSoundMixer.cpp OboeBackend/OboeStream.cpp OboeBackend/OboeSoundPlayingContext.cpp OboeBackend/OboeBufferedSoundPlayingContext.cpp OboeBackend/OboeLoopingSoundPlayingContext.cpp
        Provider/BakedSoundProvider.cpp Provider/LoopHoldSoundProvider.cpp Provider/MP3SoundProvider.cpp Provider/OggSoundProvider.cpp Provider/WavSoundProvider.cpp Provider/TestSoundProviders.cpp
)
