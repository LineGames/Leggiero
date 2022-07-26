==========
Dependency
==========

Overview
========

Leggiero는 C++로 작성되었으며, 개발 과정에서의 편의성을 위해 아래와 같은 개발 환경 및 SDK를 이용합니다:

- **Windows PC** 플랫폼에서의 개발 환경을 위해 Microsoft 사의 `Visual Studio <https://visualstudio.microsoft.com/ko/>`_ 를 사용합니다.
  
  + 아래의 Windows PC 개발 환경 설정에 대한 내용은, Windows 10에서의 사용을 기준으로 합니다.
    
  
- **iOS** 앱 빌드를 위해서는 `XCode <https://developer.apple.com/kr/xcode/>`_ 가 필요합니다.
  
- **Android** 앱 빌드를 위해서는 Android SDK 및 NDK가 필요하며, Gradle과 CMake 기반 NDK 프로젝트를 빌드할 수 있는 기타 도구들이 필요합니다.
  
  + `Android Studio <https://developer.android.com/studio>`_ 의 사용을 추천 드립니다.
    
  


Visual Studio
=============

개발용 Windows PC 환경을 대상으로 한 개발과 빌드를 위해, Leggiero에서는 Visual Studio 2022를 이용하였습니다.

  Community 에디션도 이용이 가능하며, 이 문서가 만들어지는 시점에서 17.2.6 버전을 사용하여 정상 빌드 동작을 확인했습니다.
  이하의 버전에도 어느 정도 호환이 가능할 것으로 예상되나, 동작을 보증하지는 않습니다.

Visual Studio Community 2022는, 일정한 조건을 만족하는 개인 사용자라면, 아래의 페이지에서 무료로 다운받아 설치가 가능합니다.

  `https://visualstudio.microsoft.com/ko/ <https://visualstudio.microsoft.com/ko/>`_


Android
=======

Leggiero 엔진은 **API Level 21** (Android 5.0 Lollipop) 이상 버전의 Android 운영체제를 타깃으로 하여 빌드됩니다.

JDK
---
Android Java 코드 컴파일 및 관련 툴의 활용을 위해 JDK가 필요합니다.

라이선스를 고려하여 Java 8을 지원하는 적절한 JDK를 설치한 다음, 아래의 두 환경변수 설정이 필요합니다:

  - **JAVA_HOME**
    
    + JDK의 루트 디렉터리를 *JAVA_HOME* 환경변수로 설정해 주셔야 합니다.
      
    
  - **Path**
    
    + 기존 *Path* 환경변수에 JDK의 *bin* 디렉터리를 추가해 주어야 합니다.
      
    

Android SDK
-----------
Android 어플리케이션의 빌드를 위해 Android SDK가 필요합니다.

`Android 스튜디오 <https://developer.android.com/studio>`_ 또는 다른 옵션을 통해 Android SDK를 설치해 주세요.

설치 후, 아래의 환경변수 설정이 필요합니다:

  - **ANDROID_HOME**
    
    + Android SDK가 설치된 디렉터리를 *ANDROID_HOME* 환경변수에 설정해 주세요.
      
    
  - **Path**
    
    Android SDK의 툴 바이너리를 *Path* 환경변수에 등록해 주세요.
    
    + 기존 *Path* 환경변수에 Android SDK의 *platform-tools* 와 *tools* 서브디렉터리를 추가해 주세요.
      
    

Android NDK
-----------
Leggiero의 Android Native C++ 엔진/게임 코드 빌드를 위해 Android NDK가 필요합니다.

`NDK 다운로드 <https://developer.android.com/ndk/downloads>`_ 페이지에서 Android NDK를 다운로드 하셔서, 원하는 위치에 압축을 풀어주세요.

.. Note:: 문서 작성 시점에서, Leggiero Android 버전의 개발 및 빌드 확인은 NDK r25를 이용하여 수행되었습니다.

이후, 아래의 환경변수 설정이 필요합니다:

  - **ANDROID_NDK_ROOT**
    
    + Android NDK가 위치한 디렉터리를 *ANDROID_NDK_ROOT* 환경변수에 설정해 주세요.
      
    
  - **Path**
    
    + 기존 *Path* 환경변수에 Android NDK가 위치한 디렉터리를 추가해 주세요.
      
    

Gradle
------------
Android 어플리케이션의 빌드를 위해 Gradle이 필요합니다.

Android 프로젝트의 Gradle Wrapper 설정은 7.5 버전을 기준으로 작성되어 있습니다.

CMake
------------
Android용 native C++ 코드 빌드를 위해서, Leggiero에서는 `CMake <https://cmake.org/>`_ 를 사용합니다.

.. Note:: 문서 작성 시점에서, Leggiero Android 버전의 개발 및 빌드 확인은 CMake 3.23.2 버전을 이용하여 수행되었습니다.


iOS
===

Leggiero 엔진은 **iOS 12.3** 버전 이상을 타깃으로 하여 빌드됩니다. 프로젝트 설정을 고쳐 이하의 버전에도 어느 정도는 호환이 가능하게 빌드할 수 있을지 모르겠지만, 동작을 보증하지는 않습니다.

OS X 상에서 XCode를 통해 Leggiero를 빌드하실 수 있으며, 일반적인 iOS 개발을 위한 환경 설정 방법을 참고하셔서 설치 등을 수행해 주시면 됩니다.

Leggiero를 통해 생성할 수 있는 새 프로젝트는 CocoaPods이 적용되지 않은 상태입니다만, ``pod install`` 과 같이 프로젝트에 CocoaPods를 직접 적용하실 수 있습니다. LegToy 프로젝트는 (비록 아무것도 사용하지 않지만) CocoaPods 적용의 예시를 보여줍니다.


Other Dependencies
==================

- Windows PC 개발 플랫폼은 64비트 환경만을 지원합니다. x64 아키텍처의 바이너리가 실행되는 OS 및 CPU 환경이어야 합니다.
  
- Windows 환경에서 OpenAL 사운드 출력을 위해서 OpenAL redist binary가 필요합니다. 라이브러리 저장소 내의 *Dependent\\Win-X64\\OpenAL\\redist\\oalinst.exe* 인스톨러를 이용해 설치하셔야 Windows 바이너리의 정상 실행이 가능합니다.
  
- OpenGL 구현 등과 관련하여, 그래픽 카드 제조사의 최신 드라이버 업데이트를 적용하여야 정상 동작이 가능할 수도 있습니다.
  
