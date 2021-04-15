////////////////////////////////////////////////////////////////////////////////
// Prefab/UIShapeRenderingComponentPrefab.h (Leggiero/Modules - LegacyUI)
//
// Prefab for Basic Shape Rendering Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__PREFAB__UI_SHAPE_RENDERING_COMPONENT_PREFAB_H
#define __LM_LUI__PREFAB__UI_SHAPE_RENDERING_COMPONENT_PREFAB_H


// Standard Library
#include <memory>
#include <vector>

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
		class IUIRenderingShape;

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
				class IRenderingShapePrefab;


				// Prefab for Shape Set Rendering Component
				class ShapeSetRenderingComponentPrefab
					: public IUIComponentPrefab
				{
				public:
					virtual ~ShapeSetRenderingComponentPrefab() { }

				public:	// IUIComponentPrefab
					virtual std::shared_ptr<IUIComponent> Fabricate(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::shared_ptr<UIObject> ownerObject, IUIPrefabPlaceProcessor *placeProcessor, ComponentPostProcessCreatingContext &postProcessingContext) override;

				public:
					static std::shared_ptr<ShapeSetRenderingComponentPrefab> ReadFromFromXMLElement(ParsingXMLElementType *elem);

				protected:
					std::vector<std::shared_ptr<IRenderingShapePrefab> > m_shapePrefabs;
				};
			}
		}
	}
}

#endif
