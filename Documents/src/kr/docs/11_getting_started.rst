===============
Getting Started
===============

이 문서에서는, Leggiero의 개발을 시작할 수 있는 방법에 대한 예시를 소개합니다.
이 문서의 내용을 적용하기 전, `Dependency <10_dependency.html>`_ 문서를 참고하여 개발 환경을 세팅해 주시기 바랍니다.


Engine Setup
============

Get Engine Source Code
----------------------
다음과 같이, GitHub 저장소에서 최신 Leggiero 엔진 코드를 받을 수 있습니다:

.. code-block:: console
  
  git clone https://github.com/LineGames/Leggiero.git
  

또는, 릴리즈 된 소스를 이용하거나 다른 브랜치를 선택하여 원하는 버전의 소스코드를 얻을 수 있습니다.

원하는 위치에 엔진 코드를 저장해 주세요. **엔진 루트** 디렉터리에는 *Documents*, *Engine*, *Tools* 서브디렉터리가 있어야 합니다.

.. Note:: 엔진 코드는 프로젝트 간에 공유됩니다. 각 프로젝트는 엔진 루트 디렉터리에 프로젝트 디렉터리를 가지게 됩니다.

Get Engine Library Source Code
------------------------------
버전 관리의 편의성과 코드의 양, 라이선스 고려사항 등의 이유로 엔진에서 사용하는 외부 라이브러리는 별도의 저장소에서 호스팅 됩니다.

앞서 받은 엔진 코드의 *Engine* 서브디렉터리에서 아래와 같이 Leggiero 엔진 라이브러리 코드를 받아주세요:

.. code-block:: console
  
  git clone https://github.com/LineGames/Leggiero-Library.git Libraries
  

.. Note:: 엔진 라이브러리 코드는 엔진 루트 경로에서 *Engine/Libraries* 경로에 위치해야 합니다. 폴더 명에 주의하세요.

엔진 루트 경로의 *Engine* 서브디렉터리는 *Common*, *Libraries*, *Modules*, *Platform* 서브디렉터리를 갖게 됩니다.

Do I need to build engine?
--------------------------
별도로 엔진 프로젝트들을 빌드하실 필요는 없습니다. 엔진의 빌드는, 각 게임 프로젝트의 빌드 시 함께 이뤄집니다.


Build and try LegToy
====================

LegToy는 Leggiero 엔진의 기능과 사용법에 대한 데모를 포함한 예시 프로젝트입니다.

**엔진 루트 디렉터리에서** 다음과 같이 LegToy 프로젝트 코드를 받을 수 있습니다:

.. code-block:: console
  
  git clone https://github.com/LineGames/LegToy.git
  

WinPC Binary Build
------------------
Visual Studio를 이용하여 Windows PC 개발 플랫폼의 바이너리를 빌드하실 수 있습니다.

- *LegToy 프로젝트 폴더* 내의 **LegToy.sln** 솔루션 파일을 열어주세요.
  
- 술루션 탐색기에서 App 필터 아래의 **LegToy.WinPC** 프로젝트를 찾아, 시작 프로젝트로 설정해 주세요.
  
- 빌드 구성 중 활성 솔루션 플랫폼을 x64로 맞춰주세요.
  
- 이제 빌드와 런타임 디버깅이 가능합니다. 빌드 결과물은 프로젝트 폴더 내 Binary 폴더 아래에 위치합니다.
  

.. Hint:: 만약 엔진 및 프로젝트 디렉터리가 긴 경로 상에 위치한다면, 운영체제 설정에 따라 중간 빌드 결과물이 너무 긴 경로(약 260자 이상)에 위치하여 빌드가 실패할 수 있습니다. 경로 길이 제한을 푸는 방법이 있다고 알려져 있지만, 모든 경우에 제대로 적용되지는 않는 것 같아, 가급적 짧은 경로에 엔진 디렉터리를 위치시키시기를 권장합니다.

iOS App Build
-------------

- *LegToy 프로젝트 폴더* 내의 **LegToy.xcodeproj** 프로젝트를 XCode로 열어주세요.
  
- LegToy 앱 타깃을 선택하고, 원하는 기기를 위해 빌드하세요.
  
  + 실 기기를 위해 빌드하는 경우 서명 관련 설정이 필요할지도 모릅니다. 본인의 개발자 계정에 맞춰 프로젝트 설정을 하시고 빌드해 주세요.
    
  
.. Note:: iOS 시뮬레이터를 대상으로 한 빌드는 현재 지원되지 않습니다.

Android App Build
-----------------

- *LegToy 프로젝트 폴더* 아래의 **Sources/App/LegToy.Android** 폴더가 Android Studio 프로젝트 폴더가 됩니다.
  
- Android Studio, 또는 원하는 빌드 도구를 이용해 프로젝트를 빌드해 주세요.
  
.. Hint:: 다음 Gradle 환경설정을 통해 사용할 빌드 도구의 버전을 지정하는 것이 유용할 수도 있습니다.
  
  - 프로젝트 gradle 파일의 *ext.ndkVersion* 및 모듈 gradle 파일의 *android.ndkVersion*
  
  - 모듈 gradle 파일의 *android.externalNativeBuild.cmake.version*
  


Start with a NEW project
========================

Project Creator 툴을 이용해 Leggiero 엔진을 위한 새 프로젝트를 생성할 수 있습니다.

Project Creator
---------------
엔진 저장소 내, *Tools/ProjectCreator* 내의 툴 바이너리 *(.NET 6.0 런타임 필요)* 를 이용하여 새 프로젝트를 생성할 수 있습니다.

- **ProjectCreatorTool.exe** 은 커맨드라인에서의 프로젝트 생성을 지원하는 툴 바이너리입니다.
  
- **ProjectCreatorGUI.exe** 는 GUI를 통한 프로젝트 생성을 지원합니다.
  
Create a Project
----------------
Project Creator 툴을 이용해 원하는 설정의 새 프로젝트를 생성할 수 있습니다.

생성된 프로젝트를 담은 폴더는 엔진 루트 폴더 내에 위치하며, 해당 폴더 내의 Visual Studio 솔루션과 Android Studio 프로젝트(**Source/App/*.Android**), 그리고 XCode 프로젝트를 이용해 프로젝트 바이너리를 빌드하실 수 있습니다.

.. Note:: 현재 엔진 루트 디렉터리와 프로젝트 디렉터리 간의 상대 위치를 변경을 지원하지 않습니다.

