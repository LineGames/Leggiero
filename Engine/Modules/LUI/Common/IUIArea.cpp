////////////////////////////////////////////////////////////////////////////////
// Common/IUIArea.cpp (Leggiero/Modules - LegacyUI)
//
// Common base implementation for area
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "IUIArea.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// Common Concrete Class

		namespace _UIAreaTrivial
		{
			//------------------------------------------------------------------------------
			class UIAreaNo
				: public IUIArea
			{
			public:
				UIAreaNo() { }
				virtual ~UIAreaNo() { }

			public:
				virtual bool IsInArea(UICoordinateType x, UICoordinateType y) const override { return false; }
			};

			//------------------------------------------------------------------------------
			class UIAreaEverywhere
				: public IUIArea
			{
			public:
				UIAreaEverywhere() { }
				virtual ~UIAreaEverywhere() { }

			public:
				virtual bool IsInArea(UICoordinateType x, UICoordinateType y) const override { return true; }
			};
		}


		//////////////////////////////////////////////////////////////////////////////// IUIArea

		std::shared_ptr<IUIArea> IUIArea::kNoArea(std::dynamic_pointer_cast<IUIArea>(std::make_shared<_UIAreaTrivial::UIAreaNo>()));
		std::shared_ptr<IUIArea> IUIArea::kEverywhereArea(std::dynamic_pointer_cast<IUIArea>(std::make_shared<_UIAreaTrivial::UIAreaEverywhere>()));
	}
}
