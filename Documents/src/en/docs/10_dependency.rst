==========
Dependency
==========

Overview
========

Leggiero is mainly developed in C++, and uses following development environments and SDKs for convenience programming:

- Uses Microsoft `Visual Studio <https://visualstudio.microsoft.com/>`_ for **Windows PC** and **Android** platform targeted development.
  
  + Windows 10 OS is assumed for following development environment settings.
    
  + For Android platform, `NVIDIA® Nsight™ Tegra®, Visual Studio Edition <https://developer.nvidia.com/nvidia-nsight-tegra>`_ is used for developing and building, and also requires Android SDK and NDK.
    
  
- `XCode <https://developer.apple.com/xcode/>`_ is needed for **iOS** application building.
  


Visual Studio
=============

Leggiero uses Visual Studio 2019 for Windows PC development platform and Android target platform in developing and building.

  Community edition can be used. The build process worked normally in version 16.9.3 at the time this document having written.
  Lower versions may be used to build Leggiero, but there is no guarantee.

If you are an individual developer meet some conditions, Visual Studio Community 2019 is available for free at the following page:

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

.. Attention:: SDK of API Level 28 should be installed because it is the most recent version supported by Nsight Tegra, Visual Studio Edition 3.6.

After install Android SDK, setting for following environment variables is needed:

  - **ANDROID_HOME**
    
    + Set *ANDROID_HOME* environment variable as the path where Android SDK installed.
      
    
  - **Path**
    
    + Add Android SDK's *platform-tools* and *tools* directories into *Path* environment variable.
      
    

Android NDK
-----------
Android NDK is needed to build Leggiero's native C++ engine/game code.

Download Android NDK, **Revision r18b** from `NDK Revision History <https://developer.android.com/ndk/downloads/revision_history>`_ page, and unzip to any path where you want.

.. Attention:: We have to use Revision r18b due to Nsight Tegra, Visual Studio Edition 3.6's supporing version limitation.

And following environment variables setting is needed:

  - **ANDROID_NDK_ROOT**
    
    + Set *ANDROID_NDK_ROOT* environment variable as the path where Android NDK have been extracted.
      
    
  - **Path**
    
    + Add the location of Android NDK into *Path* environment variable.
      
    

Ant & Gradle
------------
Gradle is needed for Android application building.

On the other hand, Apache Ant is also needed because Nsight Tegra, Visual Studio Edition usually requires Ant even we set Gradle as a build tool.

Download Gradle binary of **v4.10.X** in `Gradle Releases <https://gradle.org/releases/>`_ page and unzip it into where you want. And download binary version of Ant of **version 1.9.X** from `Apache Ant Download Page <https://ant.apache.org/bindownload.cgi>`_ unzip it too.

.. Attention:: Nsight Tegra, Visual Studio Edition 3.6 supprot Gradle up to 4 series versions. It cannot recognize Ant's version over 1.10, so we should use Ant of 1.9.X version.

Following environment variables setting is needed:

  - **GRADLE_HOME**
    
    + Set *GRADLE_HOME* as Gradle location.
      
    
  - **ANT_HOME**
    
    + Set *ANT_HOME* as Ant location.
      
    
  - **Path**
    
    + Add *bin* sub-directories of Gradle and Ant into *Path* environment variable.
      
    

Nsight Tegra, Visual Studio Edition
-----------------------------------
Please install `NVIDIA® Nsight™ Tegra®, Visual Studio Edition <https://developer.nvidia.com/nvidia-nsight-tegra>`_ . The most recent version is 3.6 at the time of this article written.

.. Note:: Please install the Visual Studio Extension for VS 2019.

After the installation, launch Visual Studio and open :guilabel:`Extensions` > :guilabel:`Tegra` > :guilabel:`Options...` menu. Select **Android Tools Locations** under **Nsight Tegra** , and check and fill correct locations for build tools.

.. Attention:: Some values are automatically filled using environment variables. But some settings, especially JDK location, could be filled with incorrect values. Please check and set to correct locations.


iOS
===

Leggiero engine is targeting **iOS 12.3** and over. Lower version can be setted, but there is no guarantee to work.

Leggiero can be built using XCode in Mac OS X. Usual iOS development setting is needed to build.

New project created by Project Creator tool is not using CocoaPods, but you can apply it using ``pod install`` . LegToy project is an example for CocoaPods.


Other Dependencies
==================

- Only 64-bit architecture machine and OS setting is supported for Windows PC platform.
  
- OpenAL redist binary is needed for OpenAL sound output in Windows platform. To run a Leggiero project in Windows PC platform, you should install it using installer at *Dependent\\Win-X64\\OpenAL\\redist\\oalinst.exe* in the library repository.
  
