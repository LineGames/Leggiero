////////////////////////////////////////////////////////////////////////////////
// Prefab/UITextInputBoxPrefab.h (Leggiero/Modules - LegacyUI)
//
// Prefab for Text Input Box UI Object
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__PREFAB__UI_TEXT_INPUT_BOX_PREFAB_H
#define __LM_LUI__PREFAB__UI_TEXT_INPUT_BOX_PREFAB_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.Font
#include <Font/Common/Typeset.h>

// Leggiero.LegacyUI
#include "../Description/UIDescriptionSourcedString.h"
#include "UIObjectPrefab.h"


namespace Leggiero
{
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


			namespace Prefab
			{
				// Prefab for Text Input Box
				class TextInputBoxPrefab
					: public UIObjectPrefab
				{
				public:
					TextInputBoxPrefab(const VariablePathType &classPath)
						: m_textClassPath(classPath)
						, m_textBlockTextAlign(Font::TextAlignType::kJustify), m_textBlockTextLastLineAlign(Font::TextAlignType::kLeft)
					{ }
					TextInputBoxPrefab(VariablePathType &&classPath)
						: m_textClassPath(classPath)
						, m_textBlockTextAlign(Font::TextAlignType::kJustify), m_textBlockTextLastLineAlign(Font::TextAlignType::kLeft)
					{ }

					virtual ~TextInputBoxPrefab() { }

				public:
					static std::shared_ptr<TextInputBoxPrefab> ReadFromFromXMLElement(ParsingXMLElementType *elem);

				public:
					bool IsTextLineBoxed() const { return (bool)m_textLineWidthExpression; }

				protected:	// UIObjectPrefab
					virtual std::shared_ptr<UIObject> _CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager,
						std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor) override;

				protected:
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_widthExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_heightExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_clippingRectLeftExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_clippingRectTopExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_clippingRectWidthExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_clippingRectHeightExpression;

					VariablePathType													m_textClassPath;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_textLineWidthExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_textLineSpaceRatioExpression;
					Font::TextAlignType													m_textBlockTextAlign;
					Font::TextAlignType													m_textBlockTextLastLineAlign;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_textSizeExpression;
					std::shared_ptr<Expression::IExpression<ColorARGBValueType> >		m_textColorExpression;
					std::shared_ptr<Expression::IExpression<ColorARGBValueType> >		m_textMultiplyColorExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_textStrokeWidthExpression;
					std::shared_ptr<Expression::IExpression<ColorARGBValueType> >		m_textStrokeColorExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_textSizeWidthModifyAddExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_textSizeHeightModifyAddExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_textSizeWidthModifyMultiplyExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_textSizeHeightModifyMultiplyExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_textLayoutParentReferenceRatioXExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_textLayoutParentReferenceRatioYExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_textLayoutParentOffsetXExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_textLayoutParentOffsetYExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_textLayoutAnchorReferenceRatioXExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_textLayoutAnchorReferenceRatioYExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_textLayoutAnchorOffsetXExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_textLayoutAnchorOffsetYExpression;

					SourcedStringEntry m_dialogTitleString;
					SourcedStringEntry m_dialogMessageString;
					SourcedStringEntry m_dialogAcceptButtonString;
					SourcedStringEntry m_dialogCancelButtonString;
				};
			}
		}
	}
}

#endif
