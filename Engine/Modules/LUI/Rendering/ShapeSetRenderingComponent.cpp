////////////////////////////////////////////////////////////////////////////////
// Rendering/ShapeSetRenderingComponent.cpp (Leggiero/Modules - LegacyUI)
//
// Shape Set Rendering Component Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ShapeSetRenderingComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Rendering
		{
			//////////////////////////////////////////////////////////////////////////////// ShapeSetRenderingComponent

			//------------------------------------------------------------------------------
			ShapeSetRenderingComponent::ShapeSetRenderingComponent()
			{
			}

			//------------------------------------------------------------------------------
			ShapeSetRenderingComponent::ShapeSetRenderingComponent(std::shared_ptr<IUIRenderingShape> singleShape)
			{
				m_shapes.push_back(singleShape);
			}

			//------------------------------------------------------------------------------
			ShapeSetRenderingComponent::ShapeSetRenderingComponent(std::initializer_list<std::shared_ptr<IUIRenderingShape> > initializer)
				: m_shapes(initializer)
			{
			}

			//------------------------------------------------------------------------------
			ShapeSetRenderingComponent::~ShapeSetRenderingComponent()
			{

			}

			//------------------------------------------------------------------------------
			std::shared_ptr<IUIComponent> ShapeSetRenderingComponent::Clone(const UIObject &ownerObject) const
			{
				// Copy references because this component hiding its shapes
				return std::make_shared<ShapeSetRenderingComponent>(m_shapes.begin(), m_shapes.end());
			}

			//------------------------------------------------------------------------------
			void ShapeSetRenderingComponent::Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha)
			{
				for (std::shared_ptr<IUIRenderingShape> &currentShape : m_shapes)
				{
					currentShape->Render(renderer, effectiveTransform, effectiveClipping, alpha);
				}
			}

			//------------------------------------------------------------------------------
			UIElementSize ShapeSetRenderingComponent::GetVisualSize()
			{
				UICoordinateType left = static_cast<UICoordinateType>(0.0f);
				UICoordinateType top = static_cast<UICoordinateType>(0.0f);
				UICoordinateType right = static_cast<UICoordinateType>(0.0f);
				UICoordinateType bottom = static_cast<UICoordinateType>(0.0f);

				for (std::shared_ptr<IUIRenderingShape> &currentShape : m_shapes)
				{
					UISimpleRectangular shapeRect = currentShape->GetBoundingBox();
					if (left > shapeRect.left)
					{
						left = shapeRect.left;
					}
					if (top > shapeRect.top)
					{
						top = shapeRect.top;
					}
					if (right < shapeRect.right)
					{
						right = shapeRect.right;
					}
					if (bottom < shapeRect.bottom)
					{
						bottom = shapeRect.bottom;
					}
				}

				return UIElementSize(right - left, bottom - top);
			}

			//------------------------------------------------------------------------------
			void ShapeSetRenderingComponent::AddShape(std::shared_ptr<IUIRenderingShape> shape)
			{
				if (!shape)
				{
					return;
				}
				for (std::shared_ptr<IUIRenderingShape> &currentShape : m_shapes)
				{
					if (currentShape == shape)
					{
						return;
					}
				}
				m_shapes.push_back(shape);
			}

			//------------------------------------------------------------------------------
			void ShapeSetRenderingComponent::RemoveShape(std::shared_ptr<IUIRenderingShape> shape)
			{
				if (!shape)
				{
					return;
				}
				for (std::vector<std::shared_ptr<IUIRenderingShape> >::iterator it = m_shapes.begin(); it != m_shapes.end(); ++it)
				{
					if ((*it) == shape)
					{
						m_shapes.erase(it);
						return;
					}
				}
			}
		}
	}
}
