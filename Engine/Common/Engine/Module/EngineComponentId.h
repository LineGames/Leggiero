////////////////////////////////////////////////////////////////////////////////
// Module/EngineComponentId.h (Leggiero - Engine)
//
// Engine Component Id Definitions
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__MODULE__ENGINE_COMPONENT_ID_H
#define __ENGINE__MODULE__ENGINE_COMPONENT_ID_H


namespace Leggiero
{
	// Engine Component Id Type
	enum class EngineComponentIdType
	{
		kINVALID = 0,

		kApplication = 1,

		kFileSystemPath = 11,
		kBundleFileResource = 12, 

		kTaskManager = 21, 

		kAsyncTaskHTTP = 52, 

		kTouchInput = 101,

		kGraphicResourceManager = 211,
		kGlyphManager = 221,

		kSoundMixer = 301, 
		kBGMPlayer = 302,
	};
}

#endif
