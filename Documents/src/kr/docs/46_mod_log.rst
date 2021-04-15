==========
Log Module
==========

Log 모듈은 로깅 기능을 지원합니다.


Module Common Information
=========================

**Project:** *Modules/Log*

**Namespace:** ``Leggiero::Log``

Module Interfaces
-----------------
- `Log Module Interface`_

Components
----------
(없음)


Logger and Log Writer
=====================
**Logger** 는 코드에서 로그를 받아 처리할 수 있는 클래스입니다. Format string으로 입력되는 로그를 처리할 수 있는 기능을 제공합니다.

**Log Writer** 는 각각의 로그 엔트리를 어딘가에 기록할 수 있는 클래스의 인터페이스입니다.
기록 대상은 파일이나 디버그용 플랫폼 로그 스트림, 심지어는 네트워크 상의 어떤 HTTP 호출이 될 수도 있으며, *ILogWriter.h* 에 정의된 ``Leggier::Log::ILogWriter`` 인터페이스, 또는 ``ThreadedLogWriter`` 등의 log writer를 서브클래싱 하여 원하는 대상에 기록을 수행하는 log writer를 구현할 수 있습니다.

Logger에 원하는 Log Writer들을 등록할 수 있습니다.
해당 logger에 로그를 기록하면, writer에 의해 로그를 남길 대상에 기록이 수행됩니다.


Log Module Interface
====================
로그 모듈에 대한 인터페이스를 제공합니다.

**Type:** ``Leggiero::Log::LogModule``

**Module Id:** ``EngineModuleIdType::kLog`` (8)

Dependencies
------------
(없음)

Overview
--------
로그 모듈에서는 로그 기록을 도와주는 유틸리티 클래스들과 디버그를 위한 공용 로거를 제공합니다.

Debug Logger
------------
*DebugLogger.h* 헤더 파일에서 디버그를 위한 공용 로거를 제공합니다.

디버그 로거에 입력되는 로그는 각 플랫폼별로 디버그 목적으로 사용되는 로그 스트림에 출력됩니다; iOS에서는 ``NSLog`` 와 같이 동작하며, Android에서는 일반 Log 출력처럼 동작하여 Logcat 등에서 확인하실 수 있습니다. Windows PC에서는 WinAPI를 통해 디버그 출력으로 나가, DbgView 등의 툴이나 Visual Studio의 명령/직접 실행 창 등에서 확인하실 수 있습니다.

아래와 같은 식으로, 디버그 로그 객체를 얻어와 (로그 모듈이 초기화 된 시점 이후에라면) 언제든지 디버그를 위한 로그를 남길 수 있습니다:

.. code-block:: C++
    :caption: example for debug logging
    
    #include <Log/DebugLogger.h>
    
    ...
    
    Leggiero::Log::GetDebugLogger().LogPrintf(Leggiero::Log::LogLevelType::kDebug, "Hello, HAPPY World! %d", 283);
    

디버그 로거는 Debug 빌드 구성에서만 동작합니다. Release 구성에서는 컴파일 단계에서 더미 객체로 대체되어, 오버헤드를 최소화합니다.

.. Note:: Leggiero 엔진 내부의 처리 상태나 예외 등의 내용도 적절한 logger를 통해 기록된다면 좋겠습니다만, 해당 기능은 구현되어 있지 않습니다.
