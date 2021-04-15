////////////////////////////////////////////////////////////////////////////////
// Object/VerySimpleObjectPool.h (Leggiero - Utility)
//
// Very Simple Object Pools
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__OBJECT__VERY_SIMPLE_OBJECT_POOL_H
#define __UTILITY__OBJECT__VERY_SIMPLE_OBJECT_POOL_H


// Standard Library
#include <memory>

// External Library
#include <concurrentqueue/concurrentqueue.h>


namespace Leggiero
{
	namespace Utility
	{
		namespace Object
		{
			// Very Simple Object Cache Pool
			template <typename ObjectT>
			class VerySimpleObjectPool
			{
			public:
				using ObjectType = ObjectT;
				using ObjectHandleType = std::shared_ptr<ObjectT>;

			public:
				ObjectHandleType GetObject()
				{
					std::shared_ptr<ObjectT> pooledObject;
					if (m_pool.try_dequeue(pooledObject))
					{
						return pooledObject;
					}
					return std::make_shared<ObjectT>();
				}

				void ReleaseObject(ObjectHandleType obj)
				{
					m_pool.enqueue(obj);
				}

				void Clear()
				{
					std::shared_ptr<ObjectT> pooledObject;
					while (m_pool.try_dequeue(pooledObject))
					{
					}
				}

			protected:
				moodycamel::ConcurrentQueue<std::shared_ptr<ObjectT> > m_pool;
			};


			// Very Simple Object Cache Pool without Object Creation
			// Used for objects needs manual creation
			template <typename ObjectT>
			class VerySimpleNonCreatingObjectPool
			{
			public:
				using ObjectType = ObjectT;
				using ObjectHandleType = std::shared_ptr<ObjectT>;

			public:
				ObjectHandleType GetObject()
				{
					std::shared_ptr<ObjectT> pooledObject;
					if (m_pool.try_dequeue(pooledObject))
					{
						return pooledObject;
					}
					return nullptr;
				}

				void ReleaseObject(ObjectHandleType obj)
				{
					m_pool.enqueue(obj);
				}

				void Clear()
				{
					std::shared_ptr<ObjectT> pooledObject;
					while (m_pool.try_dequeue(pooledObject))
					{
					}
				}

			protected:
				moodycamel::ConcurrentQueue<std::shared_ptr<ObjectT> > m_pool;
			};
		}
	}
}

#endif
