////////////////////////////////////////////////////////////////////////////////
// Tasks/ValueTasks.h (Leggiero/Modules - Task)
//
// Tasks to get a result value
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK__TASKS__VALUE_TASKS_H
#define __LM_TASK__TASKS__VALUE_TASKS_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>

// Leggiero.Task
#include "ITask.h"


namespace Leggiero
{
	namespace Task
	{
		// Interface for tasks to calculate a value asynchronously
		template <typename ValueT>
		class IAsyncValueTask
			: public ITask
		{
		public:
			// Result "Value" Type for the task
			using ValueType = ValueT;

		public:
			IAsyncValueTask(TaskCapabilityType capabilities = TaskCapabilities::kGeneral, TaskPriorityClass priority = TaskPriorityClass::kDefault, TaskPropertyType taskProperty = TaskPropertyType::kNone)
				: m_taskPriority(priority), m_taskProperty(taskProperty), m_taskCapabilities(capabilities)
			{ }
			virtual ~IAsyncValueTask() { }

		public:
			// Check whether the task have result value
			virtual bool HasValue() const = 0;

			// Get result value
			virtual ValueT GetValue() = 0;

		public:	// Task Properties
			virtual TaskPriorityClass	GetTaskPriority() { return m_taskPriority; }
			virtual TaskPropertyType	GetTaskProperty() { return m_taskProperty; }
			virtual TaskCapabilityType	GetRequiredCapabilties() { return m_taskCapabilities; }

		protected:
			TaskPriorityClass	m_taskPriority;
			TaskPropertyType	m_taskProperty;
			TaskCapabilityType	m_taskCapabilities;
		};


		// Asynchronous value calculation task based on function
		template <typename ValueT>
		class AsyncValueFunctionTask
			: public IAsyncValueTask<ValueT>
		{
		public:
			// Type for a calculation function
			using CalculationFuncType = std::function<ValueT()>;

		public:
			AsyncValueFunctionTask(CalculationFuncType calculation, TaskCapabilityType capabilities = TaskCapabilities::kGeneral, TaskPriorityClass priority = TaskPriorityClass::kDefault, TaskPropertyType taskProperty = TaskPropertyType::kNone)
				: IAsyncValueTask<ValueT>(capabilities, priority, taskProperty)
				, m_calculation(calculation)
			{ }
			virtual ~AsyncValueFunctionTask() { }

		public:	// ITask
			// Do Real Task Works
			virtual TaskDoneResult Do() override
			{
				if (m_calculation)
				{
					m_resultValue = m_calculation();
				}
				else
				{
					this->SetErrorFlag();
				}
				return TaskDoneResult(TaskDoneResult::ResultType::kFinished);
			}

		public:	//IAsyncValueTask
			// Check whether the task have result value
			virtual bool HasValue() const override
			{
				TaskState taskState = this->GetTaskState();
				return (Utility::SyntacticSugar::HasFlag(taskState, TaskState::kJobFinished)
					&& !Utility::SyntacticSugar::HasFlag(taskState, TaskState::kHasError));
			}

			// Get result value
			virtual ValueT GetValue() override { return m_resultValue; }

		protected:
			CalculationFuncType m_calculation;
			ValueT				m_resultValue;
		};


		// Chained Value Calculation Task
		template <typename InputValueT, typename ValueT>
		class DependentAsyncValueTask
			: public IAsyncValueTask<ValueT>
		{
		public:
			// Dependent prior task result type
			using InputValueType = InputValueT;

			// Type for a calculation function
			using CalculationFuncType = std::function<ValueT(InputValueT)>;

		public:
			DependentAsyncValueTask(CalculationFuncType calculation, std::shared_ptr<IAsyncValueTask<InputValueT> > inputTask, 
				TaskDoneResult waiting = TaskDoneResult(TaskDoneResult::ResultType::kSleep, std::chrono::milliseconds(16)),
				TaskCapabilityType capabilities = TaskCapabilities::kGeneral, TaskPriorityClass priority = TaskPriorityClass::kDefault, TaskPropertyType taskProperty = TaskPropertyType::kNone)
				: IAsyncValueTask<ValueT>(capabilities, priority, taskProperty)
				, m_calculation(calculation), m_inputTask(inputTask)
				, m_waitingResult(waiting)
			{ }
			virtual ~DependentAsyncValueTask() { }

		public:	// ITask
			// Do Real Task Works
			virtual TaskDoneResult Do() override
			{
				if (!m_inputTask)
				{
					this->SetErrorFlag();
					return TaskDoneResult(TaskDoneResult::ResultType::kFinished);
				}
				if (m_inputTask->HasValue())
				{
					m_resultValue = m_calculation(m_inputTask->GetValue());
					return TaskDoneResult(TaskDoneResult::ResultType::kFinished);
				}
				else
				{
					if (m_inputTask->HasError())
					{
						this->SetErrorFlag();
						return TaskDoneResult(TaskDoneResult::ResultType::kFinished);
					}
					else
					{
						if (m_waitingResult.result == TaskDoneResult::ResultType::kFinished)
						{
							m_waitingResult.result = TaskDoneResult::ResultType::kYield;
						}
						return m_waitingResult;
					}
				}
			}

		public:	//IAsyncValueTask
			// Check whether the task have result value
			virtual bool HasValue() const override
			{
				TaskState taskState = this->GetTaskState();
				return (Utility::SyntacticSugar::HasFlag(taskState, TaskState::kJobFinished)
					&& !Utility::SyntacticSugar::HasFlag(taskState, TaskState::kHasError));
			}

			// Get result value
			virtual ValueT GetValue() override { return m_resultValue; }

		protected:
			CalculationFuncType								m_calculation;
			ValueT											m_resultValue;
			std::shared_ptr<IAsyncValueTask<InputValueT> >	m_inputTask;
			TaskDoneResult									m_waitingResult;
		};
	}
}

#endif
