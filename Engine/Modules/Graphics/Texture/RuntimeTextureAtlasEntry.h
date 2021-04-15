////////////////////////////////////////////////////////////////////////////////
// Texture/RuntimeTextureAtlasEntry.h (Leggiero/Modules - Graphics)
//
// Each sub-texture entry uploaded on a runtime texture atlas
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__TEXTURE__RUNTIME_TEXTURE_ATLAS_ENTRY_H
#define __LM_GRAPHICS__TEXTURE__RUNTIME_TEXTURE_ATLAS_ENTRY_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>
#include <memory>

// External Library
#include <GLES3.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>

// Leggiero.Graphics
#include "TextureSection.h"


namespace Leggiero
{
	namespace Graphics
	{
		// Forward Declarations
		class RuntimeTextureAtlas;
		class GLTextureResource;
		class DynamicTextureResource;

		namespace _Internal
		{
			struct SpaceNode;
		}


		// Runtime Atlas Texture Entry
		class RuntimeTextureAtlasEntry
			: private Utility::SyntacticSugar::NonCopyable
		{
			friend class RuntimeTextureAtlas;
			friend struct _Internal::SpaceNode;

		public:
			using RestorerType = std::function<bool(std::shared_ptr<DynamicTextureResource>, const TextureRectSection &)>;
			static RestorerType kVoidRestorer;

		protected:
			using EntryIdType = int;

		public:
			virtual ~RuntimeTextureAtlasEntry();

		protected:
			RuntimeTextureAtlasEntry(std::shared_ptr<RuntimeTextureAtlas> atlas, EntryIdType id, const TextureRectSection &section, RestorerType restorer = kVoidRestorer);

		private:
			RuntimeTextureAtlasEntry() = delete;

		public:
			std::shared_ptr<GLTextureResource> GetTexture();
			const TextureRectSection &GetTextureSection() const { return m_sectionArea; }

			void MarkRestored() { m_isValid = true; }
			bool IsValid() const { return m_isValid; }

		protected:
			void _Invalidate() { m_isValid = false; }
			bool _TryRestore();

		protected:
			bool m_isValid;
			EntryIdType m_id;
			std::shared_ptr<RuntimeTextureAtlas> m_atlas;
			TextureRectSection m_sectionArea;

			RestorerType m_restorer;
		};
	}
}

#endif
