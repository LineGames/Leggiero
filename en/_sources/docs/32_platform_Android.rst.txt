================
Android Platform
================

Leggiero supports Android devices as an official target platform.


Supporting Details
==================

- Supports Android OS of API Level 21 (Android  5.0 Lollipop) and later.
  
    + Target API Level is 28.
    
  
- Supports ARMv7 and AArch64 architectures.
  


Development Environment
=======================

- NVIDIA Nsight Tegra, Visual Studio Edition is our primary build tool for developing and building.
  
  + Some build settings can be set using gradle override in Android app project of each game project.
    
  
- Uses Clang and LLVM standard library to compile C++ codes.
  


Libraries
=========

- Uses `Oboe <https://github.com/google/oboe>`_ library for sound play functions.
  
- Adopt `libcurl <https://curl.se/libcurl/>`_ library for request over HTTP.
  
  + SSL is available with OpenSSL.
    
  
