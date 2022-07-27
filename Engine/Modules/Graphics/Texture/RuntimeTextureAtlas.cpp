////////////////////////////////////////////////////////////////////////////////
// Texture/RuntimeTextureAtlas.cpp (Leggiero/Modules - Graphics)
//
// Implementation of Runtime Texture Atlas
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "RuntimeTextureAtlas.h"

// Leggiero.Graphics
#include "DynamicTextureResource.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// Internal Types and Utility

		namespace _Internal
		{
			//-----------------------------------------------------------------------------
			struct SpaceNode
			{
			public:
				GLint leftX;
				GLint topY;
				GLsizei width;
				GLsizei height;

				RuntimeTextureAtlasEntry::EntryIdType entryId;

				std::shared_ptr<SpaceNode> leftChild;
				std::shared_ptr<SpaceNode> rightChild;

				std::shared_ptr<SpaceNode> parent;

			public:
				SpaceNode(std::shared_ptr<SpaceNode> parent, GLint x, GLint y, GLsizei width, GLsizei height, RuntimeTextureAtlasEntry::EntryIdType entryId = kNullId)
					: parent(parent), leftX(x), topY(y), width(width), height(height), entryId(entryId)
				{ }

			public:
				bool IsEmptySpace() const { return ((entryId == kNullId) && !IsSplitNote()); }
				bool IsSplitNote() const { return (leftChild || rightChild); }

			public:
				static constexpr RuntimeTextureAtlasEntry::EntryIdType kNullId = 0;
			};


			//-----------------------------------------------------------------------------
			void _InsertSortedPosition(std::vector<std::shared_ptr<_Internal::SpaceNode> > &targetVector, std::shared_ptr<_Internal::SpaceNode> toInsert)
			{
				if (targetVector.empty())
				{
					targetVector.push_back(toInsert);
				}
				else
				{
					int lowerBound = 0;
					int upperBound = (int)targetVector.size();
					int findIndex = upperBound / 2;
					while (lowerBound < upperBound)
					{
						if (targetVector[findIndex]->width < toInsert->width)
						{
							lowerBound = findIndex + 1;
							findIndex = (lowerBound + upperBound) / 2;
						}
						else if (targetVector[findIndex]->width > toInsert->width)
						{
							upperBound = findIndex - 1;
							findIndex = (lowerBound + upperBound) / 2;
						}
						else
						{
							break;
						}
					}
					if (findIndex < 0)
					{
						findIndex = 0;
					}
					if (findIndex > targetVector.size())
					{
						findIndex = (int)targetVector.size();
					}
					targetVector.insert(targetVector.begin() + findIndex, toInsert);
				}
			}
		}


		//////////////////////////////////////////////////////////////////////////////// RuntimeTextureAtlas

		//-----------------------------------------------------------------------------
		RuntimeTextureAtlas::RuntimeTextureAtlas(GLsizei textureWidth, GLsizei textureHeight, std::shared_ptr<DynamicTextureResource> texture, GLsizei padding)
			: m_texture(texture), m_width(textureWidth), m_height(textureHeight), m_padding(padding)
			, m_issueingEntryId(_Internal::SpaceNode::kNullId + 1)
			, m_spaceTreeRoot(std::make_shared<_Internal::SpaceNode>(nullptr, 0, 0, textureWidth, textureHeight))
		{
			m_emptySpaceList.push_back(m_spaceTreeRoot);
		}

		//-----------------------------------------------------------------------------
		RuntimeTextureAtlas::~RuntimeTextureAtlas()
		{
		}

		//-----------------------------------------------------------------------------
		std::shared_ptr<RuntimeTextureAtlasEntry> RuntimeTextureAtlas::AllocateSubTexture(GLsizei width, GLsizei height, RuntimeTextureAtlasEntry::RestorerType restorer)
		{
			GLsizei effectiveWidth = width + m_padding;
			GLsizei effectiveHeight = height + m_padding;

			if (effectiveWidth > m_width || effectiveHeight > m_height)
			{
				return nullptr;
			}

			auto lock = m_atlasModifyLock.Lock();
			if (!lock)
			{
				// Something Wrong
				return nullptr;
			}

			if (m_emptySpaceList.empty())
			{
				// Full
				return nullptr;
			}

			// Find Finding-start position
			int emptyCount = (int)m_emptySpaceList.size();
			int lowerBound = 0;
			int upperBound = emptyCount;
			int findIndex = upperBound / 2;
			while (lowerBound < upperBound)
			{
				if (m_emptySpaceList[findIndex]->width < effectiveWidth)
				{
					lowerBound = findIndex + 1;
					findIndex = (lowerBound + upperBound) / 2;
				}
				else
				{
					upperBound = findIndex - 1;
					findIndex = (lowerBound + upperBound) / 2;
				}
			}
			if (findIndex < 0)
			{
				findIndex = 0;
			}
			if (findIndex >= emptyCount)
			{
				// No Fit Bin
				return nullptr;
			}

			// Find Best-Fit Bin
			unsigned long long bestFitScore = std::numeric_limits<unsigned long long>::max();
			GLsizei bestSecondaryScore = std::numeric_limits<GLsizei>::max();
			std::vector<std::shared_ptr<_Internal::SpaceNode> >::iterator bestFitIt = m_emptySpaceList.end();
			for (size_t i = (size_t)findIndex; i < emptyCount; ++i)
			{
				const std::shared_ptr<_Internal::SpaceNode> &pNode = m_emptySpaceList[i];
				if (pNode->height < effectiveHeight || pNode->width < effectiveWidth)
				{
					continue;
				}
				unsigned long long fitScore = static_cast<unsigned long long>(pNode->width - effectiveWidth) * static_cast<unsigned long long>(pNode->height - effectiveHeight);
				if (fitScore == 0)
				{
					GLsizei secondaryScore = 0;
					if (pNode->width - effectiveWidth == 0)
					{
						secondaryScore = (GLsizei)(pNode->height - effectiveHeight);
					}
					else
					{
						secondaryScore = (GLsizei)(pNode->width - effectiveWidth);
					}
					if (secondaryScore == 0)
					{
						bestFitIt = m_emptySpaceList.begin() + i;
						break;
					}
					if (bestFitScore == 0)
					{
						if (secondaryScore < bestSecondaryScore)
						{
							bestFitIt = m_emptySpaceList.begin() + i;
							bestSecondaryScore = secondaryScore;
						}
					}
				}
				if (fitScore < bestFitScore)
				{
					bestFitIt = m_emptySpaceList.begin() + i;
					bestFitScore = fitScore;
				}
			}
			if (bestFitIt == m_emptySpaceList.end())
			{
				// No Fit Bin
				return nullptr;
			}

			std::shared_ptr<_Internal::SpaceNode> fitNode = *bestFitIt;
			m_emptySpaceList.erase(bestFitIt);

			// Split Bin
			RuntimeTextureAtlasEntry::EntryIdType issuedId = m_issueingEntryId.fetch_add(1);
			if ((fitNode->width - effectiveWidth > 0) && (fitNode->height - effectiveHeight > 0))
			{
				if ((fitNode->width - effectiveWidth > 0) > (fitNode->height - effectiveHeight > 0))
				{
					// Horizontal Split
					fitNode->rightChild = std::make_shared<_Internal::SpaceNode>(fitNode, fitNode->leftX + effectiveWidth, fitNode->topY, fitNode->width - effectiveWidth, fitNode->height);
					fitNode->leftChild = std::make_shared<_Internal::SpaceNode>(fitNode, fitNode->leftX, fitNode->topY, effectiveWidth, fitNode->height);
					fitNode->leftChild->leftChild = std::make_shared<_Internal::SpaceNode>(fitNode, fitNode->leftX, fitNode->topY, effectiveWidth, effectiveHeight, issuedId);
					fitNode->leftChild->rightChild = std::make_shared<_Internal::SpaceNode>(fitNode, fitNode->leftX, fitNode->topY + effectiveHeight, effectiveWidth, fitNode->height - effectiveHeight);
				}
				else
				{
					// Vertical Split
					fitNode->rightChild = std::make_shared<_Internal::SpaceNode>(fitNode, fitNode->leftX, fitNode->topY + effectiveHeight, fitNode->width, fitNode->height - effectiveHeight);
					fitNode->leftChild = std::make_shared<_Internal::SpaceNode>(fitNode, fitNode->leftX, fitNode->topY, fitNode->width, effectiveHeight);
					fitNode->leftChild->leftChild = std::make_shared<_Internal::SpaceNode>(fitNode, fitNode->leftX, fitNode->topY, effectiveWidth, effectiveHeight, issuedId);
					fitNode->leftChild->rightChild = std::make_shared<_Internal::SpaceNode>(fitNode, fitNode->leftX + effectiveWidth, fitNode->topY, fitNode->width - effectiveWidth, effectiveHeight);
				}
				_Internal::_InsertSortedPosition(m_emptySpaceList, fitNode->rightChild);
				_Internal::_InsertSortedPosition(m_emptySpaceList, fitNode->leftChild->rightChild);
			}
			else if ((fitNode->width - effectiveWidth > 0) || (fitNode->height - effectiveHeight > 0))
			{
				if (fitNode->width - effectiveWidth > 0)
				{
					// Horizontal Split
					fitNode->leftChild = std::make_shared<_Internal::SpaceNode>(fitNode, fitNode->leftX, fitNode->topY, effectiveWidth, effectiveHeight, issuedId);
					fitNode->rightChild = std::make_shared<_Internal::SpaceNode>(fitNode, fitNode->leftX + effectiveWidth, fitNode->topY, fitNode->width - effectiveWidth, effectiveHeight);
				}
				else
				{
					// Vertical Split
					fitNode->leftChild = std::make_shared<_Internal::SpaceNode>(fitNode, fitNode->leftX, fitNode->topY, effectiveWidth, effectiveHeight, issuedId);
					fitNode->rightChild = std::make_shared<_Internal::SpaceNode>(fitNode, fitNode->leftX, fitNode->topY + effectiveHeight, effectiveWidth, fitNode->height - effectiveHeight);
				}
				_Internal::_InsertSortedPosition(m_emptySpaceList, fitNode->rightChild);
			}
			else
			{
				// Exact Fit
				fitNode->entryId = issuedId;
			}

			std::shared_ptr<RuntimeTextureAtlasEntry> createdEntry(new RuntimeTextureAtlasEntry(shared_from_this(), issuedId,
				TextureRectSection((int)fitNode->leftX + m_padding / 2, (int)fitNode->topY + m_padding / 2, width, height, m_width, m_height),
				restorer));
			m_createdEntries.push_back(createdEntry);

			return createdEntry;
		}

		//-----------------------------------------------------------------------------
		std::shared_ptr<RuntimeTextureAtlasEntry> RuntimeTextureAtlas::InsertSubTexture(const void *imageData, GLsizei width, GLsizei height, GLenum format, GLenum dataType, RuntimeTextureAtlasEntry::RestorerType restorer)
		{
			std::shared_ptr<RuntimeTextureAtlasEntry> allocatedSpace = AllocateSubTexture(width, height, restorer);
			if (!allocatedSpace)
			{
				return nullptr;
			}

			const TextureRectSection &allocatedSection = allocatedSpace->GetTextureSection();
			glBindTexture(GL_TEXTURE_2D, m_texture->GetTextureInfo().name);
			glTexSubImage2D(GL_TEXTURE_2D, m_texture->GetMipmapLevel(), allocatedSection.GetPixelLeft(), allocatedSection.GetPixelTop(), width, height, format, dataType, imageData);
            #ifdef _LEGGIERO_IOS
                glFlush();
            #else
                glFinish();
            #endif
			return allocatedSpace;
		}

		//-----------------------------------------------------------------------------
		void RuntimeTextureAtlas::_NotifyEntryDelete(RuntimeTextureAtlasEntry::EntryIdType entryId)
		{
			auto lock = m_atlasModifyLock.Lock();
			if (!lock)
			{
				// Something Wrong
				return;
			}

			std::map<RuntimeTextureAtlasEntry::EntryIdType, std::shared_ptr<_Internal::SpaceNode> >::iterator findIt = m_idEntryLookupTable.find(entryId);
			if (findIt == m_idEntryLookupTable.end())
			{
				return;
			}

			std::shared_ptr<_Internal::SpaceNode> currentNode = findIt->second;
			m_idEntryLookupTable.erase(findIt);
			currentNode->entryId = _Internal::SpaceNode::kNullId;

			std::shared_ptr<_Internal::SpaceNode> lastEmpty = currentNode;
			currentNode = currentNode->parent;
			while (currentNode)
			{
				if (currentNode->leftChild->IsEmptySpace() && currentNode->rightChild->IsEmptySpace())
				{
					currentNode->leftChild->parent = nullptr;
					currentNode->leftChild = nullptr;
					currentNode->rightChild->parent = nullptr;
					currentNode->rightChild = nullptr;
					lastEmpty = currentNode;
				}
				else
				{
					break;
				}
				currentNode = currentNode->parent;
			}

			m_emptySpaceList.erase(std::remove_if(m_emptySpaceList.begin(), m_emptySpaceList.end(), [](const std::shared_ptr<_Internal::SpaceNode> &x) {
				return (x->parent && (x->parent->IsEmptySpace()));
				}), m_emptySpaceList.end());

			_Internal::_InsertSortedPosition(m_emptySpaceList, lastEmpty);
		}

		//-----------------------------------------------------------------------------
		void RuntimeTextureAtlas::_RestoreTexture()
		{
			if (!m_texture->RestoreSlient())
			{
				// Texture Restore Failed
				return;
			}

			std::vector<std::shared_ptr<RuntimeTextureAtlasEntry> > restoreList;
			restoreList.reserve(m_createdEntries.size());

			{
				auto lock = m_atlasModifyLock.Lock();	// Try to lock, but anyway go

				m_createdEntries.erase(std::remove_if(m_createdEntries.begin(), m_createdEntries.end(), [](const std::weak_ptr<RuntimeTextureAtlasEntry> &x) {
					return x.expired();
					}), m_createdEntries.end());
				for (const std::weak_ptr<RuntimeTextureAtlasEntry> &currentEntryPointer : m_createdEntries)
				{
					std::shared_ptr<RuntimeTextureAtlasEntry> lockedPointer(currentEntryPointer.lock());
					if (lockedPointer)
					{
						restoreList.push_back(lockedPointer);
					}
				}
			}

			for (const std::shared_ptr<RuntimeTextureAtlasEntry> &restoringEntry : restoreList)
			{
				restoringEntry->_Invalidate();
				restoringEntry->_TryRestore();
			}
		}
	}
}
