===================
Windows PC Platform
===================

Leggiero는 개발용 플랫픔으로 x64 아키텍처의 Windows 10 PC를 지원합니다.
Visual Studio에서의 런타임 디버깅 기능 등은 게임 로직 개발에 유용하게 사용할 수 있습니다.


Development Environment
=======================

- Visual Studio 상에서 개발 및 빌드 수행
  
- C++ 코드의 컴파일을 위해 MSVC 사용
  


Libraries
=========

- 플랫폼 기능 지원을 위해 Windows API 사용
  
- OpenGL ES 지원을 위해, EGL을 지원하는 `PowerVR SDK <https://powervr-graphics.github.io/Native_SDK/>`_ 사용
  
- OpenAL 지원을 위해 호환 라이브러리인 `OpenAL Soft <https://openal-soft.org/>`_ 라이브러리 사용
  
  + DLL의 형태로 동적 연결하여 참조
    
  
- HTTP 요청을 위해 `libcurl <https://curl.se/libcurl/>`_ 라이브러리 사용
  
- Pthread 지원을 위해 `POSIX Threads for Windows <https://sourceforge.net/projects/pthreads4w/>`_ 라이브러리 사용
  
