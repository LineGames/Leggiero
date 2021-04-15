////////////////////////////////////////////////////////////////////////////////
// Loader/UITableAssetLoader.cpp (Leggiero/Modules - LegacyUI)
//
// Table asset loader implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UITableAssetLoader.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// UITableAssetLoader

		//------------------------------------------------------------------------------
		UITableAssetLoader::UITableAssetLoader()
		{
		}

		//------------------------------------------------------------------------------
		UITableAssetLoader::~UITableAssetLoader()
		{
		}

		//------------------------------------------------------------------------------
		size_t UITableAssetLoader::ReadDescriptionUnitData(const std::string &unitName, std::ostream &buffer)
		{
			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_descriptionTableLock.GetLock())) == EAGAIN)
			{
				sched_yield();
			}

			size_t readBytes = 0;
			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_descriptionTableLock.GetLock();
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				std::unordered_map<std::string, std::string>::const_iterator findIt = m_descriptionTable.find(unitName);
				if (findIt != m_descriptionTable.cend())
				{
					buffer.write(findIt->second.c_str(), findIt->second.length());
					readBytes = findIt->second.length();
				}
			}

			return readBytes;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<Graphics::GLTextureResource> UITableAssetLoader::LoadTexture(const std::string &textureName)
		{
			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_textureTableLock.GetLock())) == EAGAIN)
			{
				sched_yield();
			}
            
			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_textureTableLock.GetLock();
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				std::unordered_map<std::string, std::shared_ptr<Graphics::GLTextureResource> >::const_iterator findIt = m_textureTable.find(textureName);
				if (findIt != m_textureTable.cend())
				{
					return findIt->second;
				}
			}

			return nullptr;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<Graphics::TextureAtlasTable> UITableAssetLoader::LoadTextureAtlasTable(const std::string &textureName, std::shared_ptr<Graphics::GLTextureResource> texture)
		{
			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_textureAtlasDataTableLock.GetLock())) == EAGAIN)
			{
				sched_yield();
			}
            
			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_textureAtlasDataTableLock.GetLock();
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				std::unordered_map<std::string, std::shared_ptr<Graphics::TextureAtlasTable> >::const_iterator findIt = m_textureAtlasDataTable.find(textureName);
				if (findIt != m_textureAtlasDataTable.cend())
				{
					return findIt->second;
				}
			}

			return nullptr;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<Font::IFontFace> UITableAssetLoader::LoadFontFace(const std::string &fontName)
		{
			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_fontFaceTableLock.GetLock())) == EAGAIN)
			{
				sched_yield();
			}
            
			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_fontFaceTableLock.GetLock();
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				std::unordered_map<std::string, std::shared_ptr<Font::IFontFace> >::const_iterator findIt = m_fontFaceTable.find(fontName);
				if (findIt != m_fontFaceTable.cend())
				{
					return findIt->second;
				}
			}

			return nullptr;
		}

		//------------------------------------------------------------------------------
		bool UITableAssetLoader::RegisterDescriptionUnit(const std::string &unitName, const std::string &unitContents)
		{
			if (unitContents.empty())
			{
				// Cannot register empty unit
				return false;
			}

			int lockResult = pthread_rwlock_wrlock(&m_descriptionTableLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_descriptionTableLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				std::unordered_map<std::string, std::string>::iterator findIt = m_descriptionTable.find(unitName);
				if (findIt != m_descriptionTable.end())
				{
					m_descriptionTable[unitName] = unitContents;
				}
				else
				{
					m_descriptionTable.insert(std::make_pair(unitName, unitContents));
				}
			}
			else
			{
				// Lock Failed
				return false;
			}

			return true;
		}

		//------------------------------------------------------------------------------
		void UITableAssetLoader::RemoveDescriptionUnit(const std::string &unitName)
		{
			int lockResult = pthread_rwlock_wrlock(&m_descriptionTableLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_descriptionTableLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_descriptionTable.erase(unitName);
			}
			else
			{
				// Lock Failed - ignore
			}
		}

		//------------------------------------------------------------------------------
		bool UITableAssetLoader::RegisterTexture(const std::string &textureName, std::shared_ptr<Graphics::GLTextureResource> texture)
		{
			if (!texture)
			{
				// Cannot register empty texture
				return false;
			}

			int lockResult = pthread_rwlock_wrlock(&m_textureTableLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_textureTableLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				std::unordered_map<std::string, std::shared_ptr<Graphics::GLTextureResource> >::iterator findIt = m_textureTable.find(textureName);
				if (findIt != m_textureTable.end())
				{
					m_textureTable[textureName] = texture;
				}
				else
				{
					m_textureTable.insert(std::make_pair(textureName, texture));
				}
			}
			else
			{
				// Lock Failed
				return false;
			}

			return true;
		}

		//------------------------------------------------------------------------------
		void UITableAssetLoader::RemoveTexture(const std::string &textureName)
		{
			int lockResult = pthread_rwlock_wrlock(&m_textureTableLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_textureTableLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_textureTable.erase(textureName);
			}
			else
			{
				// Lock Failed - ignore
			}
		}

		//------------------------------------------------------------------------------
		bool UITableAssetLoader::RegisterTextureAtlasTable(const std::string &textureName, std::shared_ptr<Graphics::TextureAtlasTable> textureAtlasTable)
		{
			if (!textureAtlasTable)
			{
				// Cannot register empty texture atlas
				return false;
			}

			int lockResult = pthread_rwlock_wrlock(&m_textureAtlasDataTableLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_textureAtlasDataTableLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				std::unordered_map<std::string, std::shared_ptr<Graphics::TextureAtlasTable> >::iterator findIt = m_textureAtlasDataTable.find(textureName);
				if (findIt != m_textureAtlasDataTable.end())
				{
					m_textureAtlasDataTable[textureName] = textureAtlasTable;
				}
				else
				{
					m_textureAtlasDataTable.insert(std::make_pair(textureName, textureAtlasTable));
				}
			}
			else
			{
				// Lock Failed
				return false;
			}

			return true;
		}

		//------------------------------------------------------------------------------
		void UITableAssetLoader::RemoveTextureAtlasTable(const std::string &textureName)
		{
			int lockResult = pthread_rwlock_wrlock(&m_textureAtlasDataTableLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_textureAtlasDataTableLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_textureAtlasDataTable.erase(textureName);
			}
			else
			{
				// Lock Failed - ignore
			}
		}

		//------------------------------------------------------------------------------
		bool UITableAssetLoader::RegisterFontFace(const std::string &fontName, std::shared_ptr<Font::IFontFace> fontFace)
		{
			if (!fontFace)
			{
				// Cannot register empty font
				return false;
			}

			int lockResult = pthread_rwlock_wrlock(&m_fontFaceTableLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_fontFaceTableLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				std::unordered_map<std::string, std::shared_ptr<Font::IFontFace> >::iterator findIt = m_fontFaceTable.find(fontName);
				if (findIt != m_fontFaceTable.end())
				{
					m_fontFaceTable[fontName] = fontFace;
				}
				else
				{
					m_fontFaceTable.insert(std::make_pair(fontName, fontFace));
				}
			}
			else
			{
				// Lock Failed
				return false;
			}

			return true;
		}

		//------------------------------------------------------------------------------
		void UITableAssetLoader::RemoveFontFace(const std::string &fontName)
		{
			int lockResult = pthread_rwlock_wrlock(&m_fontFaceTableLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_fontFaceTableLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_fontFaceTable.erase(fontName);
			}
			else
			{
				// Lock Failed - ignore
			}
		}

		//------------------------------------------------------------------------------
		void UITableAssetLoader::ClearLoader()
		{
			ClearDescriptionUnitTable();
			ClearTextureTable();
			ClearTextureAtlasDataTable();
			ClearFontFaceTable();
		}

		//------------------------------------------------------------------------------
		void UITableAssetLoader::ClearDescriptionUnitTable()
		{
			int lockResult = pthread_rwlock_wrlock(&m_descriptionTableLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_descriptionTableLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_descriptionTable.clear();
			}
			else
			{
				// Lock Failed - ignore
			}
		}

		//------------------------------------------------------------------------------
		void UITableAssetLoader::ClearTextureTable()
		{
			int lockResult = pthread_rwlock_wrlock(&m_textureTableLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_textureTableLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_textureTable.clear();
			}
			else
			{
				// Lock Failed - ignore
			}
		}

		//------------------------------------------------------------------------------
		void UITableAssetLoader::ClearTextureAtlasDataTable()
		{
			int lockResult = pthread_rwlock_wrlock(&m_textureAtlasDataTableLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_textureAtlasDataTableLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_textureAtlasDataTable.clear();
			}
			else
			{
				// Lock Failed - ignore
			}
		}

		//------------------------------------------------------------------------------
		void UITableAssetLoader::ClearFontFaceTable()
		{
			int lockResult = pthread_rwlock_wrlock(&m_fontFaceTableLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_fontFaceTableLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_fontFaceTable.clear();
			}
			else
			{
				// Lock Failed - ignore
			}
		}

		//------------------------------------------------------------------------------
		bool UITableAssetLoader::BulkRegisterDescriptionUnit(std::initializer_list<std::pair<const std::string, std::string> > unitPairs)
		{
			int lockResult = pthread_rwlock_wrlock(&m_descriptionTableLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_descriptionTableLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_descriptionTable.insert(unitPairs);

				for (std::unordered_map<std::string, std::string>::iterator it = m_descriptionTable.begin(); it != m_descriptionTable.end(); )
				{
					if (it->second.empty())
					{
						it = m_descriptionTable.erase(it);
					}
					else
					{
						++it;
					}
				}
			}
			else
			{
				// Lock Failed
				return false;
			}
			return true;
		}

		//------------------------------------------------------------------------------
		bool UITableAssetLoader::BulkRegisterTexture(std::initializer_list<std::pair<const std::string, std::shared_ptr<Graphics::GLTextureResource> > > unitPairs)
		{
			int lockResult = pthread_rwlock_wrlock(&m_textureTableLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_textureTableLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_textureTable.insert(unitPairs);

				for (std::unordered_map<std::string, std::shared_ptr<Graphics::GLTextureResource> >::iterator it = m_textureTable.begin(); it != m_textureTable.end(); )
				{
					if (!(it->second))
					{
						it = m_textureTable.erase(it);
					}
					else
					{
						++it;
					}
				}
			}
			else
			{
				// Lock Failed
				return false;
			}
			return true;
		}

		//------------------------------------------------------------------------------
		bool UITableAssetLoader::BulkRegisterDescriptionUnit(std::initializer_list<std::pair<const std::string, std::shared_ptr<Graphics::TextureAtlasTable> > > unitPairs)
		{
			int lockResult = pthread_rwlock_wrlock(&m_textureAtlasDataTableLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_textureAtlasDataTableLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_textureAtlasDataTable.insert(unitPairs);

				for (std::unordered_map<std::string, std::shared_ptr<Graphics::TextureAtlasTable> >::iterator it = m_textureAtlasDataTable.begin(); it != m_textureAtlasDataTable.end(); )
				{
					if (!(it->second))
					{
						it = m_textureAtlasDataTable.erase(it);
					}
					else
					{
						++it;
					}
				}
			}
			else
			{
				// Lock Failed
				return false;
			}
			return true;
		}

		//------------------------------------------------------------------------------
		bool UITableAssetLoader::BulkRegisterDescriptionUnit(std::initializer_list<std::pair<const std::string, std::shared_ptr<Font::IFontFace> > > unitPairs)
		{
			int lockResult = pthread_rwlock_wrlock(&m_fontFaceTableLock.GetLock());
			if (lockResult == 0)
			{
				pthread_rwlock_t *lockCopy = &m_fontFaceTableLock.GetLock();
				auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				m_fontFaceTable.insert(unitPairs);

				for (std::unordered_map<std::string, std::shared_ptr<Font::IFontFace> >::iterator it = m_fontFaceTable.begin(); it != m_fontFaceTable.end(); )
				{
					if (!(it->second))
					{
						it = m_fontFaceTable.erase(it);
					}
					else
					{
						++it;
					}
				}
			}
			else
			{
				// Lock Failed
				return false;
			}
			return true;
		}
	}
}
