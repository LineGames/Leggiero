================
Android Platform
================

Leggiero는 정식 타깃 플랫픔으로 Android 기기를 지원합니다.


Supporting Details
==================

- API Level 21 (Android  5.0 Lollipop) 이상 버전의 OS를 지원
  
    + 빌드 타깃 API Level은 32
    
  
- ARMv7 및 AArch64 아키텍처 지원
  


Development Environment
=======================

- Gradle을 통해 앱을 빌드하며, CMake를 이용해 NDK 코드를 빌드
  
  + Android Studio와 호환됩니다.
    
  


Libraries
=========

- 사운드 재생 기능 지원을 위해 `Oboe <https://github.com/google/oboe>`_ 라이브러리 사용
  
- HTTP 요청을 위해 `libcurl <https://curl.se/libcurl/>`_ 라이브러리 사용
  
  + 함께 embed 된 OpenSSL에 의존하여 SSL 통신 수행 가능
    
  
