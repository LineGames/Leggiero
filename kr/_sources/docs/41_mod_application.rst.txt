==================
Application Module
==================

Application 모듈은 게임 어플리케이션 자체 및 어플리케이션이 구동되는 플랫폼과의 상호작용에 대한 내용을 추상화합니다.


Module Common Information
=========================

**Project:** *Modules/Application*

**Namespace:** ``Leggiero::Application``

Module Interfaces
-----------------
(없음)

Components
----------
- `Application Component`_


Application Component
=====================
게임의 어플리케이션과 플랫폼을 추상화하는 컴퍼넌트입니다.

**Type:** ``Leggiero::Application::ApplicationComponent``

**Component Id:** ``EngineComponentIdType::kApplication`` (1)

Dependencies
------------
(없음)

*엔진에 의해 직접 처리됨*

Overview
--------
컴퍼넌트 클래스 자체가 너무 지저분해지는 것을 막기 위해, 컴퍼넌트의 기능은 하위 인터페이스들로 나뉘어 제공됩니다.
하위 인터페이스에 대한 참조를 Application 컴퍼넌트에서 얻을 수 있습니다.

Information Provider
--------------------
AppInformation
  게임 어플리케이션에 대한 정보를 제공합니다.

Device Information
  게임 어플리케이션이 구동되는 디바이스에 대한 정보를 제공합니다.

Control
-------
PlatformAppControl
  어플리케이션이 구동되는 플랫폼 상에서의 게임 어플리케이션의 동작을 제어하기 위한 인터페이스입니다.

Event Center
------------
게임 어플리케이션의 이벤트를 통지받기 위한 인터페이스를 제공합니다.

SystemEventCenter
  게임 어플리케이션에서 발생하는 시스템 이벤트에 대한 옵저브 인터페이스를 제공합니다.

RawTouchEventCenter
  게임 어플리케이션에 입력되는 터치 이벤트에 대한 옵저브 인터페이스를 제공합니다.

Observe Interface
+++++++++++++++++
*SystemEventObserver.h* 나 *RawTouchEventObserver.h* 등에 정의된 옵저버 인터페이스를 상속받아 옵저버를 구현할 수 있습니다.

옵저버 객체의 raw pointer를 event center에서 제공하는 dispatcher에 등록하여 이벤트를 수신할 수 있습니다.
옵저버 객체 등록은 생명 주기에 영향을 미치지 않고 이벤트 통지 시 별도의 유효성 검사를 수행하지 않으므로, 등록된 옵저버 객체는 수명이 끝나기 전에 ``UnRegister~`` 종류의 함수를 이용해 이벤트 수신 등록을 해제해야 합니다.

Services
--------
어플리케이션이 구동되는 플랫폼의 기능을 제공하기 위한, 별도 단위 인터페이스입니다.

TextInputController
  사용자로부터 텍스트 입력을 받는 기능을 제공합니다. Leggiero의 주 대상 게임은, 텍스트 입력이나 네이티브 UI를 기반으로 한 게임이 아니므로, 현재는 팝업 창을 띄워 단순한 텍스트 입력을 받는 기능만을 제공합니다.

WebViewController
  플랫폼의 URI 혹은 URL 호출을 수행하거나, 지정 URL에 대한 단순한 웹 뷰를 열 수 있는 기능을 제공합니다.

.. Note:: **OpenURL과 OpenURLByPage의 차이**
    
    ``OpenURL`` 은 플랫폼에 URI 처리를 직접 요청합니다. 해당 scheme의 URI를 핸들링하는 타 어플리케이션을 호출하는 식으로 사용할 수 있습니다. 한편, ``OpenURLByPage`` 는 Leggiero 어플리케이션 내에서 해당 URL의 내용을 표시하는 단순한 웹 뷰를 표시합니다.
    
    단, 개발용 Windows PC 플랫폼에서는, 내장 웹 뷰를 사용하지 않아, ``OpenURLByPage`` 호출 시에도 URL에 대한 열기 요청이 OS에 전달되어 기본 웹 브라우저가 열립니다.
    
