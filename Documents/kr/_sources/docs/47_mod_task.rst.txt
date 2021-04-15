===========
Task Module
===========

Task 모듈은 작업 기반 비동기 프로그래밍을 위한 간단한 인프라를 제공합니다.


Module Common Information
=========================

**Project:** *Modules/Task*

**Namespace:** ``Leggiero::Task``

Module Interfaces
-----------------
(없음)

Components
----------
- `Task Manager Component`_


Task Manager Component
======================
비동기 작업의 수행을 관리하는 컴퍼넌트입니다.

**Type:** ``Leggiero::Task::TaskManagerComponent``

**Component Id:** ``EngineComponentIdType::kTaskManager`` (21)

Dependencies
------------
(없음)

Overview
--------
비동기 작업의 수행을 관리합니다.

작업의 상세 내용에 대한 정의는 ``Leggiero::Task::ITask`` 인터페이스를 가지는 개별 작업 객체에서 대부분 이뤄집니다.
작업 관리자 컴퍼넌트에 작업을 수행하게 하는 인터페이스는 간단합니다:

.. code-block:: C++
    :caption: part of TaskManagerComponent.h
    
    virtual bool ExecuteTask(std::shared_ptr<ITask> task) = 0;
    

.. Note:: Leggiero의 비동기 작업 ``ITask`` 는 코루틴이 아닙니다. 
    
    ``TaskDoneResult Do();`` 등의 작업 수행 메서드에서 *Yield* 결과를 반환했다면, 다음 작업의 수행은 해당 반환 지점에서 코드의 수행을 계속하는 것이 아니라, 작업 수행 메서드가 다시 호출되는 방식으로 이루어집니다.
    비동기 작업 수행의 일시적인 중단에서 기존 상태의 저장이 필요하다면, 작업 객체의 데이터 등을 이용해 직접 상태의 저장과 작업 재개 로직을 구현해야 합니다.
    

Task Sub-System
---------------
*TaskSubSystemInterface.h* 에 정의된 ``Leggiero::Task::ITaskSubSystem`` 인터페이스를 상속 받는 작업 서브시스템을 Task Manager Component에 추가하여, 작업 관리자의 기능을 확장시킬 수 있습니다.

Graphic Task Sub-System
-----------------------
OpenGL ES Context를 가지는 그래픽 스레드에서 처리되어야 하는 작업들을 처리하기 위한 작업 서브시스템입니다.

이 서브시스템은 아래와 같은 의존성을 추가합니다:

Dependencies
------------
- `Graphics Module <43_mod_graphics.html>`_

텍스처의 로딩, 셰이더의 컴파일 등은 그래픽 스레드에서 처리되어야 하며, 따라서 그래픽 자원의 비동기 로딩을 위해서는 Graphic Task Sub-System에 의한 비동기 작업 관리자 기능의 확장이 필요합니다.
LegToy 예제 프로젝트 및 Project Creator 툴을 통해 생성한 새 프로젝트에는 Graphic Task Sub-System을 적용하는 아래와 같은 코드가 포함되어 있습니다:

.. code-block:: C++
    :caption: part of CreateGame.cpp
    
    #include <Task/TaskManagerComponent.h>
    #include <Task/GraphicTask/GraphicTaskSystem.h>
    
    ...
    
    /* in Game::_RegisterUsingComponents() */
    
    Leggiero::Task::TaskManagerComponent *taskManager = Leggiero::Task::TaskManagerComponent::CreateComponentObject();
    Leggiero::Task::GraphicTask::EnableGraphicTaskSystem(taskManager);
    _RegisterEngineComponent(taskManager);
    
