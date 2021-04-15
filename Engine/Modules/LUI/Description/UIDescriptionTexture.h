////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionTexture.h (Leggiero/Modules - LegacyUI)
//
// UI Description about Texture
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__DESCRIPTION__UI_DESCRIPTION_TEXTURE_H
#define __LM_LUI__DESCRIPTION__UI_DESCRIPTION_TEXTURE_H


// Standard Library
#include <memory>
#include <tuple>
#include <vector>
#include <unordered_map>

// Leggiero.LegacyUI
#include "UIDescriptionTypes.h"
#include "UIDescription.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Graphics
	{
		class GLTextureResource;
	}


	namespace LUI
	{
		namespace Description
		{
			// Forward Declaration
			namespace Expression
			{
				template <typename ValueT>
				class IExpression;
			}


			// Interface for description of texture segment in UI system
			class ITextureSegmentDescription
			{
			public:
				virtual UIRenderTextureSection GetTextureSegment(DescriptionProcessingContext &processingContext) = 0;
			};


			// Texture Segment
			class TextureSegment
				: public IDescriptionObject
				, public ITextureSegmentDescription
			{
			public:
				TextureSegment(const UIRenderTextureSection &textureSegment)
					: m_textureSegment(textureSegment)
				{ }

				virtual ~TextureSegment() { }

			public:	// DescriptionObject
				virtual UIDescriptionObjectType GetType() const override { return UIDescriptionObjectType::kTextureSegment; }

			public:	// ITextureSegmentDescription
				virtual UIRenderTextureSection GetTextureSegment(DescriptionProcessingContext &processingContext) override { return m_textureSegment; }

			protected:
				UIRenderTextureSection m_textureSegment;
			};


			// Texture Importing
			class ImportedTexture
				: public IDescriptionObject
				, public IDescriptionNameSpaceContext
				, public ITextureSegmentDescription
			{
			public:
				ImportedTexture(const UITextureNameType &importedTextureName)
					: m_textureName(importedTextureName)
				{ }

				virtual ~ImportedTexture() { }

			public:	// DescriptionObject
				virtual UIDescriptionObjectType GetType() const override { return UIDescriptionObjectType::kTextureSource; }

			public:	// IDescriptionNameSpaceContext
				virtual ObjectEnvironmentPair Select(const VariableNameType &name, DescriptionProcessingContext &processingContext) override;

			public:	// ITextureSegmentDescription
				virtual UIRenderTextureSection GetTextureSegment(DescriptionProcessingContext &processingContext) override;

			public:
				std::shared_ptr<UICachedTexture> GetTexture(DescriptionProcessingContext &processingContext);

			protected:
				UITextureNameType	m_textureName;

				std::shared_ptr<UICachedTexture>										m_cachedTexture;
				std::unordered_map<VariableNameType, std::shared_ptr<TextureSegment> >	m_cachedSearchTable;
			};


			// Sub-section of a Texture
			class SubTextureSegment
				: public IDescriptionObject
				, public ITextureSegmentDescription
			{
			public:
				SubTextureSegment(const VariablePathType &texturePath);
				virtual ~SubTextureSegment() { }

			public:	// DescriptionObject
				virtual UIDescriptionObjectType GetType() const override { return UIDescriptionObjectType::kTextureSegment; }

			public:	// ITextureSegmentDescription
				virtual UIRenderTextureSection GetTextureSegment(DescriptionProcessingContext &processingContext) override;

			public:
				static std::shared_ptr<SubTextureSegment> ReadFromFromXMLElement(ParsingXMLElementType *elem);

			protected:
				VariablePathType m_texturePath;

				std::shared_ptr<Expression::IExpression<IntegerValueType> > m_startXExpression;
				std::shared_ptr<Expression::IExpression<IntegerValueType> > m_startYExpression;

				std::shared_ptr<Expression::IExpression<IntegerValueType> > m_widthExpression;
				std::shared_ptr<Expression::IExpression<IntegerValueType> > m_heightExpression;

				std::shared_ptr<UIRenderTextureSection> m_cachedTexture;
			};


			// Sub-section of a texture sliced for some (axis parallel) direction
			class SlicedTexture
				: public IDescriptionObject
				, public IDescriptionNameSpaceContext
			{
			public:
				SlicedTexture(const VariablePathType &texturePath, bool isVertical);
				virtual ~SlicedTexture() { }

			public:	// DescriptionObject
				virtual UIDescriptionObjectType GetType() const override { return UIDescriptionObjectType::kTextureSource; }

			public:	// IDescriptionNameSpaceContext
				virtual ObjectEnvironmentPair Select(const VariableNameType &name, DescriptionProcessingContext &processingContext) override;

			public:
				static std::shared_ptr<SlicedTexture> ReadFromFromXMLElement(ParsingXMLElementType *elem);

			protected:
				int _GetSlicePosition(size_t index, DescriptionProcessingContext &processingContext);

			protected:
				VariablePathType m_texturePath;

				bool m_isVertical;
				std::vector<std::shared_ptr<Expression::IExpression<IntegerValueType> > > m_slicePositions;

				std::shared_ptr<UIRenderTextureSection> m_cachedTexture;
				std::unordered_map<VariableNameType, std::shared_ptr<TextureSegment> >	m_cachedSearchTable;
				std::unordered_map<size_t, int> m_cachedCalculatedSlicePositions;
			};


			// 3-Patch Texture Class
			class ThreePatchTextureClass
				: public IDescriptionObject
			{
			public:
				ThreePatchTextureClass(bool isVertical);
				virtual ~ThreePatchTextureClass() { }

			public:	// DescriptionObject
				virtual UIDescriptionObjectType GetType() const override { return m_isVertical ? UIDescriptionObjectType::kThreePatchVClass : UIDescriptionObjectType::kThreePatchHClass; }

			public:
				static std::shared_ptr<ThreePatchTextureClass> ReadFromFromXMLElement(ParsingXMLElementType *elem, UIDescriptionObjectType nodeDefinedClassType);

			public:
				std::vector<std::tuple<UIRenderTextureSection, bool, bool> > &GetPatchDescription(DescriptionProcessingContext &processingContext);

			protected:
				UIRenderTextureSection _FindTextureSection(DescriptionProcessingContext &processingContext, VariablePathType &path);

			protected:
				bool m_isVertical;

				bool m_hasStartTexture;
				VariablePathType m_startTexturePath;

				VariablePathType m_middleTexturePath;

				bool m_hasFinishTexture;
				VariablePathType m_finishTexturePath;

				std::vector<std::tuple<UIRenderTextureSection, bool, bool> > m_cachedDescription;
			};


			// 9-Patch Texture Class
			class NinePatchTextureClass
				: public IDescriptionObject
			{
			public:
				NinePatchTextureClass() { }
				virtual ~NinePatchTextureClass() { }

			public:	// DescriptionObject
				virtual UIDescriptionObjectType GetType() const override { return UIDescriptionObjectType::kNinePatchClass; }

			public:
				static std::shared_ptr<NinePatchTextureClass> ReadFromFromXMLElement(ParsingXMLElementType *elem);

			public:
				std::vector<std::tuple<UIRenderTextureSection, bool, bool> > &GetPatchDescription(DescriptionProcessingContext &processingContext);

				UICoordinateRatioType GetPatchScaleX(DescriptionProcessingContext &processingContext);
				UICoordinateRatioType GetPatchScaleY(DescriptionProcessingContext &processingContext);

			protected:
				bool _HasRealTexture(size_t patchIndex) const;
				UIRenderTextureSection _GetTexture(DescriptionProcessingContext &processingContext, size_t index);
				UIRenderTextureSection _FindTextureSection(DescriptionProcessingContext &processingContext, VariablePathType &path);

			protected:
				std::unordered_map<size_t, VariablePathType> m_texturePathTable;

				std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_patchScaleXExpression;
				std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_patchScaleYExpression;

				std::vector<std::tuple<UIRenderTextureSection, bool, bool> >	m_cachedDescription;
				std::unordered_map<size_t, UIRenderTextureSection>				m_cachedRealTexture;
			};
		}
	}
}

#endif
