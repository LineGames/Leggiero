////////////////////////////////////////////////////////////////////////////////
// GraphicTass/GraphicTaskSystem.h (Leggiero/Modules - Task)
//
// Graphic Task Sub-System Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK__GRAPHIC_TASK__GRAPHIC_TASK_SYSTEM_H
#define __LM_TASK__GRAPHIC_TASK__GRAPHIC_TASK_SYSTEM_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>


namespace Leggiero
{
	namespace Task
	{
		// Forward Declaration
		class TaskManagerComponent;


		namespace GraphicTask
		{
			// Enable Graphic Task System for the task manager
			void EnableGraphicTaskSystem(TaskManagerComponent *taskManager, int graphicThreadCount = 4);
		}
	}
}

#endif
