////////////////////////////////////////////////////////////////////////////////
// Loader/UITableAssetLoader.h (Leggiero/Modules - LegacyUI)
//
// Asset data loader from pre-registered table
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__LOADER__UI_TABLE_ASSET_LOADER_H
#define __LM_LUI__LOADER__UI_TABLE_ASSET_LOADER_H


// Standard Library
#include <initializer_list>
#include <unordered_map>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.LegacyUI
#include "IUIAssetLoader.h"


namespace Leggiero
{
	namespace LUI
	{
		// Table Asset Loader
		class UITableAssetLoader
			: public IUIAssetLoader
		{
		public:
			UITableAssetLoader();
			virtual ~UITableAssetLoader();

		public:	// IUIAssetLoader
			virtual size_t ReadDescriptionUnitData(const std::string &unitName, std::ostream &buffer) override;

			virtual std::shared_ptr<Graphics::GLTextureResource> LoadTexture(const std::string &textureName) override;
			virtual std::shared_ptr<Graphics::TextureAtlasTable> LoadTextureAtlasTable(const std::string &textureName, std::shared_ptr<Graphics::GLTextureResource> texture = nullptr) override;

			virtual std::shared_ptr<Font::IFontFace> LoadFontFace(const std::string &fontName) override;

		public:
			bool RegisterDescriptionUnit(const std::string &unitName, const std::string &unitContents);
			void RemoveDescriptionUnit(const std::string &unitName);

			bool RegisterTexture(const std::string &textureName, std::shared_ptr<Graphics::GLTextureResource> texture);
			void RemoveTexture(const std::string &textureName);

			bool RegisterTextureAtlasTable(const std::string &textureName, std::shared_ptr<Graphics::TextureAtlasTable> textureAtlasTable);
			void RemoveTextureAtlasTable(const std::string &textureName);

			bool RegisterFontFace(const std::string &fontName, std::shared_ptr<Font::IFontFace> fontFace);
			void RemoveFontFace(const std::string &fontName);

			void ClearLoader();
			void ClearDescriptionUnitTable();
			void ClearTextureTable();
			void ClearTextureAtlasDataTable();
			void ClearFontFaceTable();

		public:
			template <class InputIterator>
			bool BulkRegisterDescriptionUnit(InputIterator first, InputIterator last)
			{
				int lockResult = pthread_rwlock_wrlock(&m_descriptionTableLock.GetLock());
				if (lockResult == 0)
				{
					pthread_rwlock_t *lockCopy = &m_descriptionTableLock.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					m_descriptionTable.insert(first, last);

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

			bool BulkRegisterDescriptionUnit(std::initializer_list<std::pair<const std::string, std::string> > unitPairs);

			template <class InputIterator>
			bool BulkRegisterTexture(InputIterator first, InputIterator last)
			{
				int lockResult = pthread_rwlock_wrlock(&m_textureTableLock.GetLock());
				if (lockResult == 0)
				{
					pthread_rwlock_t *lockCopy = &m_textureTableLock.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					m_textureTable.insert(first, last);

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

			bool BulkRegisterTexture(std::initializer_list<std::pair<const std::string, std::shared_ptr<Graphics::GLTextureResource> > > unitPairs);

			template <class InputIterator>
			bool BulkRegisterTextureAtlasTable(InputIterator first, InputIterator last)
			{
				int lockResult = pthread_rwlock_wrlock(&m_textureAtlasDataTableLock.GetLock());
				if (lockResult == 0)
				{
					pthread_rwlock_t *lockCopy = &m_textureAtlasDataTableLock.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					m_textureAtlasDataTable.insert(first, last);

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

			bool BulkRegisterDescriptionUnit(std::initializer_list<std::pair<const std::string, std::shared_ptr<Graphics::TextureAtlasTable> > > unitPairs);

			template <class InputIterator>
			bool BulkRegisterFontFace(InputIterator first, InputIterator last)
			{
				int lockResult = pthread_rwlock_wrlock(&m_fontFaceTableLock.GetLock());
				if (lockResult == 0)
				{
					pthread_rwlock_t *lockCopy = &m_fontFaceTableLock.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					m_fontFaceTable.insert(first, last);

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

			bool BulkRegisterDescriptionUnit(std::initializer_list<std::pair<const std::string, std::shared_ptr<Font::IFontFace> > > unitPairs);

		protected:
			std::unordered_map<std::string, std::string>	m_descriptionTable;
			Utility::Threading::SafePthreadRWLock			m_descriptionTableLock;

			std::unordered_map<std::string, std::shared_ptr<Graphics::GLTextureResource> >	m_textureTable;
			Utility::Threading::SafePthreadRWLock											m_textureTableLock;

			std::unordered_map<std::string, std::shared_ptr<Graphics::TextureAtlasTable> >	m_textureAtlasDataTable;
			Utility::Threading::SafePthreadRWLock											m_textureAtlasDataTableLock;

			std::unordered_map<std::string, std::shared_ptr<Font::IFontFace> >	m_fontFaceTable;
			Utility::Threading::SafePthreadRWLock								m_fontFaceTableLock;
		};
	}
}

#endif
