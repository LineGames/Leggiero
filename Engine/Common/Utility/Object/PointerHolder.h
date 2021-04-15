////////////////////////////////////////////////////////////////////////////////
// Object/PointerHolder.h (Leggiero - Utility)
//
// Safely Sharable Pointer Holder
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__OBJECT__POINTER_HOLDER_H
#define __UTILITY__OBJECT__POINTER_HOLDER_H


// Standard Library
#include <memory>

// Leggiero.Utility
#include "../Threading/ManagedThreadPrimitives.h"


namespace Leggiero
{
	namespace Utility
	{
		namespace Object
		{
			// Pointer Holder
			class PointerHolder
			{
			public:
				PointerHolder(void *targetPtr);

				void NotifyTargetDeleted();

			public:
				class IHoldContext
				{
				public:
					virtual ~IHoldContext() { }

					virtual void *GetObject() = 0;
					
					template <typename ObjectT>
					ObjectT *GetTypedObject()
					{
						void *targetPointer = GetObject();
						if (targetPointer == nullptr)
						{
							return nullptr;
						}
						return static_cast<ObjectT *>(targetPointer);
					}
				};

				std::unique_ptr<IHoldContext> LockTarget();

			protected:
				Threading::SafePthreadRWLock m_targetLock;
				void *m_targetPtr;
			};
		}
	}
}

#endif
