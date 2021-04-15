////////////////////////////////////////////////////////////////////////////////
// Prefab/UIObjectSliderPrefab.h (Leggiero/Modules - LegacyUI)
//
// Prefab for UI Slider Object
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__PREFAB__UI_OBJECT_SLIDER_PREFAB_H
#define __LM_LUI__PREFAB__UI_OBJECT_SLIDER_PREFAB_H


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
				// Prefab for simple slider
				class SimpleSliderPrefab
					: public UIObjectPrefab
				{
				public:
					SimpleSliderPrefab() { }
					virtual ~SimpleSliderPrefab() { }

				public:
					static std::shared_ptr<SimpleSliderPrefab> ReadFromFromXMLElement(ParsingXMLElementType *elem);

				protected:	// UIObjectPrefab
					virtual std::shared_ptr<UIObject> _CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager,
						std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor) override;

				protected:
					VariablePathType m_handlePrefabPath;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_widthExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_handlerTouchWidthRatioExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_handlerTouchHeightRatioExpression;

					std::shared_ptr<Expression::IExpression<IntegerValueType> >			m_subRulerDivideExpression;
				};
			}
		}
	}
}

#endif
