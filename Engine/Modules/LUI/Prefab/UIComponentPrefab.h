////////////////////////////////////////////////////////////////////////////////
// Prefab/UIComponentPrefab.h (Leggiero/Modules - LegacyUI)
//
// Prefab for UI Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__PREFAB__UI_COMPONENT_PREFAB_H
#define __LM_LUI__PREFAB__UI_COMPONENT_PREFAB_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Leggiero.LegacyUI
#include "../Description/UIDescriptionTypes.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UIManager;
		class UIObject;
		class IUIComponent;
		class IRectSizeSettable;

		namespace Description
		{
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


				// Data structure for component post processing
				struct ComponentPostProcessCreatingContext
				{
				public:
					std::vector<std::shared_ptr<IRectSizeSettable> >	needToSizeSets;
					std::vector<std::function<bool()> >					delayedSizeSetsForParent;

				public:
					void DoPostProcessAfterFabrication(std::shared_ptr<UIObject> createdObject, DescriptionProcessingContext &processingContext, ComponentPostProcessCreatingContext &parentPostProcessContext);

				protected:
					void _DoFitSizingPostProess(std::shared_ptr<UIObject> createdObject, ComponentPostProcessCreatingContext &parentPostProcessContext);
				};


				// Interface for prefab to create UI component
				// Only used in Object
				class IUIComponentPrefab
				{
				public:
					virtual ~IUIComponentPrefab() { }

				public:
					virtual std::shared_ptr<IUIComponent> Fabricate(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::shared_ptr<UIObject> ownerObject, IUIPrefabPlaceProcessor *placeProcessor,
						ComponentPostProcessCreatingContext &postProcessingContext) = 0;

				protected:
					virtual bool _IsSizeFittingNeeded(DescriptionProcessingContext &processingContext);

				protected:
					std::shared_ptr<Expression::IExpression<BooleanValueType> > m_isFitSizeExpression;
				};


				// Place Holder for a component created by control code
				class ComponentPlaceHolder
					: public IUIComponentPrefab
				{
				public:
					using CreateKeyType = std::string;

				public:
					virtual ~ComponentPlaceHolder() { }

				public:	// IUIComponentPrefab
					virtual std::shared_ptr<IUIComponent> Fabricate(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::shared_ptr<UIObject> ownerObject, IUIPrefabPlaceProcessor *placeProcessor, ComponentPostProcessCreatingContext &postProcessingContext) override;

				public:
					void SetCreatingKey(CreateKeyType key) { m_creatingKey = key; }
					void AddParameter(std::string name, GameDataString data) { m_parameters[name] = data; }

				protected:
					CreateKeyType									m_creatingKey;
					std::unordered_map<std::string, GameDataString>	m_parameters;
				};
			}
		}
	}
}

#endif
