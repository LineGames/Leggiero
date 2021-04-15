////////////////////////////////////////////////////////////////////////////////
// _Internal/_WinPCBundleFileResourceComponent.h (Leggiero/Modules - FileSystem)
//
// WinPC Platform's Bundled File Resources Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FILESYSTEM___INTERNAL__WINPC_BUNDLE_FILE_RESOURCE_COMPONENT_H
#define __LM_FILESYSTEM___INTERNAL__WINPC_BUNDLE_FILE_RESOURCE_COMPONENT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.FileSystem
#include "../BundleFileResourceComponent.h"


namespace Leggiero
{
	namespace FileSystem
	{
		// Bundle File Resource Component
		class WinPCBundleFileResourceComponent
			: public BundleFileResourceComponent
		{
		public:
			WinPCBundleFileResourceComponent();
			virtual ~WinPCBundleFileResourceComponent();

		public:	// EngineComponent
			// Initialize the Component
			virtual void InitializeComponent(Engine::GameProcessAnchor *gameAnchor) override;

		public:	// BundleFileResourceComponent
			virtual bool			IsBundleFileExists(const std::string &virtualPath) override;
			virtual size_t			GetBundleFileLength(const std::string &virtualPath) override;
			virtual size_t			ReadBundleFileData(const std::string &virtualPath, size_t offset, char *buffer, size_t bufferSize) override;
			virtual std::streamoff	ReadBundleFileData(const std::string &virtualPath, std::streamoff offset, std::ostream &buffer) override;

			virtual bool IsDirectory(const std::string &virtualPath) override;
			virtual std::vector<std::string> ListSubDirectories(const std::string &virtualPath) override;
			virtual std::vector<std::string> ListFiles(const std::string &virtualPath) override;

		public:	// NPO::INonPortableOptimization_BundleFileResource
			virtual bool NPO_IsEnable_BundleFileRealPath() const override { return true; }
			virtual std::string NPO_GetBundleFileRealPath(const std::string &virtualPath) override { return _GetBundleResourceFilePath(virtualPath); }

		protected:
			std::string _GetBundleResourceFilePath(const std::string &virtualPath) const;

		protected:
			std::string m_baseBundleRealPath;
			std::string m_platformOverrideBundleRealPath;
		};
	}
}

#endif
