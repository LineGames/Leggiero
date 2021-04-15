////////////////////////////////////////////////////////////////////////////////
// Prefab/UITextureRenderingComponentPrefab.h (Leggiero/Modules - LegacyUI)
//
// Prefab for Texture Rendering Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__PREFAB__UI_TEXTURE_RENDERING_COMPONENT_PREFAB_H
#define __LM_LUI__PREFAB__UI_TEXTURE_RENDERING_COMPONENT_PREFAB_H


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


				// Prefab for Texture Rendering Component
				class TextureRenderingComponentPrefab
					: public IUIComponentPrefab
				{
				public:
					virtual ~TextureRenderingComponentPrefab() { }

				public:	// IUIComponentPrefab
					virtual std::shared_ptr<IUIComponent> Fabricate(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::shared_ptr<UIObject> ownerObject, IUIPrefabPlaceProcessor *placeProcessor, ComponentPostProcessCreatingContext &postProcessingContext) override;

				public:
					static std::shared_ptr<TextureRenderingComponentPrefab> ReadFromFromXMLElement(ParsingXMLElementType *elem);

				protected:
					VariablePathType m_texturePath;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_widthExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_heightExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_scaleXExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_scaleYExpression;

					std::shared_ptr<Expression::IExpression<ColorARGBValueType> > m_colorExpression;
				};
			}
		}
	}
}

#endif
