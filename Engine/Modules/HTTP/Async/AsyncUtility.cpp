////////////////////////////////////////////////////////////////////////////////
// Async/AsyncUtility.cpp (Leggiero/Modules - HTTP)
//
// Async Common Utility Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "AsyncUtility.h"


namespace Leggiero
{
	namespace HTTP
	{
		namespace Async
		{
			namespace Settings
			{
				//------------------------------------------------------------------------------
				namespace _Internal
				{
					static Task::TaskCapabilityType kHTTPTaskCapability = Task::TaskCapabilities::kGeneral;
				}

				//------------------------------------------------------------------------------
				Task::TaskCapabilityType GetHTTPTaskCapability()
				{
					return _Internal::kHTTPTaskCapability;
				}

				//------------------------------------------------------------------------------
				void SetHTTPTaskCapability(Task::TaskCapabilityType capability)
				{
					_Internal::kHTTPTaskCapability = capability;
				}
			}
		}
	}
}
