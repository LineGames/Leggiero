////////////////////////////////////////////////////////////////////////////////
// Loader/UIFileAssetLoader.cpp (Leggiero/Modules - LegacyUI)
//
// File asset loader implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIFileAssetLoader.h"

// Standard Library
#include <fstream>
#include <sstream>

// External Library
#include <GLES3.h>

// Leggiero.FileSystem
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
		//////////////////////////////////////////////////////////////////////////////// UIFileAssetLoader

		//------------------------------------------------------------------------------
		// Sub-path default values
		const std::string UIFileAssetLoader::kDefaultDescriptionSubPath("Description");
		const std::string UIFileAssetLoader::kDefaultTextureSubPath("Texture");
		const std::string UIFileAssetLoader::kDefaultFontSubPath("Font");

		//------------------------------------------------------------------------------
		UIFileAssetLoader::UIFileAssetLoader(Graphics::GraphicResourceManagerComponent *graphicResourceManagerComponent, const std::string &uiBasePath, const std::string &descriptionSubPath, const std::string &textureSubPath, const std::string &fontSubPath)
			: m_graphicResourceManager(graphicResourceManagerComponent)
			, m_uiBasePath(uiBasePath)
			, m_descBasePath(FileSystem::Utility::CombinePathWithResolvingParent(uiBasePath, descriptionSubPath))
			, m_textureBasePath(FileSystem::Utility::CombinePathWithResolvingParent(uiBasePath, textureSubPath))
			, m_fontBasePath(FileSystem::Utility::CombinePathWithResolvingParent(uiBasePath, fontSubPath))
		{
		}

		//------------------------------------------------------------------------------
		UIFileAssetLoader::~UIFileAssetLoader()
		{
		}

		//------------------------------------------------------------------------------
		size_t UIFileAssetLoader::ReadDescriptionUnitData(const std::string &unitName, std::ostream &buffer)
		{
			std::string assetPath(FileSystem::Utility::CombinePath(m_descBasePath, unitName + ".desc.xml"));

			std::ifstream fs(assetPath, std::ios::binary);
			if (!fs.good())
			{
				return 0;
			}

			std::ostream::pos_type beforePos = buffer.tellp();
			buffer << fs.rdbuf();
			fs.close();

			return buffer.tellp() - beforePos;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<Graphics::GLTextureResource> UIFileAssetLoader::LoadTexture(const std::string &textureName)
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

			std::shared_ptr<Graphics::GLTextureResource> loadedTexture(m_graphicResourceManager->GetTextureManager().LoadTextureFromImageInFile(assetPath, loadingFormat, GL_LINEAR, GL_LINEAR));
			if (loadedTexture)
			{
				return loadedTexture;
			}

			if (formatFromName == Graphics::ImageFormatType::kINVALID)
			{
				// using no extension
				assetPath = FileSystem::Utility::CombinePath(m_textureBasePath, textureName);
				loadedTexture = m_graphicResourceManager->GetTextureManager().LoadTextureFromImageInFile(assetPath, Graphics::ImageFormatType::kPNG, GL_LINEAR, GL_LINEAR);
				if (loadedTexture)
				{
					return loadedTexture;
				}
			}

			return nullptr;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<Graphics::TextureAtlasTable> UIFileAssetLoader::LoadTextureAtlasTable(const std::string &textureName, std::shared_ptr<Graphics::GLTextureResource> texture)
		{
			std::string assetPath(FileSystem::Utility::CombinePath(m_textureBasePath, textureName + ".atlas.xml"));

			std::ifstream fs(assetPath, std::ios::binary);
			if (!fs.good())
			{
				return nullptr;
			}

			std::stringstream textureAtlasDataBuffer;
			textureAtlasDataBuffer << fs.rdbuf();
			fs.close();

			return Graphics::TextureAtlasTable::LoadFromXML(textureAtlasDataBuffer.str(), texture);
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<Font::IFontFace> UIFileAssetLoader::LoadFontFace(const std::string &fontName)
		{
			// Load FreeType Font
			std::string assetPath(FileSystem::Utility::CombinePath(m_fontBasePath, fontName));

			return Font::FreeType::CreateFontFaceFromFile(assetPath);
		}
	}
}
