////////////////////////////////////////////////////////////////////////////////
// Texture/RuntimeTextureAtlasEntry.cpp (Leggiero/Modules - Graphics)
//
// Implementation of OpenGL ES Texture Resource
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "RuntimeTextureAtlasEntry.h"

// Leggiero.Graphics
#include "RuntimeTextureAtlas.h"
#include "DynamicTextureResource.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// RuntimeTextureAtlasEntry

		//------------------------------------------------------------------------------
		RuntimeTextureAtlasEntry::RestorerType RuntimeTextureAtlasEntry::kVoidRestorer = [](std::shared_ptr<DynamicTextureResource> _dummy1, const TextureRectSection &_dummy2) { return false; };

		//------------------------------------------------------------------------------
		RuntimeTextureAtlasEntry::RuntimeTextureAtlasEntry(std::shared_ptr<RuntimeTextureAtlas> atlas, RuntimeTextureAtlasEntry::EntryIdType id, const TextureRectSection &section, RuntimeTextureAtlasEntry::RestorerType restorer)
			: m_atlas(atlas), m_restorer(restorer), m_id(id), m_sectionArea(section), m_isValid(true)
		{
		}

		//------------------------------------------------------------------------------
		RuntimeTextureAtlasEntry::~RuntimeTextureAtlasEntry()
		{
			m_atlas->_NotifyEntryDelete(m_id);
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<GLTextureResource> RuntimeTextureAtlasEntry::GetTexture()
		{
			return std::dynamic_pointer_cast<GLTextureResource>(m_atlas->m_texture);
		}

		//------------------------------------------------------------------------------
		bool RuntimeTextureAtlasEntry::_TryRestore()
		{
			if (m_isValid)
			{
				return true;
			}

			bool result = m_restorer(m_atlas->m_texture, m_sectionArea);
			if (result)
			{
				m_isValid = true;
			}
			return result;
		}
	}
}
