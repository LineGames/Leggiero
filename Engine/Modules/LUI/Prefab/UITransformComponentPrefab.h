////////////////////////////////////////////////////////////////////////////////
// Prefab/UITransformComponentPrefab.h (Leggiero/Modules - LegacyUI)
//
// Prefab for Transform Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__PREFAB__UI_TRANSFORM_COMPONENT_PREFAB_H
#define __LM_LUI__PREFAB__UI_TRANSFORM_COMPONENT_PREFAB_H


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


				// Prefab for Transform Component
				class TransformComponentPrefab
					: public IUIComponentPrefab
				{
				public:
					virtual ~TransformComponentPrefab() { }

				public:	// IUIComponentPrefab
					virtual std::shared_ptr<IUIComponent> Fabricate(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::shared_ptr<UIObject> ownerObject, IUIPrefabPlaceProcessor *placeProcessor, ComponentPostProcessCreatingContext &postProcessingContext) override;

				public:
					static std::shared_ptr<TransformComponentPrefab> ReadFromFromXMLElement(ParsingXMLElementType *elem);

				protected:
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_translateXExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_translateYExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_anchorXExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_anchorYExpression;

					bool m_isRotateDegree;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_rotateExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_scaleXExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_scaleYExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_skewXExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_skewYExpression;

					bool m_isPerspectiveXFocus;
					bool m_isPerspectiveYFocus;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_perspectiveXExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_perspectiveYExpression;
				};
			}
		}
	}
}

#endif
