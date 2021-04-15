////////////////////////////////////////////////////////////////////////////////
// BundleFileResourceComponent_NPO.h (Leggiero/Modules - FileSystem)
//
// Non-Portable Optimization Interface for Bundle File Resource Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FILESYSTEM__BUNDLE_FILE_RESOURCE_COMPONENT_NPO_H
#define __LM_FILESYSTEM__BUNDLE_FILE_RESOURCE_COMPONENT_NPO_H


// Standard Library
#include <string>


namespace Leggiero
{
	namespace FileSystem
	{
		namespace NPO
		{
			// Non-Portable Interface for Bundle FIle Resources
			class INonPortableOptimization_BundleFileResource
			{
			public:
				virtual bool NPO_IsEnable_BundleFileRealPath() const { return false; }
				virtual std::string NPO_GetBundleFileRealPath(const std::string &virtualPath) { return ""; }
			};
		}
	}
}

#endif
