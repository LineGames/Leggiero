cmake_minimum_required(VERSION 3.18.1)

include("../../Platform/Platform.Android/cmake/AndroidCommon.cmake")


add_library(LE_M_Task STATIC)
le_tgt_setCppOptions(LE_M_Task)


target_include_directories(LE_M_Task PUBLIC ..)

target_link_libraries(LE_M_Task
    PRIVATE
        LE_Libraries
        LE_Basic LE_Utility LE_Engine
        LE_PlatformA
        LE_M_Graphics
)


target_sources(LE_M_Task
    PUBLIC
        TaskTypes.h TaskManagerComponent.h TaskSubSystemInterface.h
        Tasks/ITask.h Tasks/DependentTask.h Tasks/SingleActionTask.h Tasks/ValueTasks.h
        Processor/ITaskProcessor.h Processor/IThreadWorkerContext.h Processor/ThreadWorker.h Processor/ThreadWorkerPool.h
        GraphicTask/GraphicTaskSystem.h GraphicTask/GraphicThreadWorker.h GraphicTask/GraphicThreadWorkerPool.h
        
    PRIVATE
        TaskManagerComponent.cpp ConcreteTaskManager.cpp
        Tasks/DependentTask.cpp Tasks/SingleActionTask.cpp
        Processor/ThreadWorker.cpp Processor/ThreadWorkerPool.cpp
        _Internal/ITaskManagerSystemFunctions.h _Internal/_ConcreteTaskManager.h _Internal/_TaskExecutionEntry.h
        Platform/TaskPlatform_Android.cpp
        GraphicTask/GraphicTaskSystem.cpp GraphicTask/GraphicThreadWorker.cpp GraphicTask/GraphicThreadWorkerPool.cpp
)
