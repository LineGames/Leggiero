============
iOS Platform
============

Leggiero supports iOS as an official target platform.


Supporting Details
==================

- Supports both iPhone and iPad.
  
- For iOS version 12.3 and later.
  
- Supports ARM64 architecture.
  


Development Environment
=======================

- Compatible with XCode development environment.
  
- Uses Clang to compile C++ code.
  


Libraries
=========

- Adopts `MetalANGLE <https://github.com/kakashidinho/metalangle>`_ library to support OpenGL ES with Metal backend.
  
- Uses `OpenAL Soft <https://openal-soft.org/>`_ for OpenAL support.
  
  + Dynamically linked as an embeded framework.
    
  


Notes
=====

In iOS platform, screen is black-outed during a few frame after splash screen shown and before MGLKView is rendered.
  
  + For seamless presentation, you can override **getInitialShowingUIView** method in *LeggieroViewController* class to set custom UIView to show.
    
  
