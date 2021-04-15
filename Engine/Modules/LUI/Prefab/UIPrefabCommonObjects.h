////////////////////////////////////////////////////////////////////////////////
// Prefab/UIPrefabCommonObjects.h (Leggiero/Modules - LegacyUI)
//
// Common object definitions for UI prefab
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__PREFAB__UI_PREFAB_COMMON_OBJECTS_H
#define __LM_LUI__PREFAB__UI_PREFAB_COMMON_OBJECTS_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>
#include <string>
#include <unordered_map>

// Leggiero.LegacyUI
#include "../Description/UIDescriptionTypes.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UIManager;
		class UIObject;


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
				// Forward Declarations
				class IUIPrefabPlaceProcessor;
				struct ComponentPostProcessCreatingContext;


				// Type tag for object prefabs
				enum class ObjectPrefabClassType
				{
					kINVALID,

					kPlaceholder,
					kPrefabImport,
					kObject,
				};


				// Abstract interface for UI Object Prefab
				class IUIObjectPrefab
				{
				public:
					IUIObjectPrefab() : m_hasId(false) { }
					IUIObjectPrefab(const std::string &id) : m_hasId(true), m_id(TypeUtility::ToVariableName(id)) { }
					IUIObjectPrefab(std::string &&id) : m_hasId(true), m_id(TypeUtility::ToVariableName(id)) { }

					virtual ~IUIObjectPrefab() { }

				public:
					void SetId(const char *id) { if (id == nullptr) { m_id = TypeUtility::kInvalidName; m_hasId = false; } else { m_id = TypeUtility::ToVariableName(id); m_hasId = true; } }
					void SetId(const std::string &id) { m_id = TypeUtility::ToVariableName(id); m_hasId = id.empty(); }

					bool HasId() const { return m_hasId; }
					const VariableNameType &GetId() const { return m_id; }

					void SetShowCondition(std::shared_ptr<Expression::IExpression<BooleanValueType> > conditionExpression) { m_showConditionExpression = conditionExpression; }
					void SetHideCondition(std::shared_ptr<Expression::IExpression<BooleanValueType> > conditionExpression) { m_hideConditionExpression = conditionExpression; }

					bool IsShowing(DescriptionProcessingContext &processingContext);

				public:
					virtual ObjectPrefabClassType GetClassType() const { return ObjectPrefabClassType::kINVALID; }
					virtual std::shared_ptr<UIObject> FabricateObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager,
						std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &parentPostProcessContext, IUIPrefabPlaceProcessor *placeProcessor = nullptr) = 0;

				protected:
					void _RegisterToIdTable(std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, std::weak_ptr<UIObject> createdObjectPointer);

				protected:
					bool				m_hasId;
					VariableNameType	m_id;

					std::shared_ptr<Expression::IExpression<BooleanValueType> > m_showConditionExpression;
					std::shared_ptr<Expression::IExpression<BooleanValueType> > m_hideConditionExpression;
				};
			}
		}
	}
}

#endif
