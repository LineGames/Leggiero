////////////////////////////////////////////////////////////////////////////////
// Loader/UIBundleAssetLoader.cpp (Leggiero/Modules - LegacyUI)
//
// Bundle asset loader implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIBundleAssetLoader.h"

// Standard Library
#include <sstream>

// External Library
#include <GLES3.h>

// Leggiero.FileSystem
#include <FileSystem/BundleFileResourceComponent.h>
#include <FileSystem/FileSystemUtility.h>

// Leggiero.Graphics
#include <Graphics/GraphicResourceManagerComponent.h>
#include <Graphics/Texture/TextureManager.h>
#include <Graphics/Texture/ImageFormatType.h>
#include <Graphics/Texture/TextureAtlasTable.h>

// Leggiero.Font
#include <Font/FreeType/FreeTypeFontFace.h>


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// UIBundleAssetLoader

		//------------------------------------------------------------------------------
		// Sub-path default values
		const std::string UIBundleAssetLoader::kDefaultDescriptionSubPath("Description");
		const std::string UIBundleAssetLoader::kDefaultTextureSubPath("Texture");
		const std::string UIBundleAssetLoader::kDefaultFontSubPath("Font");

		//------------------------------------------------------------------------------
		UIBundleAssetLoader::UIBundleAssetLoader(FileSystem::BundleFileResourceComponent *bundleComponent, Graphics::GraphicResourceManagerComponent *graphicResourceManagerComponent, const std::string &uiBasePath, const std::string &descriptionSubPath, const std::string &textureSubPath, const std::string &fontSubPath)
			: m_bundle(bundleComponent), m_graphicResourceManager(graphicResourceManagerComponent)
			, m_uiBasePath(uiBasePath)
			, m_descBasePath(FileSystem::Utility::CombinePathWithResolvingParent(uiBasePath, descriptionSubPath))
			, m_textureBasePath(FileSystem::Utility::CombinePathWithResolvingParent(uiBasePath, textureSubPath))
			, m_fontBasePath(FileSystem::Utility::CombinePathWithResolvingParent(uiBasePath, fontSubPath))
		{
		}

		//------------------------------------------------------------------------------
		UIBundleAssetLoader::~UIBundleAssetLoader()
		{
		}

		//------------------------------------------------------------------------------
		size_t UIBundleAssetLoader::ReadDescriptionUnitData(const std::string &unitName, std::ostream &buffer)
		{
			std::string assetPath(FileSystem::Utility::CombinePath(m_descBasePath, unitName + ".desc.xml"));

			return m_bundle->ReadBundleFileData(assetPath, 0, buffer);
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<Graphics::GLTextureResource> UIBundleAssetLoader::LoadTexture(const std::string &textureName)
		{
			std::string assetPath(FileSystem::Utility::CombinePath(m_textureBasePath, textureName));

			Graphics::ImageFormatType formatFromName = Graphics::GetImageFormatFromFilePathExtension(textureName.c_str());
			Graphics::ImageFormatType loadingFormat = formatFromName;
			if (formatFromName == Graphics::ImageFormatType::kINVALID)
			{
				// Default Assume PNG
				assetPath += ".png";
				loadingFormat = Graphics::ImageFormatType::kPNG;
			}

			std::shared_ptr<Graphics::GLTextureResource> loadedTexture(m_graphicResourceManager->GetTextureManager().LoadTextureFromImageInBundle(assetPath, loadingFormat, GL_LINEAR, GL_LINEAR));
			if (loadedTexture)
			{
				return loadedTexture;
			}

			if (formatFromName == Graphics::ImageFormatType::kINVALID)
			{
				// using no extension
				assetPath = FileSystem::Utility::CombinePath(m_textureBasePath, textureName);
				loadedTexture = m_graphicResourceManager->GetTextureManager().LoadTextureFromImageInBundle(assetPath, Graphics::ImageFormatType::kPNG, GL_LINEAR, GL_LINEAR);
				if (loadedTexture)
				{
					return loadedTexture;
				}
			}

			return nullptr;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<Graphics::TextureAtlasTable> UIBundleAssetLoader::LoadTextureAtlasTable(const std::string &textureName, std::shared_ptr<Graphics::GLTextureResource> texture)
		{
			std::string assetPath(FileSystem::Utility::CombinePath(m_textureBasePath, textureName + ".atlas.xml"));

			if (!m_bundle->IsBundleFileExists(assetPath))
			{
				std::string fileExtension = FileSystem::Utility::GetExtension(textureName);
				if (!fileExtension.empty())
				{
					assetPath = FileSystem::Utility::CombinePath(m_textureBasePath, textureName.substr(0, textureName.length() - fileExtension.length()) + ".atlas.xml");
				}
			}

			if (!m_bundle->IsBundleFileExists(assetPath))
			{
				return nullptr;
			}

			std::stringstream textureAtlasDataBuffer;
			m_bundle->ReadBundleFileData(assetPath, 0, textureAtlasDataBuffer);
			return Graphics::TextureAtlasTable::LoadFromXML(textureAtlasDataBuffer.str(), texture);
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<Font::IFontFace> UIBundleAssetLoader::LoadFontFace(const std::string &fontName)
		{
			// Load FreeType Font
			std::string assetPath(FileSystem::Utility::CombinePath(m_fontBasePath, fontName));

			if (!m_bundle->IsBundleFileExists(assetPath))
			{
				return nullptr;
			}

			if (m_bundle->NPO_IsEnable_BundleFileRealPath())
			{
				return Font::FreeType::CreateFontFaceFromFile(m_bundle->NPO_GetBundleFileRealPath(assetPath));
			}
			else
			{
				std::stringstream fontDataBuffer;
				m_bundle->ReadBundleFileData(assetPath, 0, fontDataBuffer);
				fontDataBuffer.seekg(0);
				return Font::FreeType::CreateFontFaceFromMemory(fontDataBuffer);
			}
		}
	}
}
