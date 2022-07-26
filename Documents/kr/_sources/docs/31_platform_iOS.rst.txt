============
iOS Platform
============

Leggiero는 정식 타깃 플랫픔으로 iOS 기기를 지원합니다.


Supporting Details
==================

- iPhone 및 iPad 공통 지원
  
- iOS 12.3 이상의 버전을 지원
  
- ARM64 아키텍처 지원
  


Development Environment
=======================

- XCode 환경을 지원
  
- 공통 C++ 코드의 컴파일을 위해 Clang 사용
  


Libraries
=========

- OpenGL ES 지원을 위해, Metal backend를 사용하는 `MetalANGLE <https://github.com/kakashidinho/metalangle>`_ 라이브러리 사용
  
- OpenAL 지원을 위해 호환 라이브러리인 `OpenAL Soft <https://openal-soft.org/>`_ 라이브러리 사용
  
  + Framework의 형태로 embed 하여, 동적으로 연결하여 사용
    
  
