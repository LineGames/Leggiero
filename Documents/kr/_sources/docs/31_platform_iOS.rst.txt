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
    
  


Notes
=====

- 앱을 시작할 때, 스플래시 스크린을 표시한 후 MGLKView를 시작하는 도중, 첫 프레임이 온전히 랜더링 되기 전 검은 화면이 잠시 표시될 수 있습니다.
  
  + 심리스한 연출을 원하시면, *LeggieroViewController* 클래스의 **getInitialShowingUIView** 메서드를 오버라이드 해서 검은 화면 대신 표시될 UIView를 지정할 수 있습니다.
    
  
