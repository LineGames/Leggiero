////////////////////////////////////////////////////////////////////////////////
// GameModules.cpp (${{ProgramName}} - ${{ProgramName}})
//
// Definition of Modules used by the Game
////////////////////////////////////////////////////////////////////////////////

// Leggiero.Engine
#include <Engine/Toolbox/ModuledGame/ModuledGame.h>

// Leggiero.Log
#include <Log/LogModuleInterface.h>

// Leggiero.Graphics
#include <Graphics/GraphicsModuleInterface.h>

// Leggiero.Font
#include <Font/FontModuleInterface.h>

// Leggiero.HTTP
#include <HTTP/HttpModuleInterface.h>

// Leggiero.Crypto
#include <Crypto/CryptoModuleInterface.h>


namespace Leggiero
{
	namespace Toolbox
	{
		namespace Game
		{
			//------------------------------------------------------------------------------
			std::map<EngineModuleIdType, EngineModuleInterface *> ModuledGame::_ListAllModules()
			{
				std::map<EngineModuleIdType, EngineModuleInterface *> moduleTable;

				moduleTable.insert(std::make_pair(EngineModuleIdType::kLog, &GetModuleInterface<EngineModuleIdType::kLog>()));
				moduleTable.insert(std::make_pair(EngineModuleIdType::kHTTP, &GetModuleInterface<EngineModuleIdType::kHTTP>()));
				moduleTable.insert(std::make_pair(EngineModuleIdType::kCrypto, &GetModuleInterface<EngineModuleIdType::kCrypto>()));
				moduleTable.insert(std::make_pair(EngineModuleIdType::kGraphics, &GetModuleInterface<EngineModuleIdType::kGraphics>()));
				moduleTable.insert(std::make_pair(EngineModuleIdType::kFont, &GetModuleInterface<EngineModuleIdType::kFont>()));

				return moduleTable;
			}
		}
	}
}
