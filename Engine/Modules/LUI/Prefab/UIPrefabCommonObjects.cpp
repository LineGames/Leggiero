////////////////////////////////////////////////////////////////////////////////
// Prefab/UIPrefabCommonObjects.cpp (Leggiero/Modules - LegacyUI)
//
// Common UI prefab implementations
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIPrefabCommonObjects.h"

// Leggiero.LegacyUI
#include "../Description/UIPrefabDescription.h"
#include "../Description/UIDescriptionExpression.h"
#include "UIComponentPrefab.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Prefab
			{
				//////////////////////////////////////////////////////////////////////////////// IUIObjectPrefab

				//------------------------------------------------------------------------------
				bool IUIObjectPrefab::IsShowing(DescriptionProcessingContext &processingContext)
				{
					bool isShowing = true;
					if (m_showConditionExpression)
					{
						isShowing = static_cast<bool>(m_showConditionExpression->Evaluate(processingContext));
					}
					if (!isShowing)
					{
						return false;
					}

					bool isHiding = false;
					if (m_hideConditionExpression)
					{
						isHiding = static_cast<bool>(m_hideConditionExpression->Evaluate(processingContext));
					}
					if (isHiding)
					{
						return false;
					}

					return true;
				}

				//------------------------------------------------------------------------------
				void IUIObjectPrefab::_RegisterToIdTable(std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, std::weak_ptr<UIObject> createdObjectPointer)
				{
					if (!m_hasId)
					{
						return;
					}
					idTable[m_id] = createdObjectPointer;
				}
			}
		}
	}
}
