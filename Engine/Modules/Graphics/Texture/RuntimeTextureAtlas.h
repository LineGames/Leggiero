////////////////////////////////////////////////////////////////////////////////
// Texture/RuntimeTextureAtlas.h (Leggiero/Modules - Graphics)
//
// Runtime Texture Atlas Class
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__TEXTURE__RUNTIME_TEXTURE_ATLAS_H
#define __LM_GRAPHICS__TEXTURE__RUNTIME_TEXTURE_ATLAS_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <vector>

// External Library
#include <GLES3.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Graphics
#include "RuntimeTextureAtlasEntry.h"


namespace Leggiero
{
	namespace Graphics
	{
		// Forward Declarations
		class DynamicTextureResource;

		namespace _Internal
		{
			struct SpaceNode;
		}


		// Runtime Texture Atlas
		class RuntimeTextureAtlas
			: public std::enable_shared_from_this<RuntimeTextureAtlas>
			, private Utility::SyntacticSugar::NonCopyable
		{
			friend class RuntimeTextureAtlasEntry;
			friend class RuntimeTextureAtlasManager;

		public:
			RuntimeTextureAtlas(GLsizei textureWidth, GLsizei textureHeight, std::shared_ptr<DynamicTextureResource> texture, GLsizei padding = 0);
			virtual ~RuntimeTextureAtlas();

		public:
			std::shared_ptr<RuntimeTextureAtlasEntry> AllocateSubTexture(GLsizei width, GLsizei height, RuntimeTextureAtlasEntry::RestorerType restorer = RuntimeTextureAtlasEntry::kVoidRestorer);
			std::shared_ptr<RuntimeTextureAtlasEntry> InsertSubTexture(const void *imageData, GLsizei width, GLsizei height, GLenum format = GL_RGBA, GLenum dataType = GL_UNSIGNED_BYTE, RuntimeTextureAtlasEntry::RestorerType restorer = RuntimeTextureAtlasEntry::kVoidRestorer);

		protected:
			void _NotifyEntryDelete(RuntimeTextureAtlasEntry::EntryIdType entryId);
			void _RestoreTexture();

		protected:
			Utility::Threading::SafePthreadLock					m_atlasModifyLock;
			std::atomic<RuntimeTextureAtlasEntry::EntryIdType>	m_issueingEntryId;

			std::shared_ptr<DynamicTextureResource> m_texture;

			std::shared_ptr<_Internal::SpaceNode>				m_spaceTreeRoot;
			std::vector<std::shared_ptr<_Internal::SpaceNode> > m_emptySpaceList;

			std::vector<std::weak_ptr<RuntimeTextureAtlasEntry> > m_createdEntries;
			std::map<RuntimeTextureAtlasEntry::EntryIdType, std::shared_ptr<_Internal::SpaceNode> > m_idEntryLookupTable;

			GLsizei m_width;
			GLsizei m_height;
			GLsizei m_padding;
		};
	}
}

#endif
