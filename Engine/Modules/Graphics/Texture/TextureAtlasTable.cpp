////////////////////////////////////////////////////////////////////////////////
// Texture/TextureAtlasTable.cpp (Leggiero/Modules - Graphics)
//
// Simple Texture Atlas Table Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "TextureAtlasTable.h"

// External Library
#include <tinyxml2/tinyxml2.h>

// Leggiero.Graphics
#include "GLTextureResource.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// TextureAtlasTable

		//------------------------------------------------------------------------------
		std::shared_ptr<TextureRectSection> TextureAtlasTable::GetTextureSection(const GameDataString &sectionName) const
		{
			std::unordered_map<GameDataString, std::shared_ptr<TextureRectSection> >::const_iterator findIt = atlasTextureSectionTable.find(sectionName);
			if (findIt != atlasTextureSectionTable.cend())
			{
				return findIt->second;
			}
			return nullptr;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<TextureRectSection> TextureAtlasTable::GetTextureSection(const char *sectionName, size_t nameLength) const
		{
			if (nameLength == std::numeric_limits<size_t>::max())
			{
				nameLength = strlen(sectionName);
			}
			GameDataString sectionNameString(sectionName, nameLength);
			return GetTextureSection(sectionNameString);
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<TextureAtlasTable> TextureAtlasTable::LoadFromXML(std::string xmlData, std::shared_ptr<GLTextureResource> texture)
		{
			tinyxml2::XMLDocument dataDoc;
			if (dataDoc.Parse(xmlData.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
			{
				return nullptr;
			}
			tinyxml2::XMLElement *rootElem = dataDoc.RootElement();
			if (rootElem == NULL)
			{
				return nullptr;
			}

			try
			{
				std::shared_ptr<TextureAtlasTable> createdTable = std::make_shared<TextureAtlasTable>();

				bool isTextureGiven = (bool)(texture);
				int textureWidth = 0;
				int textureHeight = 0;
				if (!isTextureGiven)
				{
					if (rootElem->Attribute("texture_width") != nullptr && rootElem->Attribute("texture_height") != nullptr)
					{
						textureWidth = atoi(rootElem->Attribute("texture_width"));
						textureHeight = atoi(rootElem->Attribute("texture_height"));
					}
					else if (rootElem->Attribute("width") != nullptr && rootElem->Attribute("height") != nullptr)
					{
						textureWidth = atoi(rootElem->Attribute("width"));
						textureHeight = atoi(rootElem->Attribute("height"));
					}
					else
					{
						// Texture Size Not Specified
						return nullptr;
					}
				}

				float scaleFactor = 1.0f;
				if (rootElem->Attribute("scale_factor") != nullptr)
				{
					scaleFactor = (float)atof(rootElem->Attribute("scale_factor"));
				}

				// Load Sections
				if (rootElem->FirstChildElement("Section") != nullptr)
				{
					for (tinyxml2::XMLElement *currentSectionNode = rootElem->FirstChildElement("Section"); currentSectionNode != nullptr; currentSectionNode = currentSectionNode->NextSiblingElement("Section"))
					{
						GameDataString sectionName(currentSectionNode->Attribute("name"));

						std::shared_ptr<TextureRectSection> creatingSection;
						if (isTextureGiven)
						{
							creatingSection = std::make_shared<TextureRectSection>(
								atoi(currentSectionNode->Attribute("left")), atoi(currentSectionNode->Attribute("top")),
								atoi(currentSectionNode->Attribute("width")), atoi(currentSectionNode->Attribute("height")),
								texture, scaleFactor);
						}
						else
						{
							creatingSection = std::make_shared<TextureRectSection>(
								atoi(currentSectionNode->Attribute("left")), atoi(currentSectionNode->Attribute("top")),
								atoi(currentSectionNode->Attribute("width")), atoi(currentSectionNode->Attribute("height")),
								textureWidth, textureHeight, scaleFactor);
						}
						createdTable->atlasTextureSectionTable[sectionName] = creatingSection;
					}
				}
				else
				{
					for (tinyxml2::XMLElement *currentSectionNode = rootElem->FirstChildElement("sprite"); currentSectionNode != nullptr; currentSectionNode = currentSectionNode->NextSiblingElement("sprite"))
					{
						std::string currentName(currentSectionNode->Attribute("n"));
						if (currentName.length() > 4)
						{
							// Trim 3-char length extension
							if (currentName[currentName.length() - 4] == '.')
							{
								currentName[currentName.length() - 4] = '\0';
								currentName = currentName.c_str();
							}
						}
						GameDataString sectionName(currentName.c_str());

						std::shared_ptr<TextureRectSection> creatingSection;
						if (isTextureGiven)
						{
							creatingSection = std::make_shared<TextureRectSection>(
								atoi(currentSectionNode->Attribute("x")), atoi(currentSectionNode->Attribute("y")),
								atoi(currentSectionNode->Attribute("w")), atoi(currentSectionNode->Attribute("h")),
								texture, scaleFactor);
						}
						else
						{
							creatingSection = std::make_shared<TextureRectSection>(
								atoi(currentSectionNode->Attribute("x")), atoi(currentSectionNode->Attribute("y")),
								atoi(currentSectionNode->Attribute("w")), atoi(currentSectionNode->Attribute("h")),
								textureWidth, textureHeight, scaleFactor);
						}
						createdTable->atlasTextureSectionTable[sectionName] = creatingSection;
					}
				}

				return createdTable;
			}
			catch (...)
			{
				// Invalid Data
			}

			return nullptr;
		}
	}
}
