==================
File System Module
==================

File System 모듈은 플랫폼 파일 시스템 접근에 대한 내용을 추상화하여 제공합니다.


Module Common Information
=========================

**Project:** *Modules/FileSystem*

**Namespace:** ``Leggiero::FileSystem``

Module Interfaces
-----------------
(없음)

Components
----------
- `Bundle File Resource Component`_
- `File System Path Component`_

Notes
-----
*FileSystemUtility.h* 헤더를 통해, 별도의 초기화 과정이 필요 없는 파일 시스템 관련 유틸리티 API 함수들을 이용하실 수 있습니다.


Bundle File Resource Component
==============================
게임 어플리케이션에 동봉된 번들 리소스에 대한 접근을 추상화하는 컴퍼넌트입니다.

**Type:** ``Leggiero::FileSystem::BundleFileResourceComponent``

**Component Id:** ``EngineComponentIdType::kBundleFileResource`` (12)

Dependencies
------------
(없음)

Overview
--------
게임 어플리케이션에 동봉되어 앱 설치 시 함께 저장되는 번들 리소스에 대한 접근을 추상화합니다.

iOS와 Windows PC 플랫폼의 경우, 번들 리소스는 대상 플랫폼 상의 개별 파일로 존재합니다. 하지만, Android에서는 번들 리소스는 패키징 된 에셋 번들 파일 안에 함께 존재하므로, 번들 리소스를 실제 플랫폼 운영체제 상의 파일로 간주해서는 안 됩니다.

Leggiero에서는 번들 내의 경로에 해당하는 개별 리소스에 대해, 바이너리로 표현되는 내용을 읽을 수 있는 인터페이스를 제공합니다.
당연한 이야기지만, 번들 리소스는 앱의 일부이므로, 논리적으로 앱 실행 시에 쓰기 접근을 할 수는 없습니다.


Non-Portable Interface
----------------------
iOS와 Windows PC 플랫폼에서는 각 번들 리소스는 실제 파일의 형태로 존재합니다. 최적화를 위해, portable 하지 않은 이런 가정에 의거한 접근 방식을, ``Leggiero::FileSystem::NPO::INonPortableOptimization_BundleFileResource`` 인터페이스를 통해 제공합니다.

유저는 컴퍼넌트에, 해당 최적화 요소가 동작하는지를 질의하여 지원 여부를 확인 후, 리소스에 대한 portable 하지 않은 방식의 접근을 수행할 수 있습니다.

Bundle Resource Packing
-----------------------
각 게임 프로젝트의 디렉터리 내의, *Bundle* 서브디렉터리 내의 내용이 번들 리소스로 앱에 동봉됩니다.

Leggiero는 플랫폼별로 최적화된 별도의 번들 리소스를 패키징 할 수 있는 매커니즘을 제공합니다.
*Bundle/Base* 디렉터리 내의 리소스는 전 플랫폼의 앱에 공통적으로 패키징됩니다. 이때, *Bundle/Base* 디렉터리가 번들 된 파일 리소스의 가상 경로의 루트 디렉터리로 간주됩니다.

한편, *Bundle/Platform* 디렉터리 내의 각 플랫폼 디렉터리가, 플랫폼별로 서로 다르게 패키징 되는 리소스를 담을 수 있습니다.
이때, 각 플랫폼 디렉터리(가령 *Bundle/Platform/iOS* )가 번들 리소스 가상 경로의 루트 디렉터리에 매핑되며, 기본적으로는 Base 리소스 중 같은 가상 경로 상의 리소스를 override 하는 형태로 작동합니다.
단, Android 플랫폼에서는 리소스 overriding이 불가능하며, 동일 가상 경로 상에 서로 다른 리소스가 존재한다면 앱 빌드 과정에서 에러를 발생시킵니다.


File System Path Component
==========================
게임 어플리케이션에서 이용할 수 있도록 플랫폼에서 제공하는, 파일 시스템 상의 공간을 추상화하는 컴퍼넌트입니다.

**Type:** ``Leggiero::FileSystem::FileSystemPathComponent``

**Component Id:** ``EngineComponentIdType::kFileSystemPath`` (11)

Dependencies
------------
(없음)

Overview
--------
플랫폼에서 어플리케이션이 사용할 수 있는 용도로 제공하는 파일 시스템 상의 공간을, 특성과 용도별로 추상화하여 제공합니다.
아래와 같은 종류의 데이터 디렉터리를 제공합니다:

+--------------+------------+--------------+
|         Name | Volatility | Cloud Backup |
+==============+============+==============+
| **MainData** | X          | Yes          |
+--------------+------------+--------------+
|  **RawData** | X          | No           |
+--------------+------------+--------------+
|    **Cache** | cache      | No           |
+--------------+------------+--------------+
|     **Temp** | temporary  | No           |
+--------------+------------+--------------+

iOS와 Android의 경우, iCloud와 Google Play를 통해 클라우드에 유저 데이터를 백업할 수 있는 기능을 제공합니다.
**MainData** 경로 내의 파일들은 백업 대상이 되며, 보존되어야 하는 유저 데이터를 저장하는데 사용할 수 있습니다.
단, 클라우드 백업 가능한 데이터의 용량에는 제한이 있으므로, 전체 파일 크기에 신경을 써 주세요.

컴퍼넌트가 반환하는 경로는 C/C++ 라이브러리를 통해 접근 및 조작 가능한 실제 파일 시스템 상의 경로입니다.

Non-Portable Interface
----------------------
Android 플랫폼에서는 SD 카드 등의 용량이 큰 외부 저장 공간을 사용할 수 있는 경우가 있습니다.
외부 저장소 영역에 대한 접근 기능은 ``Leggiero::FileSystem::NPO::INonPortableOptimization_BundleFileResource`` 인터페이스를 통해 제공합니다.
