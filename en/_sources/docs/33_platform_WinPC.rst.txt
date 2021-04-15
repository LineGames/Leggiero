===================
Windows PC Platform
===================

Leggiero supports Windows 10 PC of x64 architecture as an development platform.
Runtime debugging on Visual Studio is very useful for game logic development.


Development Environment
=======================

- Develop and build in Visual Studio environment.
  
- Uses MSVC to compile C++ codes.
  


Libraries
=========

- Windows API is used to support platform functions.
  
- Uses `PowerVR SDK <https://powervr-graphics.github.io/Native_SDK/>`_ which supports EGL for OpenGL ES support.
  
- Uses `OpenAL Soft <https://openal-soft.org/>`_ for OpenAL support.
  
  + Dynamically linked as DLL file.
    
  
- Adopt `libcurl <https://curl.se/libcurl/>`_ library for request over HTTP.
  
- `POSIX Threads for Windows <https://sourceforge.net/projects/pthreads4w/>`_ is used for Pthread support.
  
