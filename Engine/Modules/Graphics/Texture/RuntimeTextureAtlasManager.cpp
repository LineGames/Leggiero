////////////////////////////////////////////////////////////////////////////////
// Texture/RuntimeTextureAtlasManager.cpp (Leggiero/Modules - Graphics)
//
// Implementation of Runtime Texture Atlas Manager
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "RuntimeTextureAtlasManager.h"

// Leggiero.Utility
#include <Utility/Math/SimpleMath.h>
#include <Utility/Sugar/Finally.h>

// Leggiero.Graphics
#include "../GraphicResourceManagerComponent.h"
#include "TextureManager.h"
#include "DynamicTextureResource.h"
#include "RuntimeTextureAtlas.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// Internal Utility

		namespace _Internal
		{
			constexpr GLsizei _kDefaultRuntimeTextureAtlasSize = 2048;
			constexpr GLsizei _kDefaultRuntimeTextureAtlasPadding = 1;

			//------------------------------------------------------------------------------
			class RuntimeTextureAtlasGroupData
			{
			public:
				GLsizei padding;
				GLsizei atlasTextureWidth;
				GLsizei atlasTextureHeight;

				std::vector<std::shared_ptr<RuntimeTextureAtlas> > atlasList;
				pthread_rwlock_t atlasListLock;

			public:
				RuntimeTextureAtlasGroupData(GLsizei padding, GLsizei textureWidth, GLsizei textureHeight)
					: padding(padding), atlasTextureWidth(textureWidth), atlasTextureHeight(textureHeight)
				{
					pthread_rwlock_init(&atlasListLock, NULL);
				}

				virtual ~RuntimeTextureAtlasGroupData()
				{
					pthread_rwlock_destroy(&atlasListLock);
				}
			};
		}


		//////////////////////////////////////////////////////////////////////////////// RuntimeTextureAtlasManager

		//------------------------------------------------------------------------------
		const RuntimeTextureAtlasManager::AtlasGroupIdType RuntimeTextureAtlasManager::kDefaultAtlasGroup = 0;

		//------------------------------------------------------------------------------
		RuntimeTextureAtlasManager::RuntimeTextureAtlasManager(GraphicResourceManagerComponent *ownerManager)
			: m_ownerResourceManager(ownerManager)
		{
			pthread_rwlock_init(&m_groupTableLock, NULL);

			m_atlasGroupTable.insert(std::pair<AtlasGroupIdType, std::shared_ptr<_Internal::RuntimeTextureAtlasGroupData> >(kDefaultAtlasGroup, std::make_shared<_Internal::RuntimeTextureAtlasGroupData>(_Internal::_kDefaultRuntimeTextureAtlasPadding, _Internal::_kDefaultRuntimeTextureAtlasSize, _Internal::_kDefaultRuntimeTextureAtlasSize)));
		}

		//------------------------------------------------------------------------------
		RuntimeTextureAtlasManager::~RuntimeTextureAtlasManager()
		{
			pthread_rwlock_destroy(&m_groupTableLock);
		}

		//------------------------------------------------------------------------------
		RuntimeTextureAtlasManager::AtlasGroupIdType RuntimeTextureAtlasManager::RegisterTextureAtlasGroup(GLsizei padding, GLsizei textureWidth, GLsizei textureHeight)
		{
			if (textureWidth <= 0 || textureHeight < 0)
			{
				return kDefaultAtlasGroup;
			}
			if (textureHeight == 0)
			{
				textureHeight = textureWidth;
			}
			textureWidth = Utility::Math::SmallestGreaterEqualPow2(textureWidth);
			textureHeight = Utility::Math::SmallestGreaterEqualPow2(textureHeight);

			AtlasGroupIdType issuedGroupId = m_issuingGroupId.fetch_add(1);

			int writeLockResult = pthread_rwlock_wrlock(&m_groupTableLock);
			if (writeLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_groupTableLock;
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_WRITE_LOCK, releaseLockFunc);

				m_atlasGroupTable.insert(std::pair<AtlasGroupIdType, std::shared_ptr<_Internal::RuntimeTextureAtlasGroupData> >(issuedGroupId, std::make_shared<_Internal::RuntimeTextureAtlasGroupData>(padding, textureWidth, textureHeight)));
				return issuedGroupId;
			}
			else
			{
				// Cannot Register Group
				return kDefaultAtlasGroup;
			}
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<RuntimeTextureAtlasEntry> RuntimeTextureAtlasManager::AllocateSubTexture(RuntimeTextureAtlasManager::AtlasGroupIdType atlasGroup, GLsizei width, GLsizei height, RuntimeTextureAtlasEntry::RestorerType restorer)
		{
			std::shared_ptr<_Internal::RuntimeTextureAtlasGroupData> targetGroup;

			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_groupTableLock)) == EAGAIN)
			{
				sched_yield();
			}
			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_groupTableLock;
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				std::map<AtlasGroupIdType, std::shared_ptr<_Internal::RuntimeTextureAtlasGroupData> >::iterator groupFindIt = m_atlasGroupTable.find(atlasGroup);
				if (groupFindIt != m_atlasGroupTable.end())
				{
					targetGroup = groupFindIt->second;
				}
			}
			else
			{
				// Something Wrong
				return nullptr;
			}

			if (!targetGroup)
			{
				// Cannot Find Target Group
				return nullptr;
			}

			std::vector<std::shared_ptr<RuntimeTextureAtlas> > existingAtlas;
			existingAtlas.reserve(targetGroup->atlasList.size() + 1);
			while ((readLockResult = pthread_rwlock_rdlock(&(targetGroup->atlasListLock))) == EAGAIN)
			{
				sched_yield();
			}
			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &(targetGroup->atlasListLock);
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				existingAtlas.assign(targetGroup->atlasList.begin(), targetGroup->atlasList.end());
			}
			else
			{
				// Cannot Hold Lock for Atlas Group
				return nullptr;
			}

			for (const std::shared_ptr<RuntimeTextureAtlas> &currentAtlas : existingAtlas)
			{
				std::shared_ptr<RuntimeTextureAtlasEntry> registeredEntry = currentAtlas->AllocateSubTexture(width, height, restorer);
				if (registeredEntry)
				{
					return registeredEntry;
				}
			}

			// Create New Atlas
			GLint maxTextureSize;
			glEnable(GL_TEXTURE_2D);
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
			if ((width + targetGroup->padding > maxTextureSize) || (height + targetGroup->padding > maxTextureSize))
			{
				// Too Big SubTexture
				return nullptr;
			}
			if (targetGroup->atlasTextureWidth > maxTextureSize)
			{
				targetGroup->atlasTextureWidth = maxTextureSize;
			}
			if (targetGroup->atlasTextureHeight > maxTextureSize)
			{
				targetGroup->atlasTextureHeight = maxTextureSize;
			}
			GLsizei effectiveWidth = targetGroup->atlasTextureWidth;
			GLsizei effectiveHeight = targetGroup->atlasTextureHeight;
			if (effectiveWidth < width + targetGroup->padding)
			{
				effectiveWidth = Utility::Math::SmallestGreaterEqualPow2(width + targetGroup->padding);
			}
			if (effectiveHeight < height + targetGroup->padding)
			{
				effectiveHeight = Utility::Math::SmallestGreaterEqualPow2(height + targetGroup->padding);
			}

			std::shared_ptr<DynamicTextureResource> atlasTexture = m_ownerResourceManager->GetTextureManager().CreateDynamicTexture(effectiveWidth, effectiveHeight);
			if (!atlasTexture)
			{
				// Cannot Create Texture
				return nullptr;
			}

			std::shared_ptr<RuntimeTextureAtlas> createdAtlas(std::make_shared<RuntimeTextureAtlas>(effectiveWidth, effectiveHeight, atlasTexture, targetGroup->padding));
			std::shared_ptr<RuntimeTextureAtlasEntry> createdEntry = createdAtlas->AllocateSubTexture(width, height, restorer);
			if (!createdEntry)
			{
				// Atlas Error???
				return nullptr;
			}

			int writeLockResult = pthread_rwlock_wrlock(&m_groupTableLock);
			if (writeLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_groupTableLock;
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_WRITE_LOCK, releaseLockFunc);

				targetGroup->atlasList.push_back(createdAtlas);
			}
			else
			{
				// Cannot Update Group Data
				return nullptr;
			}

			return createdEntry;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<RuntimeTextureAtlasEntry> RuntimeTextureAtlasManager::InsertSubTexture(RuntimeTextureAtlasManager::AtlasGroupIdType atlasGroup, const void *imageData, GLsizei width, GLsizei height, GLenum format, GLenum dataType, RuntimeTextureAtlasEntry::RestorerType restorer)
		{
			std::shared_ptr<_Internal::RuntimeTextureAtlasGroupData> targetGroup;

			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_groupTableLock)) == EAGAIN)
			{
				sched_yield();
			}
			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_groupTableLock;
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				std::map<AtlasGroupIdType, std::shared_ptr<_Internal::RuntimeTextureAtlasGroupData> >::iterator groupFindIt = m_atlasGroupTable.find(atlasGroup);
				if (groupFindIt != m_atlasGroupTable.end())
				{
					targetGroup = groupFindIt->second;
				}
			}
			else
			{
				// Something Wrong
				return nullptr;
			}

			if (!targetGroup)
			{
				// Cannot Find Target Group
				return nullptr;
			}

			std::vector<std::shared_ptr<RuntimeTextureAtlas> > existingAtlas;
			existingAtlas.reserve(targetGroup->atlasList.size() + 1);
			while ((readLockResult = pthread_rwlock_rdlock(&(targetGroup->atlasListLock))) == EAGAIN)
			{
				sched_yield();
			}
			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &(targetGroup->atlasListLock);
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				existingAtlas.assign(targetGroup->atlasList.begin(), targetGroup->atlasList.end());
			}
			else
			{
				// Cannot Hold Lock for Atlas Group
				return nullptr;
			}

			for (const std::shared_ptr<RuntimeTextureAtlas> &currentAtlas : existingAtlas)
			{
				std::shared_ptr<RuntimeTextureAtlasEntry> registeredEntry = currentAtlas->InsertSubTexture(imageData, width, height, format, dataType, restorer);
				if (registeredEntry)
				{
					return registeredEntry;
				}
			}

			// Create New Atlas
			GLint maxTextureSize;
			glEnable(GL_TEXTURE_2D);
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
			if ((width + targetGroup->padding > maxTextureSize) || (height + targetGroup->padding > maxTextureSize))
			{
				// Too Big SubTexture
				return nullptr;
			}
			if (targetGroup->atlasTextureWidth > maxTextureSize)
			{
				targetGroup->atlasTextureWidth = maxTextureSize;
			}
			if (targetGroup->atlasTextureHeight > maxTextureSize)
			{
				targetGroup->atlasTextureHeight = maxTextureSize;
			}
			GLsizei effectiveWidth = targetGroup->atlasTextureWidth;
			GLsizei effectiveHeight = targetGroup->atlasTextureHeight;
			if (effectiveWidth < width + targetGroup->padding)
			{
				effectiveWidth = Utility::Math::SmallestGreaterEqualPow2(width + targetGroup->padding);
			}
			if (effectiveHeight < height + targetGroup->padding)
			{
				effectiveHeight = Utility::Math::SmallestGreaterEqualPow2(height + targetGroup->padding);
			}

			std::shared_ptr<DynamicTextureResource> atlasTexture = m_ownerResourceManager->GetTextureManager().CreateDynamicTexture(effectiveWidth, effectiveHeight);
			if (!atlasTexture)
			{
				// Cannot Create Texture
				return nullptr;
			}

			std::shared_ptr<RuntimeTextureAtlas> createdAtlas(std::make_shared<RuntimeTextureAtlas>(effectiveWidth, effectiveHeight, atlasTexture, targetGroup->padding));
			std::shared_ptr<RuntimeTextureAtlasEntry> createdEntry = createdAtlas->InsertSubTexture(imageData, width, height, format, dataType, restorer);
			if (!createdEntry)
			{
				// Atlas Error???
				return nullptr;
			}

			int writeLockResult = pthread_rwlock_wrlock(&m_groupTableLock);
			if (writeLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_groupTableLock;
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_WRITE_LOCK, releaseLockFunc);

				targetGroup->atlasList.push_back(createdAtlas);
			}
			else
			{
				// Cannot Update Group Data
				return nullptr;
			}

			return createdEntry;
		}

		//------------------------------------------------------------------------------
		void RuntimeTextureAtlasManager::_CheckAndRestoreManagedResources()
		{
			std::vector<std::shared_ptr<RuntimeTextureAtlas> > collectedAtlas;

			// Collect All Atlas
			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_groupTableLock)) == EAGAIN)
			{
				sched_yield();
			}
			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_groupTableLock;
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				for (const std::pair<AtlasGroupIdType, std::shared_ptr<_Internal::RuntimeTextureAtlasGroupData> > &currentGroup : m_atlasGroupTable)
				{
					int groupLockResult;
					while ((groupLockResult = pthread_rwlock_rdlock(&(currentGroup.second->atlasListLock))) == EAGAIN)
					{
						sched_yield();
					}
					if (groupLockResult == 0)
					{
						pthread_rwlock_t *atlasListLock = &(currentGroup.second->atlasListLock);
						auto releaseAtlasLockFunc = [atlasListLock]() mutable { pthread_rwlock_unlock(atlasListLock); };
						FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseAtlasLockFunc);

						collectedAtlas.insert(collectedAtlas.end(), currentGroup.second->atlasList.begin(), currentGroup.second->atlasList.end());
					}
					else
					{
						// Anyway, Go
						collectedAtlas.insert(collectedAtlas.end(), currentGroup.second->atlasList.begin(), currentGroup.second->atlasList.end());
					}
				}
			}
			else
			{
				// Anyway, Go
				for (const std::pair<AtlasGroupIdType, std::shared_ptr<_Internal::RuntimeTextureAtlasGroupData> > &currentGroup : m_atlasGroupTable)
				{
					int groupLockResult;
					while ((groupLockResult = pthread_rwlock_rdlock(&(currentGroup.second->atlasListLock))) == EAGAIN)
					{
						sched_yield();
					}
					if (groupLockResult == 0)
					{
						pthread_rwlock_t *listLock = &(currentGroup.second->atlasListLock);
						auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
						FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

						collectedAtlas.insert(collectedAtlas.end(), currentGroup.second->atlasList.begin(), currentGroup.second->atlasList.end());
					}
					else
					{
						// Anyway, Go
						collectedAtlas.insert(collectedAtlas.end(), currentGroup.second->atlasList.begin(), currentGroup.second->atlasList.end());
					}
				}
			}

			collectedAtlas.erase(std::remove_if(collectedAtlas.begin(), collectedAtlas.end(), [](const std::shared_ptr<RuntimeTextureAtlas> &x) {
				return x->m_texture->IsValid();
				}), collectedAtlas.end());
			for (const std::shared_ptr<RuntimeTextureAtlas> &currentAtlas : collectedAtlas)
			{
				currentAtlas->_RestoreTexture();
			}
		}

		//------------------------------------------------------------------------------
		void RuntimeTextureAtlasManager::_ShutdownManager()
		{
			int writeLockResult = pthread_rwlock_wrlock(&m_groupTableLock);
			if (writeLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_groupTableLock;
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_WRITE_LOCK, releaseLockFunc);

				m_atlasGroupTable.clear();
			}
			else
			{
				// Anyway, Go
				m_atlasGroupTable.clear();
			}
		}
	}
}
