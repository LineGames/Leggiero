////////////////////////////////////////////////////////////////////////////////
// Rendering/ShapeSetRenderingComponent.h (Leggiero/Modules - LegacyUI)
//
// UI rendering component to render a set of shapes
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__RENDERING__SHAPE_SET_RENDERING_COMPONENT_H
#define __LM_LUI__RENDERING__SHAPE_SET_RENDERING_COMPONENT_H


// Standard Library
#include <memory>
#include <utility>
#include <vector>

// Leggiero.LegacyUI
#include "UIRenderingComponent.h"
#include "IUIRenderingShape.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Rendering
		{
			// Shape Set Rendering Component
			class ShapeSetRenderingComponent
				: public UIRenderingComponent
			{
			public:
				ShapeSetRenderingComponent();
				ShapeSetRenderingComponent(std::shared_ptr<IUIRenderingShape> singleShape);
				ShapeSetRenderingComponent(std::initializer_list<std::shared_ptr<IUIRenderingShape> > initializer);

				template <typename InputIterator>
				ShapeSetRenderingComponent(InputIterator first, InputIterator last)
					: m_shapes(first, last)
				{
				}

				virtual ~ShapeSetRenderingComponent();

			public:	// IUIComponent
				virtual std::shared_ptr<IUIComponent> Clone(const UIObject &ownerObject) const override;

			public:	// UIRenderingComponent
				virtual void Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha) override;

				virtual UIElementSize GetVisualSize() override;

			public:
				void AddShape(std::shared_ptr<IUIRenderingShape> shape);
				void RemoveShape(std::shared_ptr<IUIRenderingShape> shape);

				std::vector<std::shared_ptr<IUIRenderingShape> > GetShapes() { return std::vector<std::shared_ptr<IUIRenderingShape> >(m_shapes); }

			protected:
				std::vector<std::shared_ptr<IUIRenderingShape> > m_shapes;
			};
		}
	}
}

#endif
