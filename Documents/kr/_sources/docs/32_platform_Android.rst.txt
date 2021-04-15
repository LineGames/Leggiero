================
Android Platform
================

Leggiero는 정식 타깃 플랫픔으로 Android 기기를 지원합니다.


Supporting Details
==================

- API Level 21 (Android  5.0 Lollipop) 이상 버전의 OS를 지원
  
    + 빌드 타깃 API Level은 28
    
  
- ARMv7 및 AArch64 아키텍처 지원
  


Development Environment
=======================

- Visual Studio 상에서 NVIDIA Nsight Tegra, Visual Studio Edition을 이용하여 개발 및 빌드 수행
  
  + 일부 빌드 설정은 각 게임 프로젝트의 Android 앱 프로젝트에서, gradle override 파일을 통해 수정할 수 있다.
    
  
- C++ 코드의 컴파일을 위해 Clang 및 LLVM standard library를 사용
  


Libraries
=========

- 사운드 재생 기능 지원을 위해 `Oboe <https://github.com/google/oboe>`_ 라이브러리 사용
  
- HTTP 요청을 위해 `libcurl <https://curl.se/libcurl/>`_ 라이브러리 사용
  
  + 함께 embed 된 OpenSSL에 의존하여 SSL 통신 수행 가능
    
  
