////////////////////////////////////////////////////////////////////////////////
// Sugar/SingletonPattern.h (Leggiero - Utility)
//
// GoF Singleton Pattern - referenced EC++ implementation
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__SUGAR__SINGLETON_PATTERN_H
#define __UTILITY__SUGAR__SINGLETON_PATTERN_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <cstdlib>
#include <atomic>

// External Library
#include <pthread.h>

// Leggiero.Utility
#include "Finally.h"


namespace Leggiero
{
	namespace Utility
	{
		namespace DesignPattern
		{
			// Singleton Base Class
			template <class T>
			class Singleton
			{
			public:
				virtual ~Singleton()
				{
					// To raise AV after deletion
					ms_instance = nullptr;
				}

			protected:
				virtual void Initialize() { }

			private:
				static T *ms_instance;

				static void SingletonDeleter()
				{
					delete ms_instance;
				}

			public:
				static T &GetInstance()
				{
#ifdef _LEGGIERO_IOS
					static pthread_mutex_t		initMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#else
                    static pthread_mutex_t        initMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#endif
					static std::atomic_uchar	initializedFlag;

					unsigned char isInitialized = initializedFlag.load();
					if (isInitialized == 0)
					{
						int lockResult = pthread_mutex_lock(&initMutex);
						if (lockResult == 0)
						{
							pthread_mutex_t *lockCopy = &initMutex;
							auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
							FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

							isInitialized = initializedFlag.exchange(1);
							if (isInitialized == 0)
							{
								ms_instance = new T();
								ms_instance->Initialize();
								std::atexit(SingletonDeleter);
							}
						}
						else
						{
							// Keep going with atomic
							// This make sure to initialize once, but access before the initialization can be occured

							isInitialized = initializedFlag.exchange(1);
							if (isInitialized == 0)
							{
								ms_instance = new T();
								ms_instance->Initialize();
								std::atexit(SingletonDeleter);
							}
						}
					}

					return *ms_instance;

					//NOTE: We need to consider Longevity for finalization order
				}
			};
		}
	}
}


//------------------------------------------------------------------------------ Singleton Macros
#define LEGGIERO_MAKE_SINGLETON_UNIQUE(CLASS_NAME) \
private: \
	CLASS_NAME() { } \
	CLASS_NAME(const CLASS_NAME& other) {} \
	CLASS_NAME& operator=(const CLASS_NAME& rhs) { return *this; } \
	friend class Leggiero::Utility::DesignPattern::Singleton<CLASS_NAME>;

#define LEGGIERO_DECLARE_SINGLETON_MEMBERS(CLASS_NAME) \
namespace Leggiero \
{ \
	namespace Utility \
	{ \
		namespace DesignPattern \
		{ \
			template<> \
			CLASS_NAME *Singleton<CLASS_NAME>::ms_instance; \
		} \
	} \
}

#define LEGGIERO_DEFINE_SINGLETON_MEMBERS(CLASS_NAME) \
namespace Leggiero \
{ \
	namespace Utility \
	{ \
		namespace DesignPattern \
		{ \
			template<> \
			CLASS_NAME *Singleton<CLASS_NAME>::ms_instance = nullptr; \
		} \
	} \
}

#endif
