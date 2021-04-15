////////////////////////////////////////////////////////////////////////////////
// Prefab/UILayoutComponentPrefab.h (Leggiero/Modules - LegacyUI)
//
// Prefab for Layout Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__PREFAB__UI_LAYOUT_COMPONENT_PREFAB_H
#define __LM_LUI__PREFAB__UI_LAYOUT_COMPONENT_PREFAB_H


// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include "../Description/UIDescriptionTypes.h"
#include "UIComponentPrefab.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UIManager;
		class UIObject;
		class IUIComponent;

		namespace Description
		{
			// Forward Declarations
			class DescriptionProcessingContext;

			namespace Expression
			{
				template <typename ValueT>
				class IExpression;
			}


			namespace Prefab
			{
				// Forward Declaration
				class IUIPrefabPlaceProcessor;


				// Prefab for Layout Component
				class LayoutComponentPrefab
					: public IUIComponentPrefab
				{
				public:
					// Size reference type for layout
					enum class SizeReferenceType
					{
						kINVALID,
						kObject,
						kObjectElement,
						kHorizontalChildren,
						kVerticalChildren,
						kParent,
					};

				public:
					LayoutComponentPrefab();
					virtual ~LayoutComponentPrefab() { }

				public:	// IUIComponentPrefab
					virtual std::shared_ptr<IUIComponent> Fabricate(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::shared_ptr<UIObject> ownerObject, IUIPrefabPlaceProcessor *placeProcessor, ComponentPostProcessCreatingContext &postProcessingContext) override;

				public:
					static std::shared_ptr<LayoutComponentPrefab> ReadFromFromXMLElement(ParsingXMLElementType *elem);

				protected:
					SizeReferenceType m_sizeReference;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_fixedWidthExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_fixedHeightExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_widthModifyAddExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_heightModifyAddExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_widthModifyMultiplyExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_heightModifyMultiplyExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_parentReferenceRatioXExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_parentReferenceRatioYExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_parentOffsetXExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_parentOffsetYExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_anchorReferenceRatioXExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_anchorReferenceRatioYExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_anchorOffsetXExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_anchorOffsetYExpression;
				};
			}
		}
	}
}

#endif
