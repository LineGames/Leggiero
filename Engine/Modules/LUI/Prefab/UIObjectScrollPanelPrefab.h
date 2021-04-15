////////////////////////////////////////////////////////////////////////////////
// Prefab/UIObjectScrollPanelPrefab.h (Leggiero/Modules - LegacyUI)
//
// Prefab for Scrollable Panel Object
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__PREFAB__UI_OBJECT_SCROLL_PANEL_PREFAB_H
#define __LM_LUI__PREFAB__UI_OBJECT_SCROLL_PANEL_PREFAB_H


// Standard Library
#include <memory>
#include <vector>

// Leggiero.LegacyUI
#include "UIObjectPrefab.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Prefab
			{
				// Prefab for simple vertical scroll panel
				class SimpleVerticalScrollPanelPrefab
					: public UIObjectPrefab
				{
				public:
					SimpleVerticalScrollPanelPrefab() { }
					virtual ~SimpleVerticalScrollPanelPrefab() { }

				public:
					static std::shared_ptr<SimpleVerticalScrollPanelPrefab> ReadFromFromXMLElement(ParsingXMLElementType *elem);

				protected:	// UIObjectPrefab
					virtual bool _IsObjectSpecialXMLElement(ParsingXMLElementType *elem) override;

					virtual std::shared_ptr<UIObject> _CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager,
						std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor) override;

				protected:
					std::shared_ptr<IUIObjectPrefab> contentsObjectPrefab;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_contentsHeightExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_widthExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_heightExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_moveThresholdAmountExpression;

					std::shared_ptr<Expression::IExpression<BooleanValueType> >			m_isClippingExpression;
				};


				// Prefab for simple horizontal scroll panel
				class SimpleHorizontalScrollPanelPrefab
					: public UIObjectPrefab
				{
				public:
					SimpleHorizontalScrollPanelPrefab() { }
					virtual ~SimpleHorizontalScrollPanelPrefab() { }

				public:
					static std::shared_ptr<SimpleHorizontalScrollPanelPrefab> ReadFromFromXMLElement(ParsingXMLElementType *elem);

				protected:	// UIObjectPrefab
					virtual bool _IsObjectSpecialXMLElement(ParsingXMLElementType *elem) override;

					virtual std::shared_ptr<UIObject> _CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager,
						std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor) override;

				protected:
					std::shared_ptr<IUIObjectPrefab> contentsObjectPrefab;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_contentsWidthExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_widthExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_heightExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_moveThresholdAmountExpression;

					std::shared_ptr<Expression::IExpression<BooleanValueType> >			m_isClippingExpression;
				};
			}
		}
	}
}

#endif
