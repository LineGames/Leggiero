==========
Dependency
==========

Overview
========

Leggiero is mainly developed in C++, and uses following development environments and SDKs for convenience programming:

- Uses Microsoft `Visual Studio <https://visualstudio.microsoft.com/>`_ for **Windows PC** development platform targeted development.
  
  + Windows 10 OS is assumed for following development environment settings.
  

- `XCode <https://developer.apple.com/xcode/>`_ is needed for **iOS** application building.

- Android SDK and NDK are needed for **Android** application building, and Gradle and CMake are used to build the application.
  
  + Use of `Android Studio <https://developer.android.com/studio>`_ is recommended.


Visual Studio
=============

Leggiero uses Visual Studio 2022 for Windows PC development platform in developing and building.

  Community edition can be used. The build process worked normally in version 17.2.6 at the time this document having written.
  Lower versions may be used to build Leggiero, but there is no guarantee.

If you are an individual developer meet some conditions, Visual Studio Community 2022 is available for free at the following page:

  `https://visualstudio.microsoft.com/ <https://visualstudio.microsoft.com/>`_


Android
=======

Leggiero engine has minimum API level requirement for Android platform as **API Level 21** (Android 5.0 Lollipop).

JDK
---
JDK is needed to compile Android Java code and to execute some build tools.

Please install a JDK supporting Java 8+. You need to set following two environment variables after that:

  - **JAVA_HOME**
    
    + Set *JAVA_HOME* environment variable as JDK's root directory path.
      
    
  - **Path**
    
    + Add JDK's *bin* directory into *Path* environment variable.
      
    

Android SDK
-----------
Android SDK is needed to build Android application.

Install Android SDK with `Android Studio <https://developer.android.com/studio>`_ or other options.

After install Android SDK, setting for following environment variables is needed:

  - **ANDROID_HOME**
    
    + Set *ANDROID_HOME* environment variable as the path where Android SDK installed.
      
    
  - **Path**
    
    + Add Android SDK's *platform-tools* and *tools* directories into *Path* environment variable.
      
    

Android NDK
-----------
Android NDK is needed to build Leggiero's native C++ engine/game code.

Download Android NDK, **Revision r18b** from `NDK Revision History <https://developer.android.com/ndk/downloads/revision_history>`_ page, and unzip to any path where you want.

.. Note:: We used NDK r25 to develop Leggiero.

And following environment variables setting is needed:

  - **ANDROID_NDK_ROOT**
    
    + Set *ANDROID_NDK_ROOT* environment variable as the path where Android NDK have been extracted.
      
    
  - **Path**
    
    + Add the location of Android NDK into *Path* environment variable.
      
    

Gradle
------
Gradle is needed for Android application building.

We set Gradle Wrapper for Android project for Gradle 7.5.

CMake
-----------------------------------
`CMake <https://cmake.org/>`_ is needed to build  Android native C++ codes.

.. Note:: We tested CMake 3.23.2 for the engine.


iOS
===

Leggiero engine is targeting **iOS 12.3** and over. Lower version can be setted, but there is no guarantee to work.

Leggiero can be built using XCode in Mac OS X. Usual iOS development setting is needed to build.

New project created by Project Creator tool is not using CocoaPods, but you can apply it using ``pod install`` . LegToy project is an example for CocoaPods.


Other Dependencies
==================

- Only 64-bit architecture machine and OS setting is supported for Windows PC platform.
  
- OpenAL redist binary is needed for OpenAL sound output in Windows platform. To run a Leggiero project in Windows PC platform, you should install it using installer at *Dependent\\Win-X64\\OpenAL\\redist\\oalinst.exe* in the library repository.
  
