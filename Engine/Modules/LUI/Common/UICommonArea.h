////////////////////////////////////////////////////////////////////////////////
// Common/UICommonArea.h (Leggiero/Modules - LegacyUI)
//
// Commonly used areas
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__COMMON__UI_COMMON_AREA_H
#define __LM_LUI__COMMON__UI_COMMON_AREA_H


// Standard Library
#include <vector>

// Leggiero.LegacyUI
#include "IUIArea.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declaration
		class UIObject;


		namespace CommonArea
		{
			namespace Operation
			{
				// Area Not Operation
				class Not
					: public IUIArea
				{
				public:
					Not(const std::shared_ptr<IUIArea> &sourceArea)
						: m_sourceArea(sourceArea)
					{ }
					virtual ~Not() { }

				public:
					virtual bool IsInArea(UICoordinateType x, UICoordinateType y) const override { return !m_sourceArea->IsInArea(x, y); }

				protected:
					const std::shared_ptr<IUIArea> m_sourceArea;
				};


				// Area Union Operation
				class Union
					: public IUIArea
				{
				public:
					Union() { }
					Union(const std::shared_ptr<IUIArea> &a, const std::shared_ptr<IUIArea> &b);
					Union(const std::shared_ptr<IUIArea> &a, const std::shared_ptr<IUIArea> &b, const std::shared_ptr<IUIArea> &c);
					Union(std::initializer_list<std::shared_ptr<IUIArea> > initializer);

					template <typename InputIterator>
					Union(InputIterator first, InputIterator last)
						: m_sourceAreas(first, last)
					{ }

					virtual ~Union() { }

				public:
					virtual bool IsInArea(UICoordinateType x, UICoordinateType y) const override;

				protected:
					std::vector<std::shared_ptr<IUIArea> > m_sourceAreas;
				};


				// Area Intersection Operation
				class Intersection
					: public IUIArea
				{
				public:
					Intersection() { }
					Intersection(const std::shared_ptr<IUIArea> &a, const std::shared_ptr<IUIArea> &b);
					Intersection(const std::shared_ptr<IUIArea> &a, const std::shared_ptr<IUIArea> &b, const std::shared_ptr<IUIArea> &c);
					Intersection(std::initializer_list<std::shared_ptr<IUIArea> > initializer);

					template <typename InputIterator>
					Intersection(InputIterator first, InputIterator last)
						: m_sourceAreas(first, last)
					{ }

					virtual ~Intersection() { }

				public:
					virtual bool IsInArea(UICoordinateType x, UICoordinateType y) const override;

				protected:
					std::vector<std::shared_ptr<IUIArea> > m_sourceAreas;
				};


				// Area Operators
				inline std::shared_ptr<IUIArea> operator~(const std::shared_ptr<IUIArea> &a) { return std::dynamic_pointer_cast<IUIArea>(std::make_shared<Not>(a)); }
				inline std::shared_ptr<IUIArea> operator-(const std::shared_ptr<IUIArea> &a) { return std::dynamic_pointer_cast<IUIArea>(std::make_shared<Not>(a)); }
				inline std::shared_ptr<IUIArea> operator+(const std::shared_ptr<IUIArea> &a, const std::shared_ptr<IUIArea> &b) { return std::dynamic_pointer_cast<IUIArea>(std::make_shared<Union>(a, b)); }
				inline std::shared_ptr<IUIArea> operator-(const std::shared_ptr<IUIArea> &a, const std::shared_ptr<IUIArea> &b) { return std::dynamic_pointer_cast<IUIArea>(std::make_shared<Union>(a, std::dynamic_pointer_cast<IUIArea>(std::make_shared<Not>(b)))); }
				inline std::shared_ptr<IUIArea> operator*(const std::shared_ptr<IUIArea> &a, const std::shared_ptr<IUIArea> &b) { return std::dynamic_pointer_cast<IUIArea>(std::make_shared<Intersection>(a, b)); }
			}


			namespace Shape
			{
				// Rectangular Area
				class Rect
					: public IUIArea
				{
				public:
					Rect(UICoordinateType width, UICoordinateType height, UICoordinateType left = static_cast<UICoordinateType>(0.0f), UICoordinateType top = static_cast<UICoordinateType>(0.0f));
					virtual ~Rect() { }

				public:
					virtual bool IsInArea(UICoordinateType x, UICoordinateType y) const override;

				protected:
					UICoordinateType m_left;
					UICoordinateType m_top;
					UICoordinateType m_right;
					UICoordinateType m_bottom;
				};


				// Circular Area
				class Circle
					: public IUIArea
				{
				public:
					Circle(UICoordinateType radius, UICoordinateType x = static_cast<UICoordinateType>(0.0f), UICoordinateType y = static_cast<UICoordinateType>(0.0f));
					virtual ~Circle() { }

				public:
					virtual bool IsInArea(UICoordinateType x, UICoordinateType y) const override;

				protected:
					UICoordinateType m_radiusSquare;
					UICoordinateType m_x;
					UICoordinateType m_y;
				};
			}


			// Specific rational rectangular area in the screen
			class ScreenArea
				: public IUIArea
			{
			public:
				ScreenArea(UICoordinateRatioType leftRatio = static_cast<UICoordinateRatioType>(0.0f), UICoordinateRatioType topRatio = static_cast<UICoordinateRatioType>(0.0f), UICoordinateRatioType rightRatio = static_cast<UICoordinateRatioType>(1.0f), UICoordinateRatioType bottomRatio = static_cast<UICoordinateRatioType>(1.0f));
				virtual ~ScreenArea() { }

			public:
				virtual bool IsInArea(UICoordinateType x, UICoordinateType y) const override;

			protected:
				UICoordinateRatioType m_leftRatio;
				UICoordinateRatioType m_topRatio;
				UICoordinateRatioType m_rightRatio;
				UICoordinateRatioType m_bottomRatio;
			};

			// Specific absolute rectangular area in the screen
			class ScreenAbsoluteArea
				: public IUIArea
			{
			public:
				ScreenAbsoluteArea(UICoordinateType left = static_cast<UICoordinateType>(0.0f), UICoordinateType top = static_cast<UICoordinateType>(0.0f), UICoordinateType right = static_cast<UICoordinateType>(0.0f), UICoordinateType bottom = static_cast<UICoordinateType>(0.0f));
				virtual ~ScreenAbsoluteArea() { }

			public:
				virtual bool IsInArea(UICoordinateType x, UICoordinateType y) const override;

			protected:
				UICoordinateType m_left;
				UICoordinateType m_top;
				UICoordinateType m_right;
				UICoordinateType m_bottom;
			};


			// Area of UI Object
			class ObjectBasedArea
				: public IUIArea
			{
			public:
				ObjectBasedArea(std::shared_ptr<UIObject> sourceObject, UICoordinateRatioType widthRatio = static_cast<UICoordinateRatioType>(1.0f), UICoordinateRatioType heightRatio = static_cast<UICoordinateRatioType>(1.0f),
					UICoordinateType offsetX = static_cast<UICoordinateType>(0.0f), UICoordinateType offsetY = static_cast<UICoordinateType>(0.0f));
				virtual ~ObjectBasedArea() { }

			public:
				virtual bool IsInArea(UICoordinateType x, UICoordinateType y) const override;

			public:
				bool IsApplyClipping() const { return m_isApplyClipping; }
				void SetApplyClipping(bool isApplyClipping) { m_isApplyClipping = isApplyClipping; }

			protected:
				std::weak_ptr<UIObject> m_sourceObject;

				UICoordinateRatioType m_widthRatio;
				UICoordinateRatioType m_heightRatio;

				UICoordinateType m_offsetX;
				UICoordinateType m_offsetY;

				bool m_isApplyClipping;
			};


			// Offseted area for UI Object
			class ObjectOffsetArea
				: public IUIArea
			{
			public:
				ObjectOffsetArea(std::shared_ptr<IUIArea> originalArea, std::shared_ptr<UIObject> sourceObject,
					UICoordinateRatioType referenceRatioX = static_cast<UICoordinateRatioType>(0.0f), UICoordinateRatioType referenceRatioY = static_cast<UICoordinateRatioType>(0.0f),
					UICoordinateType offsetX = static_cast<UICoordinateType>(0.0f), UICoordinateType offsetY = static_cast<UICoordinateType>(0.0f));
				virtual ~ObjectOffsetArea() { }

			public:
				virtual bool IsInArea(UICoordinateType x, UICoordinateType y) const override;

			public:
				bool IsApplyClipping() const { return m_isApplyClipping; }
				void SetApplyClipping(bool isApplyClipping) { m_isApplyClipping = isApplyClipping; }

			protected:
				std::shared_ptr<IUIArea> m_originalArea;

				std::weak_ptr<UIObject> m_sourceObject;

				UICoordinateRatioType m_referenceRatioX;
				UICoordinateRatioType m_referenceRatioY;
				UICoordinateType m_offsetX;
				UICoordinateType m_offsetY;

				bool m_isApplyClipping;
			};
		}
	}
}

#endif
