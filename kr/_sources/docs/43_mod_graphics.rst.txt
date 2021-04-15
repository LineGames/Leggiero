===============
Graphics Module
===============

Graphics 모듈은 OpenGL ES 시스템에 대한 초기화 및 관리를 수행하며, OpenGL ES 그래픽스 관련 편의 기능들을 제공합니다.


Module Common Information
=========================

**Project:** *Modules/Graphics*

**Namespace:** ``Leggiero::Graphics``

Module Interfaces
-----------------
- `Graphics Module Interface`_

Components
----------
- `Graphic Resource Manager Component`_


Graphics Module Interface
=========================
그래픽스 모듈에 대한 인터페이스를 제공합니다.

**Type:** ``Leggiero::Graphics::GraphicsModule``

**Module Id:** ``EngineModuleIdType::kGraphics`` (200)

Dependencies
------------
(없음)

Overview
--------
어플리케이션 플랫폼에서 그래픽 API를 정상적으로 이용하기 위해 수행해야 하는 처리들이 존재합니다.
그래픽스 모듈은 이런 작업을 게임 프로그래머에게 숨기고, 대신 수행해 줄 수 있습니다.

.. Caution:: 그래픽스 모듈은 텍스처나 셰이더 등의 OpenGL ES 그래픽 자원에 대한 유틸리티 API를 노출하고 있습니다.
    이런 API를 직접 사용하셔도 상관은 없지만, 무언가 특별한 의도를 가지고 OpenGL ES 컨텍스트의 변화에 따른 처리 등을 직접 수행하며 자원들을 다루시기를 원하는 것이 아니라면, `Graphic Resource Manager Component`_ 에게 자원의 생성과 관리를 위임하시기를 권장합니다.


Graphic Resource Manager Component
==================================
OpenGL ES 그래픽 자원을 관리하는 컴퍼넌트입니다.

**Type:** ``Leggiero::Graphics::GraphicResourceManagerComponent``

**Component Id:** ``EngineComponentIdType::kGraphicResourceManager`` (211)

Dependencies
------------
- `Application Component <41_mod_application.html>`_
- `Bundle File Resource Component <42_mod_filesystem.html>`_ *(Optional)*

Overview
--------
OpenGL ES 자원들을 생성하고, 생명주기 내에서의 관리를 수행하는 컴퍼넌트입니다.
앱이 백그라운드 상태로 전환되는 등의 이유로 OpenGL ES 컨텍스트에 변화가 생겼을 때, 각 자원의 유효성을 체크하고 필요하다면 업데이트를 수행합니다.

텍스처, 셰이더의 생성은 이 컴퍼넌트를 통해 수행하시면 됩니다.
``GetTextureManager`` , ``GetRuntimeTextureAtlasManager`` , ``GetProgramManager`` 메서드를 통해, 자원을 생성할 수 있는 개별 관리자 객체에 접근할 수 있습니다.
