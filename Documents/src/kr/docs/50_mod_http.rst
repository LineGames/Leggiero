===========
HTTP Module
===========

HTTP 모듈은 HTTP를 통한 동기 및 비동기 통신 기능을 제공합니다.


Module Common Information
=========================

**Project:** *Modules/HTTP*

**Namespace:** ``Leggiero::HTTP``

Module Interfaces
-----------------
- `HTTP Module Interface`_

Components
----------
- `Async HTTP Component`_


HTTP Module Interface
=====================
HTTP 모듈에 대한 인터페이스를 제공합니다.

**Type:** ``Leggiero::HTTP::HttpModule``

**Module Id:** ``EngineModuleIdType::kHTTP`` (51)

Dependencies
------------
(없음)

Overview
--------
HTTP 요청을 위한 모듈의 내부 상태를 초기화하기 위한 모듈 인터페이스입니다. cURL 초기화 등을 수행합니다.

Do HTTP Request
---------------
*HttpRequest.h* 헤더 파일의 함수들을 이용해 동기 HTTP(S) 요청을 수행할 수 있습니다.


Async HTTP Component
====================
작업 기반 비동기 방식의 HTTP 요청을 처리하기 위한 컴퍼넌트입니다.

**Type:** ``Leggiero::HTTP::AsyncTaskHttpComponent``

**Component Id:** ``EngineComponentIdType::kAsyncTaskHTTP`` (52)

Dependencies
------------
- `HTTP Module <HTTP Module Interface>`_
- `Task Manager Component <47_mod_task.html>`_ (except iOS Platform)

Overview
--------
비동기 방식으로 HTTP 요청을 수행하고, 요청을 처리하는 비동기 작업을 반환합니다.

Android와 Windows PC 플랫폼에서는 Task Manager를 이용하여 비동기 HTTP 요청을 수행하나, iOS에서는 Grand Central Dispatch를 직접 이용하여 비동기 요청을 수행하게 됩니다.
