////////////////////////////////////////////////////////////////////////////////
// Prefab/UITextObjectPrefab.h (Leggiero/Modules - LegacyUI)
//
// Prefabs for Text UI Object
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__PREFAB__UI_TEXT_OBJECT_PREFAB_H
#define __LM_LUI__PREFAB__UI_TEXT_OBJECT_PREFAB_H


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
				// Prefab for Fixed Text Label
				class TextLabelPrefab
					: public UIObjectPrefab
				{
				public:
					TextLabelPrefab(const VariablePathType &classPath)
						: m_classPath(classPath)
						, m_blockTextAlign(Font::TextAlignType::kJustify), m_blockTextLastLineAlign(Font::TextAlignType::kLeft)
					{ }
					TextLabelPrefab(VariablePathType &&classPath)
						: m_classPath(classPath)
						, m_blockTextAlign(Font::TextAlignType::kJustify), m_blockTextLastLineAlign(Font::TextAlignType::kLeft)
					{ }

					virtual ~TextLabelPrefab() { }

				public:
					static std::shared_ptr<TextLabelPrefab> ReadFromFromXMLElement(ParsingXMLElementType *elem);

				public:
					bool IsLineBoxed() const { return (bool)m_lineWidthExpression; }

				protected:	// UIObjectPrefab
					virtual std::shared_ptr<UIObject> _CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager,
						std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor) override;

				protected:
					GameDataString _GetStringValue(DescriptionProcessingContext &processingContext, IUIPrefabPlaceProcessor *placeProcessor);

				protected:
					VariablePathType	m_classPath;

					SourcedStringEntry	m_sourceString;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_lineWidthExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_lineSpaceRatioExpression;
					Font::TextAlignType													m_blockTextAlign;
					Font::TextAlignType													m_blockTextLastLineAlign;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_sizeExpression;
					std::shared_ptr<Expression::IExpression<ColorARGBValueType> >		m_colorExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_strokeWidthExpression;
					std::shared_ptr<Expression::IExpression<ColorARGBValueType> >		m_strokeColorExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_shadowOffsetX;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_shadowOffsetY;
					std::shared_ptr<Expression::IExpression<ColorARGBValueType> >		m_shadowColorExpression;
				};


				// Prefab for UI element shows varaible text
				class TextPrintPrefab
					: public UIObjectPrefab
				{
				public:
					TextPrintPrefab(const VariablePathType &classPath, const GameDataString &placeholder)
						: m_classPath(classPath), m_placeholderString(placeholder)
						, m_blockTextAlign(Font::TextAlignType::kJustify), m_blockTextLastLineAlign(Font::TextAlignType::kLeft)
					{ }
					TextPrintPrefab(VariablePathType &&classPath, const GameDataString &placeholder)
						: m_classPath(classPath), m_placeholderString(placeholder)
						, m_blockTextAlign(Font::TextAlignType::kJustify), m_blockTextLastLineAlign(Font::TextAlignType::kLeft)
					{ }

					virtual ~TextPrintPrefab() { }

				public:
					static std::shared_ptr<TextPrintPrefab> ReadFromFromXMLElement(ParsingXMLElementType *elem);

				public:
					bool IsLineBoxed() const { return (bool)m_lineWidthExpression; }

				protected:	// UIObjectPrefab
					virtual std::shared_ptr<UIObject> _CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager,
						std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor) override;

				protected:
					VariablePathType	m_classPath;
					GameDataString		m_placeholderString;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_lineWidthExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_lineSpaceRatioExpression;
					Font::TextAlignType													m_blockTextAlign;
					Font::TextAlignType													m_blockTextLastLineAlign;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_sizeExpression;
					std::shared_ptr<Expression::IExpression<ColorARGBValueType> >		m_colorExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_strokeWidthExpression;
					std::shared_ptr<Expression::IExpression<ColorARGBValueType> >		m_strokeColorExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_shadowOffsetX;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_shadowOffsetY;
					std::shared_ptr<Expression::IExpression<ColorARGBValueType> >		m_shadowColorExpression;
				};


				// Prefab for cache optimized text
				class CachedTextPrintPrefab
					: public UIObjectPrefab
				{
				public:
					CachedTextPrintPrefab(const VariablePathType &classPath, const GameDataString &placeholder)
						: m_classPath(classPath), m_placeholderString(placeholder)
					{ }
					CachedTextPrintPrefab(VariablePathType &&classPath, const GameDataString &placeholder)
						: m_classPath(classPath), m_placeholderString(placeholder)
					{ }

					virtual ~CachedTextPrintPrefab() { }

				public:
					static std::shared_ptr<CachedTextPrintPrefab> ReadFromFromXMLElement(ParsingXMLElementType *elem);

				protected:	// UIObjectPrefab
					virtual std::shared_ptr<UIObject> _CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager,
						std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor) override;

				protected:
					VariablePathType	m_classPath;
					GameDataString		m_placeholderString;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_sizeExpression;
					std::shared_ptr<Expression::IExpression<ColorARGBValueType> >		m_colorExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_strokeWidthExpression;
					std::shared_ptr<Expression::IExpression<ColorARGBValueType> >		m_strokeColorExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_shadowOffsetX;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_shadowOffsetY;
					std::shared_ptr<Expression::IExpression<ColorARGBValueType> >		m_shadowColorExpression;
				};
			}
		}
	}
}

#endif
