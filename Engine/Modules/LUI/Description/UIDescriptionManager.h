////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionManager.h (Leggiero/Modules - LegacyUI)
//
// Manager class for UI Description
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__DESCRIPTION__UI_DESCRIPTION_MANAGER_H
#define __LM_LUI__DESCRIPTION__UI_DESCRIPTION_MANAGER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "UIDescriptionTypes.h"


namespace Leggiero
{
	// Forward Declarations
	namespace Font
	{
		class GlyphManagerComponent;
		class IFontFace;
	}


	namespace LUI
	{
		// Forward Declarations
		class IUIAssetLoader;
		class UICachedTexture;
		class UITextureManager;
		class UIFontFaceManager;
		class UIManager;
		class UIObject;


		namespace Description
		{
			// Forward Declarations
			class DescriptionUnit;
			class IDescriptionObject;

			namespace Prefab
			{
				class IUIPrefabPlaceProcessor;
			}


			// Description Manager
			class DescriptionManager
			{
			public:
				DescriptionManager(Font::GlyphManagerComponent &engineGlyphManager, IUIAssetLoader *pAssetLoader = nullptr, UITextureManager *pTextureManager = nullptr, UIFontFaceManager *pFontFaceManager = nullptr, DataStringBagType *pDataStringBag = nullptr);
				virtual ~DescriptionManager();

			private:
				DescriptionManager() = delete;

			public:
				std::shared_ptr<DescriptionUnit> GetUnit(const UnitNameType &unitName);
				void PreLoadUnit(const UnitNameType &unitName);

				void AddUnit(const UnitNameType &unitName, std::shared_ptr<DescriptionUnit> unit);

				std::shared_ptr<UICachedTexture> GetTexture(const UITextureNameType &textureName);
				void PreLoadTexture(const UITextureNameType &textureName);

				std::shared_ptr<Font::IFontFace> GetFontFace(const UIFontNameType &fontName);
				void PreLoadFontFace(const UIFontNameType &fontName);

				std::shared_ptr<DescriptionUnit> GetCachedUnit(const SavedUnitNameType &savedUnitName);
				std::shared_ptr<UICachedTexture> GetCachedTexture(const UITextureNameType &savedTextureName);

				void ClearDescriptionCache(std::shared_ptr<DescriptionUnit> newSystemUnit = std::shared_ptr<DescriptionUnit>());

			public:
				std::shared_ptr<UIObject> FabricateUIObject(const std::string &unitName, const std::string &prefabName, std::shared_ptr<UIManager> creatingManager, Prefab::IUIPrefabPlaceProcessor *placeProcessor = nullptr);
				std::shared_ptr<UIObject> FabricateUIObject(std::shared_ptr<DescriptionUnit> unit, const std::string &prefabName, std::shared_ptr<UIManager> creatingManager, Prefab::IUIPrefabPlaceProcessor *placeProcessor = nullptr);
				std::shared_ptr<UIObject> FabricateUIObjectByPath(const std::string &unitName, const std::string &prefabPath, std::shared_ptr<UIManager> creatingManager, Prefab::IUIPrefabPlaceProcessor *placeProcessor = nullptr);
				std::shared_ptr<UIObject> FabricateUIObjectByPath(std::shared_ptr<DescriptionUnit> unit, const std::string &prefabPath, std::shared_ptr<UIManager> creatingManager, Prefab::IUIPrefabPlaceProcessor *placeProcessor = nullptr);

				template <typename ValueT>
				ValueT ReadDescriptionValue(const std::string &unitName, const std::string &valueName)
				{
					return ReadDescriptionValue<ValueT>(GetUnit(unitName), valueName);
				}
				template <typename ValueT>
				ValueT ReadDescriptionValue(std::shared_ptr<DescriptionUnit> unit, const std::string &valueName);
				template <typename ValueT>
				ValueT ReadDescriptionValueByPath(const std::string &unitName, const std::string &valuePath)
				{
					return ReadDescriptionValueByPath<ValueT>(GetUnit(unitName), valuePath);
				}
				template <typename ValueT>
				ValueT ReadDescriptionValueByPath(std::shared_ptr<DescriptionUnit> unit, const std::string &valuePath);

			public:
				GameDataString GetDataString(DataStringKeyType key);

			public:
				Font::GlyphManagerComponent &GetGlyphManager() { return m_engineGlyphManager; }

			public:
				void DEBUG_Reload();

			protected:
				std::shared_ptr<DescriptionUnit> _LoadBundleUIUnitByName(const UnitNameType &unitName);

			protected:
				Font::GlyphManagerComponent &m_engineGlyphManager;

				IUIAssetLoader		&m_assetLoader;
				UITextureManager	&m_textureManager;
				UIFontFaceManager	&m_fontFaceManager;

				DataStringBagType	&m_stringBag;

				std::unordered_map<SavedUnitNameType, std::shared_ptr<DescriptionUnit> > m_unitCache;
				std::unordered_set<SavedUnitNameType> m_loadingUnitNames;
			};


			// Template specialization to read value
			template<> BooleanValueType DescriptionManager::ReadDescriptionValue<BooleanValueType>(std::shared_ptr<DescriptionUnit> unit, const std::string &valueName);
			template<> BooleanValueType DescriptionManager::ReadDescriptionValueByPath<BooleanValueType>(std::shared_ptr<DescriptionUnit> unit, const std::string &valuePath);

			template<> IntegerValueType DescriptionManager::ReadDescriptionValue<IntegerValueType>(std::shared_ptr<DescriptionUnit> unit, const std::string &valueName);
			template<> IntegerValueType DescriptionManager::ReadDescriptionValueByPath<IntegerValueType>(std::shared_ptr<DescriptionUnit> unit, const std::string &valuePath);

			template<> FloatingPointValueType DescriptionManager::ReadDescriptionValue<FloatingPointValueType>(std::shared_ptr<DescriptionUnit> unit, const std::string &valueName);
			template<> FloatingPointValueType DescriptionManager::ReadDescriptionValueByPath<FloatingPointValueType>(std::shared_ptr<DescriptionUnit> unit, const std::string &valuePath);

			template<> ColorARGBValueType DescriptionManager::ReadDescriptionValue<ColorARGBValueType>(std::shared_ptr<DescriptionUnit> unit, const std::string &valueName);
			template<> ColorARGBValueType DescriptionManager::ReadDescriptionValueByPath<ColorARGBValueType>(std::shared_ptr<DescriptionUnit> unit, const std::string &valuePath);
		}
	}
}

#endif
