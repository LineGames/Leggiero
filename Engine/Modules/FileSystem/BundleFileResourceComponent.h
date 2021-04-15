////////////////////////////////////////////////////////////////////////////////
// BundleFileResourceComponent.h (Leggiero/Modules - FileSystem)
//
// Engine Component to access to Bundled File Resources
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FILESYSTEM__BUNDLE_FILE_RESOURCE_COMPONENT_H
#define __LM_FILESYSTEM__BUNDLE_FILE_RESOURCE_COMPONENT_H


// Standard Library
#include <iostream>
#include <string>

// Leggiero.Engine
#include <Engine/Module/EngineComponent.h>
#include <Engine/Module/EngineComponentHolder.h>

// Leggiero.FileSystem
#include "BundleFileResourceComponent_NPO.h"


namespace Leggiero
{
	namespace FileSystem
	{
		// Bundle File Resource Component
		class BundleFileResourceComponent
			: public EngineComponent
			, public NPO::INonPortableOptimization_BundleFileResource
		{
		public:	// Can be created by Creation Function (for each platform)
			static BundleFileResourceComponent *CreateComponentObject();

		public:
			virtual ~BundleFileResourceComponent();

		protected:
			BundleFileResourceComponent();

		public:	// EngineComponent
			// Get Type Id of the Component
			virtual EngineComponentIdType GetComponentType() const override { return EngineComponentIdType::kBundleFileResource; };

		public:	// Bundle Resource Interface
			virtual bool			IsBundleFileExists(const std::string &virtualPath) = 0;
			virtual size_t			GetBundleFileLength(const std::string &virtualPath) = 0;
			virtual size_t			ReadBundleFileData(const std::string &virtualPath, size_t offset, char *buffer, size_t bufferSize) = 0;
			virtual std::streamoff	ReadBundleFileData(const std::string &virtualPath, std::streamoff offset, std::ostream &buffer) = 0;

			virtual bool IsDirectory(const std::string &virtualPath) = 0;
			virtual std::vector<std::string> ListSubDirectories(const std::string &virtualPath) = 0;
			virtual std::vector<std::string> ListFiles(const std::string &virtualPath) = 0;
		};
	}
}


DECLARE_GET_COMPONENT_INTERFACE(Leggiero::FileSystem::BundleFileResourceComponent, Leggiero::EngineComponentIdType::kBundleFileResource);

#endif
