============
Input Module
============

Input 모듈은 게임 어플리케이션에 대한 유저의 입력을 처리합니다.


Module Common Information
=========================

**Project:** *Modules/Input*

**Namespace:** ``Leggiero::Input``

Module Interfaces
-----------------
(없음)

Components
----------
- `Touch Input Component`_


Touch Input Component
=====================
게임에 대한 터치 입력을 처리할 수 있는 컴퍼넌트입니다.

**Type:** ``Leggiero::Input::TouchInputComponent``

**Component Id:** ``EngineComponentIdType::kTouchInput`` (101)

Dependencies
------------
- `Application Component <41_mod_application.html>`_

Overview
--------
어플리케이션으로 입력되는 raw touch events를 처리하여, 게임에서 처리하기 적절한 형태로 가공된 데이터를 제공합니다.

각 터치에 대한 상태를 추적하며, 터치 입력을 이벤트 형태로 즉각 수신할 수 있는 옵저버 인터페이스를 제공합니다.

Event Based Touch Context
-------------------------
매 게임 프레임에 사용할 수 있는 큐잉 된 이벤트의 형태로 터치 정보를 제공하기 위한 컨텍스트 구현입니다.
Touch Input Component와 함께 사용할 수 있습니다.

*Touch/EventBasedTouchContext.h* 헤더 파일에 정의되어 있으며, 게임 프로그램에서 생성하여 사용할 수 있습니다.
