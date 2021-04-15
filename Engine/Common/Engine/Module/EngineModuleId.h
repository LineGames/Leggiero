////////////////////////////////////////////////////////////////////////////////
// Module/EngineModuleId.h (Leggiero - Engine)
//
// Engine Module Id Definitions
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__MODULE__ENGINE_MODULE_ID_H
#define __ENGINE__MODULE__ENGINE_MODULE_ID_H


namespace Leggiero
{
	// Engine Module Id Type
	enum class EngineModuleIdType
	{
		kINVALID = 0,

		kLog = 8,

		kHTTP = 51, 

		kCrypto = 81,

		kGraphics = 200,
		kFont = 220, 
	};
}

#endif
