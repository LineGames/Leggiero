////////////////////////////////////////////////////////////////////////////////
// Rendering/UIRenderingComponent.h (Leggiero/Modules - LegacyUI)
//
// Renderable UI Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__RENDERING__UI_RENDERING_COMPONENT_H
#define __LM_LUI__RENDERING__UI_RENDERING_COMPONENT_H


// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "../Component/IUIComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UIRenderer;
		class IUITransform;
		class IUIClipping;


		// Renderable UI Component
		class UIRenderingComponent
			: public IUIComponent
		{
		public:
			UIRenderingComponent() { }
			virtual ~UIRenderingComponent() { }

		public:	// IUIComponent
			virtual UIComponentType GetComponentType() const override { return UIComponentType::kRendering; }

		public:
			virtual void Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha) = 0;

			virtual UIElementSize GetVisualSize() { return UIElementSize(); }
		};


		// Dummy rendering component: does not draw anything
		class DummyRenderingComponent
			: public UIRenderingComponent
		{
		public:
			DummyRenderingComponent() { }
			virtual ~DummyRenderingComponent() { }

		public:	// IUIComponent
			virtual std::shared_ptr<IUIComponent> Clone(const UIObject &ownerObject) const override { return std::make_shared<DummyRenderingComponent>(); }

		public:	// UIRenderingComponent
			virtual void Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha) override { }
		};
	}
}

#endif
