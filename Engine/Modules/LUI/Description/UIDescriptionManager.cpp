////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionManager.cpp (Leggiero/Modules - LegacyUI)
//
// Implementation of UI Description Manager
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIDescriptionManager.h"

// Standard Library
#include <sstream>

// Leggiero.LegacyUI
#include "../Loader/IUIAssetLoader.h"
#include "../Loader/DummyUIAssetLoader.h"
#include "../Rendering/UITextureManager.h"
#include "../Rendering/UIFontFaceManager.h"
#include "UIDescriptionUnit.h"
#include "UIPrefabDescription.h"
#include "UIDescriptionSystemUnit.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			//////////////////////////////////////////////////////////////////////////////// Internal Utility

			namespace _Internal
			{
				static UITextureManager s_dummyTextureManager(DummyAssetLoader::GetInstance());
				static UIFontFaceManager s_dummyFontFaceManager(DummyAssetLoader::GetInstance());
			}


			//////////////////////////////////////////////////////////////////////////////// DescriptionManager

			//------------------------------------------------------------------------------
			DescriptionManager::DescriptionManager(Font::GlyphManagerComponent &engineGlyphManager, IUIAssetLoader *pAssetLoader, UITextureManager *pTextureManager, UIFontFaceManager *pFontFaceManager, DataStringBagType *pDataStringBag)
				: m_engineGlyphManager(engineGlyphManager)
				, m_assetLoader((pAssetLoader == nullptr) ? DummyAssetLoader::GetInstance() : *pAssetLoader)
				, m_textureManager((pTextureManager == nullptr) ? _Internal::s_dummyTextureManager : *pTextureManager)
				, m_fontFaceManager((pFontFaceManager == nullptr) ? _Internal::s_dummyFontFaceManager : *pFontFaceManager)
				, m_stringBag((pDataStringBag == nullptr) ? GetEmptyStringBag() : *pDataStringBag)
			{
			}

			//------------------------------------------------------------------------------
			DescriptionManager::~DescriptionManager()
			{
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<DescriptionUnit> DescriptionManager::GetUnit(const UnitNameType &unitName)
			{
				SavedUnitNameType savedName(TypeUtility::ToSavedUnitName(unitName));

				std::unordered_map<SavedUnitNameType, std::shared_ptr<DescriptionUnit> >::iterator findIt = m_unitCache.find(savedName);
				if (findIt != m_unitCache.end())
				{
					return findIt->second;
				}

				std::shared_ptr<DescriptionUnit> loadedUnit(_LoadBundleUIUnitByName(unitName));
				if (!loadedUnit)
				{
					return nullptr;
				}

				m_unitCache[savedName] = loadedUnit;
				return loadedUnit;
			}

			//------------------------------------------------------------------------------
			void DescriptionManager::PreLoadUnit(const UnitNameType &unitName)
			{
				SavedUnitNameType savedName(TypeUtility::ToSavedUnitName(unitName));

				std::unordered_map<SavedUnitNameType, std::shared_ptr<DescriptionUnit> >::iterator findIt = m_unitCache.find(savedName);
				if (findIt != m_unitCache.end())
				{
					return;
				}

				if (m_loadingUnitNames.find(savedName) != m_loadingUnitNames.end())
				{
					return;
				}
				m_loadingUnitNames.insert(savedName);

				std::shared_ptr<DescriptionUnit> loadedUnit(_LoadBundleUIUnitByName(unitName));
				m_loadingUnitNames.erase(savedName);

				if (!loadedUnit)
				{
					return;
				}

				m_unitCache[savedName] = loadedUnit;
			}

			//------------------------------------------------------------------------------
			void DescriptionManager::AddUnit(const UnitNameType &unitName, std::shared_ptr<DescriptionUnit> unit)
			{
				if (unit)
				{
					m_unitCache[TypeUtility::ToSavedUnitName(unitName)] = unit;
				}
				else
				{
					m_unitCache.erase(TypeUtility::ToSavedUnitName(unitName));
				}
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UICachedTexture> DescriptionManager::GetTexture(const UITextureNameType &textureName)
			{
				return m_textureManager.GetTexture(textureName);
			}

			//------------------------------------------------------------------------------
			void DescriptionManager::PreLoadTexture(const UITextureNameType &textureName)
			{
				m_textureManager.PreLoadTexture(textureName);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<Font::IFontFace> DescriptionManager::GetFontFace(const UIFontNameType &fontName)
			{
				return m_fontFaceManager.GetFont(fontName);
			}

			//------------------------------------------------------------------------------
			void DescriptionManager::PreLoadFontFace(const UIFontNameType &fontName)
			{
				m_fontFaceManager.PreLoadFont(fontName);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<DescriptionUnit> DescriptionManager::GetCachedUnit(const SavedUnitNameType &savedUnitName)
			{
				std::unordered_map<SavedUnitNameType, std::shared_ptr<DescriptionUnit> >::iterator findIt = m_unitCache.find(savedUnitName);
				if (findIt != m_unitCache.end())
				{
					return findIt->second;
				}
				return nullptr;
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UICachedTexture> DescriptionManager::GetCachedTexture(const UITextureNameType &savedTextureName)
			{
				return m_textureManager.GetCachedTexture(savedTextureName);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<DescriptionUnit> DescriptionManager::_LoadBundleUIUnitByName(const UnitNameType &unitName)
			{
				std::stringstream contentsBuffer;
				std::streamoff readBytes = m_assetLoader.ReadDescriptionUnitData(unitName, contentsBuffer);
				if (readBytes == 0)
				{
					// Invalid File
					return nullptr;
				}

				return DescriptionUnit::LoadFromXML(contentsBuffer.str(), *this);
			}

			//------------------------------------------------------------------------------
			GameDataString DescriptionManager::GetDataString(DataStringKeyType key)
			{
				return m_stringBag.GetString(key);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> DescriptionManager::FabricateUIObject(const std::string &unitName, const std::string &prefabName, std::shared_ptr<UIManager> creatingManager, Prefab::IUIPrefabPlaceProcessor *placeProcessor)
			{
				std::shared_ptr<DescriptionUnit> foundUnit(GetUnit(unitName));
				if (!foundUnit)
				{
					return nullptr;
				}

				return FabricateUIObject(foundUnit, prefabName, creatingManager, placeProcessor);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> DescriptionManager::FabricateUIObject(std::shared_ptr<DescriptionUnit> unit, const std::string &prefabName, std::shared_ptr<UIManager> creatingManager, Prefab::IUIPrefabPlaceProcessor *placeProcessor)
			{
				ObjectEnvironmentPair foundPrefab = unit->SelectDescriptionByName(*this, prefabName);
				if (!std::get<0>(foundPrefab))
				{
					return nullptr;
				}

				std::shared_ptr<UIPrefabDescription> foundObjectPrefab(std::dynamic_pointer_cast<UIPrefabDescription>(std::get<0>(foundPrefab)));
				if (!foundObjectPrefab)
				{
					return nullptr;
				}

				return foundObjectPrefab->Fabricate(std::get<1>(foundPrefab), creatingManager, placeProcessor);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> DescriptionManager::FabricateUIObjectByPath(const std::string &unitName, const std::string &prefabPath, std::shared_ptr<UIManager> creatingManager, Prefab::IUIPrefabPlaceProcessor *placeProcessor)
			{
				std::shared_ptr<DescriptionUnit> foundUnit(GetUnit(unitName));
				if (!foundUnit)
				{
					return nullptr;
				}

				return FabricateUIObjectByPath(foundUnit, prefabPath, creatingManager, placeProcessor);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> DescriptionManager::FabricateUIObjectByPath(std::shared_ptr<DescriptionUnit> unit, const std::string &prefabPath, std::shared_ptr<UIManager> creatingManager, Prefab::IUIPrefabPlaceProcessor *placeProcessor)
			{
				ObjectEnvironmentPair foundPrefab = unit->SelectDescriptionByPath(*this, prefabPath);
				if (!std::get<0>(foundPrefab))
				{
					return nullptr;
				}

				std::shared_ptr<UIPrefabDescription> foundObjectPrefab(std::dynamic_pointer_cast<UIPrefabDescription>(std::get<0>(foundPrefab)));
				if (!foundObjectPrefab)
				{
					return nullptr;
				}

				return foundObjectPrefab->Fabricate(std::get<1>(foundPrefab), creatingManager, placeProcessor);
			}


			namespace _Internal
			{
				//------------------------------------------------------------------------------
				std::tuple<std::shared_ptr<IValueGetter>, DescriptionProcessingContext> GetValueGetterByName(DescriptionManager &manager, std::shared_ptr<DescriptionUnit> unit, const std::string &valueName)
				{
					ObjectEnvironmentPair foundValue = unit->SelectDescriptionByName(manager, valueName);
					if (!std::get<0>(foundValue))
					{
						return std::make_tuple(nullptr, DescriptionProcessingContext(manager));
					}

					std::shared_ptr<IValueGetter> foundValueGetter(std::dynamic_pointer_cast<IValueGetter>(std::get<0>(foundValue)));
					if (!foundValueGetter)
					{
						return std::make_tuple(nullptr, DescriptionProcessingContext(manager));
					}

					return std::make_tuple(foundValueGetter, std::get<1>(foundValue));
				}

				//------------------------------------------------------------------------------
				std::tuple<std::shared_ptr<IValueGetter>, DescriptionProcessingContext> GetValueGetterByPath(DescriptionManager &manager, std::shared_ptr<DescriptionUnit> unit, const std::string &valuePath)
				{
					ObjectEnvironmentPair foundValue = unit->SelectDescriptionByPath(manager, valuePath);
					if (!std::get<0>(foundValue))
					{
						return std::make_tuple(nullptr, DescriptionProcessingContext(manager));
					}

					std::shared_ptr<IValueGetter> foundValueGetter(std::dynamic_pointer_cast<IValueGetter>(std::get<0>(foundValue)));
					if (!foundValueGetter)
					{
						return std::make_tuple(nullptr, DescriptionProcessingContext(manager));
					}

					return std::make_tuple(foundValueGetter, std::get<1>(foundValue));
				}
			}


			//------------------------------------------------------------------------------
			template<> BooleanValueType DescriptionManager::ReadDescriptionValue<BooleanValueType>(std::shared_ptr<DescriptionUnit> unit, const std::string &valueName)
			{
				std::tuple<std::shared_ptr<IValueGetter>, DescriptionProcessingContext> foundValueGetter(_Internal::GetValueGetterByName(*this, unit, valueName));
				return std::get<0>(foundValueGetter)->GetValueBoolean(std::get<1>(foundValueGetter));
			}

			template<> BooleanValueType DescriptionManager::ReadDescriptionValueByPath<BooleanValueType>(std::shared_ptr<DescriptionUnit> unit, const std::string &valuePath)
			{
				std::tuple<std::shared_ptr<IValueGetter>, DescriptionProcessingContext> foundValueGetter(_Internal::GetValueGetterByPath(*this, unit, valuePath));
				return std::get<0>(foundValueGetter)->GetValueBoolean(std::get<1>(foundValueGetter));
			}

			template<> IntegerValueType DescriptionManager::ReadDescriptionValue<IntegerValueType>(std::shared_ptr<DescriptionUnit> unit, const std::string &valueName)
			{
				std::tuple<std::shared_ptr<IValueGetter>, DescriptionProcessingContext> foundValueGetter(_Internal::GetValueGetterByName(*this, unit, valueName));
				return std::get<0>(foundValueGetter)->GetValueInteger(std::get<1>(foundValueGetter));
			}

			template<> IntegerValueType DescriptionManager::ReadDescriptionValueByPath<IntegerValueType>(std::shared_ptr<DescriptionUnit> unit, const std::string &valuePath)
			{
				std::tuple<std::shared_ptr<IValueGetter>, DescriptionProcessingContext> foundValueGetter(_Internal::GetValueGetterByPath(*this, unit, valuePath));
				return std::get<0>(foundValueGetter)->GetValueInteger(std::get<1>(foundValueGetter));
			}

			template<> FloatingPointValueType DescriptionManager::ReadDescriptionValue<FloatingPointValueType>(std::shared_ptr<DescriptionUnit> unit, const std::string &valueName)
			{
				std::tuple<std::shared_ptr<IValueGetter>, DescriptionProcessingContext> foundValueGetter(_Internal::GetValueGetterByName(*this, unit, valueName));
				return std::get<0>(foundValueGetter)->GetValueFloatingPoint(std::get<1>(foundValueGetter));
			}

			template<> FloatingPointValueType DescriptionManager::ReadDescriptionValueByPath<FloatingPointValueType>(std::shared_ptr<DescriptionUnit> unit, const std::string &valuePath)
			{
				std::tuple<std::shared_ptr<IValueGetter>, DescriptionProcessingContext> foundValueGetter(_Internal::GetValueGetterByPath(*this, unit, valuePath));
				return std::get<0>(foundValueGetter)->GetValueFloatingPoint(std::get<1>(foundValueGetter));
			}

			template<> ColorARGBValueType DescriptionManager::ReadDescriptionValue<ColorARGBValueType>(std::shared_ptr<DescriptionUnit> unit, const std::string &valueName)
			{
				std::tuple<std::shared_ptr<IValueGetter>, DescriptionProcessingContext> foundValueGetter(_Internal::GetValueGetterByName(*this, unit, valueName));
				return std::get<0>(foundValueGetter)->GetValueColor(std::get<1>(foundValueGetter));
			}

			template<> ColorARGBValueType DescriptionManager::ReadDescriptionValueByPath<ColorARGBValueType>(std::shared_ptr<DescriptionUnit> unit, const std::string &valuePath)
			{
				std::tuple<std::shared_ptr<IValueGetter>, DescriptionProcessingContext> foundValueGetter(_Internal::GetValueGetterByPath(*this, unit, valuePath));
				return std::get<0>(foundValueGetter)->GetValueColor(std::get<1>(foundValueGetter));
			}

			//------------------------------------------------------------------------------
			// Reload function for debug
			void DescriptionManager::DEBUG_Reload()
			{
				m_textureManager.ClearCache();
				m_fontFaceManager.ClearCache();
				
				ClearDescriptionCache();
			}

			//------------------------------------------------------------------------------
			// Cache Clear Function to re-create UI
			void DescriptionManager::ClearDescriptionCache(std::shared_ptr<DescriptionUnit> newSystemUnit)
			{
				std::shared_ptr<DescriptionUnit> systemUnit(newSystemUnit);
				if (!systemUnit)
				{
					systemUnit = GetUnit(SystemUnit::kSystemUnitName);
					std::shared_ptr<SystemUnit> concreteSystemUnit(std::dynamic_pointer_cast<SystemUnit>(systemUnit));
					if (concreteSystemUnit)
					{
						concreteSystemUnit->ReloadSystemValues();
					}
				}
				
				m_loadingUnitNames.clear();
				m_unitCache.clear();
				if (systemUnit)
				{
					AddUnit(SystemUnit::kSystemUnitName, systemUnit);
				}
			}
		}
	}
}
