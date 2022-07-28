=========
Changelog
=========


v0.10.1 (2022-07-28, current)
=============================

- Fixed some issues in iOS platform due to usage of MetalANGLE library.
  
  + Changed some **glFinish()** calls to **glFlush()** to avoid graphic system stuck issue due to infinity waiting for MTLCommandBuffer finish.
  
  + Provide interfaces for custom *UIView* to show during screen black-outed time just after launch screen and before.
  

- Modify Gralde build script for Android platform to appropriately clean built APKs.
  


v0.10.0 (2022-07-27)
====================

- We migrated Androud build system to Gradle and CMake based, and also made compatible with Android Studio.

- We upgraded the version of Visual Studio for WinPC development platform and Project Creator tool.

- Support for 32-bit architectures have been removed in iOS platform.

- `MetalANGLE <https://github.com/kakashidinho/metalangle>`_ framework in iOS platform have upgraded to GLES3 0.0.8 version.
  
  + Forced scaling animation effect during app starting have been removed.
  

- Some libraries have been upgraded:
  
  + including OpenSSL which upgraded to 1.1.1q.
  

- Changed some libraries, including zlib and libpng in WinPC platform, to prebuilt stuffs.

- Fixed some bugs:
  
  + Fix deadlock issue of Sound module in WinPC platform
  
  + Fix invalid screen sizing issue in some iOS devices
  

- Some technical improvements including:
  
  + Deal with build warnings
  
  + Make Oboe library setting more appropriately for Android API versions
  
  + String literal to std::string conversion issue under C++20 standard
  


v0.9.0
======

Initial release.
