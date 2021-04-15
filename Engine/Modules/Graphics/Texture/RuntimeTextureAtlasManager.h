////////////////////////////////////////////////////////////////////////////////
// Texture/RuntimeTextureAtlasManager.h (Leggiero/Modules - Graphics)
//
// Runtime Texture Atlas Manager Class (member of Resource Manager Component)
// Manage atlases as groups and provide easy fire-and-forgot interface to
// store and use small realtime textures.
// All Load and Creation Function should be called in the thread w/ GL Context
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__TEXTURE__RUNTIME_TEXTURE_ATLAS_MANAGER_H
#define __LM_GRAPHICS__TEXTURE__RUNTIME_TEXTURE_ATLAS_MANAGER_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <atomic>
#include <map>
#include <memory>

// External Library
#include <GLES3.h>
#include <pthread.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>

// Leggiero.Graphics
#include "RuntimeTextureAtlasEntry.h"


namespace Leggiero
{
	namespace Graphics
	{
		// Forward Declarations
		class GraphicResourceManagerComponent;
		
		namespace _Internal
		{
			class RuntimeTextureAtlasGroupData;
		}


		// Texture Manager
		class RuntimeTextureAtlasManager
			: private Utility::SyntacticSugar::NonCopyable
		{
			friend class GraphicResourceManagerComponent;

		public:
			using AtlasGroupIdType = int;
			static const AtlasGroupIdType kDefaultAtlasGroup;

		public:
			RuntimeTextureAtlasManager(GraphicResourceManagerComponent *ownerManager);
			virtual ~RuntimeTextureAtlasManager();

		public:
			AtlasGroupIdType RegisterTextureAtlasGroup(GLsizei padding, GLsizei textureWidth, GLsizei textureHeight = 0);

			std::shared_ptr<RuntimeTextureAtlasEntry> AllocateSubTexture(AtlasGroupIdType atlasGroup, GLsizei width, GLsizei height, RuntimeTextureAtlasEntry::RestorerType restorer = RuntimeTextureAtlasEntry::kVoidRestorer);
			std::shared_ptr<RuntimeTextureAtlasEntry> InsertSubTexture(AtlasGroupIdType atlasGroup, const void *imageData, GLsizei width, GLsizei height, GLenum format = GL_RGBA, GLenum dataType = GL_UNSIGNED_BYTE, RuntimeTextureAtlasEntry::RestorerType restorer = RuntimeTextureAtlasEntry::kVoidRestorer);

		protected:
			void _CheckAndRestoreManagedResources();
			void _ShutdownManager();

		protected:
			GraphicResourceManagerComponent *m_ownerResourceManager;

			std::atomic<AtlasGroupIdType> m_issuingGroupId;

			std::map<AtlasGroupIdType, std::shared_ptr<_Internal::RuntimeTextureAtlasGroupData> > m_atlasGroupTable;
			pthread_rwlock_t m_groupTableLock;
		};
	}
}

#endif
