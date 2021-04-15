////////////////////////////////////////////////////////////////////////////////
// Prefab/UIOjbectButtonPrefab.h (Leggiero/Modules - LegacyUI)
//
// Prefab for UI Button Object
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__PREFAB__UI_OBJECT_BUTTON_PREFAB_H
#define __LM_LUI__PREFAB__UI_OBJECT_BUTTON_PREFAB_H


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
				// Prefab for a simple button
				class SimpleSingleButtonPrefab
					: public UIObjectPrefab
				{
				public:
					SimpleSingleButtonPrefab() { }
					virtual ~SimpleSingleButtonPrefab() { }

				public:
					static std::shared_ptr<SimpleSingleButtonPrefab> ReadFromFromXMLElement(ParsingXMLElementType *elem);

				protected:	// UIObjectPrefab
					virtual std::shared_ptr<UIObject> _CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager,
						std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor) override;

				protected:
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_widthExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_heightExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_coverWidthExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_coverHeightExpression;
				};
			}
		}
	}
}

#endif
