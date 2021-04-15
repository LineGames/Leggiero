////////////////////////////////////////////////////////////////////////////////
// Common/UITransform.cpp (Leggiero/Modules - LegacyUI)
//
// Transform Implementations
////////////////////////////////////////////////////////////////////////////////

// Math Defines
#define _USE_MATH_DEFINES
#include <cmath>

// My Header
#include "UITransform.h"

// External Library
#include <glm/gtx/matrix_transform_2d.hpp>


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// IUITransform

		//------------------------------------------------------------------------------
		std::shared_ptr<IUITransform> IUITransform::Combine(std::shared_ptr<IUITransform> rhs)
		{
			if (!rhs)
			{
				return shared_from_this();
			}
			return std::make_shared<UICombinedTransform>(shared_from_this(), rhs);
		}


		//////////////////////////////////////////////////////////////////////////////// IdentityTransform

		moodycamel::ConcurrentQueue<std::shared_ptr<IdentityTransform> > IdentityTransform::ms_transformPool;

		//------------------------------------------------------------------------------
		std::shared_ptr<IUITransform> IdentityTransform::Combine(std::shared_ptr<IUITransform> rhs)
		{
			if (!rhs)
			{
				return shared_from_this();
			}
			return rhs;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<IdentityTransform> IdentityTransform::AllocateTransform()
		{
			std::shared_ptr<IdentityTransform> transformEntry;
			if (ms_transformPool.try_dequeue(transformEntry))
			{
				return transformEntry;
			}
			else
			{
				return std::make_shared<IdentityTransform>();
			}
		}

		//------------------------------------------------------------------------------
		void IdentityTransform::ReleaseTransform(std::shared_ptr<IdentityTransform> transformToRelease)
		{
			if (transformToRelease)
			{
				ms_transformPool.enqueue(transformToRelease);
			}
		}


		//////////////////////////////////////////////////////////////////////////////// OffsetTransform

		moodycamel::ConcurrentQueue<std::shared_ptr<OffsetTransform> > OffsetTransform::ms_transformPool;

		//------------------------------------------------------------------------------
		std::shared_ptr<IUITransform> OffsetTransform::Combine(std::shared_ptr<IUITransform> rhs)
		{
			if (!rhs)
			{
				return shared_from_this();
			}

			if (rhs->_IsMatrixTransform())
			{
				std::shared_ptr<UITransform> matrixTransformRHS(std::dynamic_pointer_cast<UITransform>(rhs));

				UIParameterTransform offsetMatrixTransform;
				offsetMatrixTransform.SetTranslate(m_xOffset, m_yOffset);

				return std::make_shared<UITransform>(offsetMatrixTransform * (*matrixTransformRHS));
			}
			else
			{
				std::shared_ptr<OffsetTransform> offsetRhs(std::dynamic_pointer_cast<OffsetTransform>(rhs));
				if (offsetRhs)
				{
					std::shared_ptr<OffsetTransform> combined(std::make_shared<OffsetTransform>(m_xOffset + offsetRhs->m_xOffset, m_yOffset + offsetRhs->m_yOffset));
					return combined;
				}

				std::shared_ptr<IdentityTransform> idRhs(std::dynamic_pointer_cast<IdentityTransform>(rhs));
				if (idRhs)
				{
					return shared_from_this();
				}
			}

			return IUITransform::Combine(rhs);
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<OffsetTransform> OffsetTransform::AllocateTransform(UICoordinateType xOffset, UICoordinateType yOffset)
		{
			std::shared_ptr<OffsetTransform> transformEntry;
			if (ms_transformPool.try_dequeue(transformEntry))
			{
				transformEntry->m_xOffset = xOffset;
				transformEntry->m_yOffset = yOffset;
				return transformEntry;
			}
			else
			{
				return std::make_shared<OffsetTransform>(xOffset, yOffset);
			}
		}

		//------------------------------------------------------------------------------
		void OffsetTransform::ReleaseTransform(std::shared_ptr<OffsetTransform> transformToRelease)
		{
			if (transformToRelease)
			{
				ms_transformPool.enqueue(transformToRelease);
			}
		}


		//////////////////////////////////////////////////////////////////////////////// UICombinedTransform

		moodycamel::ConcurrentQueue<std::shared_ptr<UICombinedTransform> > UICombinedTransform::ms_transformPool;

		//------------------------------------------------------------------------------
		UICombinedTransform::UICombinedTransform(std::shared_ptr<IUITransform> pre, std::shared_ptr<IUITransform> post)
			: m_pre(pre), m_post(post)
		{
		}

		//------------------------------------------------------------------------------
		UIVector2D UICombinedTransform::ApplyTransform(UICoordinateType x, UICoordinateType y)
		{
			UIVector2D result = m_post->ApplyTransform(x, y);
			return m_pre->ApplyTransform(result.x, result.y);
		}

		//------------------------------------------------------------------------------
		UIVector2D UICombinedTransform::ApplyInverseTransform(UICoordinateType x, UICoordinateType y)
		{
			UIVector2D result = m_pre->ApplyInverseTransform(x, y);
			return m_post->ApplyInverseTransform(result.x, result.y);
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<UICombinedTransform> UICombinedTransform::AllocateTransform(std::shared_ptr<IUITransform> pre, std::shared_ptr<IUITransform> post)
		{
			std::shared_ptr<UICombinedTransform> transformEntry;
			if (ms_transformPool.try_dequeue(transformEntry))
			{
				transformEntry->m_pre = pre;
				transformEntry->m_post = post;
				return transformEntry;
			}
			else
			{
				return std::make_shared<UICombinedTransform>(pre, post);
			}
		}

		//------------------------------------------------------------------------------
		void UICombinedTransform::ReleaseTransform(std::shared_ptr<UICombinedTransform> transformToRelease)
		{
			if (transformToRelease)
			{
				transformToRelease->m_pre.reset();
				transformToRelease->m_post.reset();
				ms_transformPool.enqueue(transformToRelease);
			}
		}


		//////////////////////////////////////////////////////////////////////////////// UITransform

		//------------------------------------------------------------------------------
		UITransform::UITransform()
			: m_transform(1.0f), m_isTransformDirty(false), m_inverseTransform(1.0f), m_isInverseDirty(false)
		{
		}

		//------------------------------------------------------------------------------
		UITransform::UITransform(UITransform &rhs)
		{
			rhs._UpdateTransform();

			m_transform = rhs.m_transform;
			m_isTransformDirty = rhs.m_isTransformDirty;
			m_inverseTransform = rhs.m_inverseTransform;
			m_isInverseDirty = rhs.m_isInverseDirty;
		}

		//------------------------------------------------------------------------------
		UITransform::UITransform(UITransform &&rhs)
		{
			rhs._UpdateTransform();

			m_transform = rhs.m_transform;
			m_isTransformDirty = rhs.m_isTransformDirty;
			m_inverseTransform = rhs.m_inverseTransform;
			m_isInverseDirty = rhs.m_isInverseDirty;
		}

		//------------------------------------------------------------------------------
		UITransform::~UITransform()
		{
		}

		//------------------------------------------------------------------------------
		UIVector2D UITransform::ApplyTransform(UICoordinateType x, UICoordinateType y)
		{
			glm::vec3 position(x, y, 1.0f);
			_UpdateTransform();

			position = m_transform * position;
			if (std::isinf(position.z))
			{
				float effectiveX;
				float effectiveY;
				if (std::isinf(position.x))
				{
					effectiveX = x * m_transform[0][0] + y * m_transform[0][1] + m_transform[0][2];
				}
				else
				{
					effectiveX = m_transform[0][2];
				}
				if (std::isinf(position.y))
				{
					effectiveY = x * m_transform[1][0] + y * m_transform[1][1] + m_transform[1][2];
				}
				else
				{
					effectiveY = m_transform[1][2];
				}
				return UIVector2D((UICoordinateType)effectiveX, (UICoordinateType)effectiveY);
			}

			return UIVector2D((UICoordinateType)(position.x / position.z), (UICoordinateType)(position.y / position.z));
		}

		//------------------------------------------------------------------------------
		UIVector2D UITransform::ApplyInverseTransform(UICoordinateType x, UICoordinateType y)
		{
			glm::vec3 position(x, y, 1.0f);
			_UpdateInverseTransform();

			position = m_inverseTransform * position;

			return UIVector2D((UICoordinateType)(position.x / position.z), (UICoordinateType)(position.y / position.z));
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<IUITransform> UITransform::Combine(std::shared_ptr<IUITransform> rhs)
		{
			if (!rhs)
			{
				return shared_from_this();
			}

			if (rhs->_IsMatrixTransform())
			{
				std::shared_ptr<UITransform> matrixTransformRHS(std::dynamic_pointer_cast<UITransform>(rhs));
				if (matrixTransformRHS)
				{
					return std::make_shared<UITransform>((*this) * (*matrixTransformRHS));
				}
			}
			else
			{
				std::shared_ptr<IdentityTransform> idRhs(std::dynamic_pointer_cast<IdentityTransform>(rhs));
				if (idRhs)
				{
					return shared_from_this();
				}

				std::shared_ptr<OffsetTransform> offsetRhs(std::dynamic_pointer_cast<OffsetTransform>(rhs));
				if (offsetRhs)
				{
					UIParameterTransform offsetMatrixTransform;
					offsetMatrixTransform.SetTranslate(offsetRhs->GetOffsetX(), offsetRhs->GetOffsetY());

					return std::make_shared<UITransform>((*this) * offsetMatrixTransform);
				}
			}

			return IUITransform::Combine(rhs);
		}

		//------------------------------------------------------------------------------
		// Calculate sequential transform as product
		UITransform UITransform::operator*(UITransform &rhs)
		{
			_UpdateTransform();
			rhs._UpdateTransform();

			UITransform result;
			result.m_transform = m_transform * rhs.m_transform;
			result.m_isTransformDirty = false;
			result.m_isInverseDirty = true;
			return result;
		}

		//------------------------------------------------------------------------------
		UITransform &UITransform::operator=(UITransform &rhs)
		{
			rhs._UpdateTransform();

			m_transform = rhs.m_transform;
			m_isTransformDirty = rhs.m_isTransformDirty;
			m_inverseTransform = rhs.m_inverseTransform;
			m_isInverseDirty = rhs.m_isInverseDirty;
			return *this;
		}

		//------------------------------------------------------------------------------
		// Combine rhs transform
		UITransform &UITransform::operator*=(UITransform &rhs)
		{
			_UpdateTransform();
			rhs._UpdateTransform();

			m_transform = m_transform * rhs.m_transform;
			m_isTransformDirty = false;
			m_isInverseDirty = true;
			return *this;
		}

		//------------------------------------------------------------------------------
		// Calculate sequential transform as product
		UITransform UITransform::operator*(UITransform &&rhs)
		{
			_UpdateTransform();
			rhs._UpdateTransform();

			UITransform result;
			result.m_transform = m_transform * rhs.m_transform;
			result.m_isTransformDirty = false;
			result.m_isInverseDirty = true;
			return result;
		}

		//------------------------------------------------------------------------------
		UITransform &UITransform::operator=(UITransform &&rhs)
		{
			rhs._UpdateTransform();

			m_transform = rhs.m_transform;
			m_isTransformDirty = rhs.m_isTransformDirty;
			m_inverseTransform = rhs.m_inverseTransform;
			m_isInverseDirty = rhs.m_isInverseDirty;
			return *this;
		}

		//------------------------------------------------------------------------------
		// Combine rhs transform
		UITransform &UITransform::operator*=(UITransform &&rhs)
		{
			_UpdateTransform();
			rhs._UpdateTransform();

			m_transform = m_transform * rhs.m_transform;
			m_isTransformDirty = false;
			m_isInverseDirty = true;
			return *this;
		}

		//------------------------------------------------------------------------------
		// Update transform matrix
		void UITransform::_UpdateTransform()
		{
			if (!m_isTransformDirty)
			{
				return;
			}

			// Do Nothing for Basic Transform

			m_isTransformDirty = false;
		}

		//------------------------------------------------------------------------------
		// Update inverse transform matrix
		void UITransform::_UpdateInverseTransform()
		{
			if (!m_isInverseDirty)
			{
				return;
			}
			_UpdateTransform();

			m_inverseTransform = glm::inverse(m_transform);

			m_isInverseDirty = false;
		}


		//////////////////////////////////////////////////////////////////////////////// UIParameterTransform

		//------------------------------------------------------------------------------
		UIParameterTransform::UIParameterTransform()
			: m_translate(0.0f), m_anchor(0.0f), m_rotate(0.0f), m_scale(1.0f), m_skew(0.0f), m_roughPerspective(0.0f)
		{
		}

		//------------------------------------------------------------------------------
		UIParameterTransform::~UIParameterTransform()
		{
		}

		//------------------------------------------------------------------------------
		// Update transform matrix
		void UIParameterTransform::_UpdateTransform()
		{
			if (!m_isTransformDirty)
			{
				return;
			}

			glm::mat3 transform(1.0f);
			transform = glm::translate(transform, m_translate);
			transform = glm::translate(transform, m_anchor);
			transform = glm::rotate(transform, m_rotate);
			transform = glm::scale(transform, m_scale);
			transform = glm::shearX(transform, m_skew.y);
			transform = glm::shearY(transform, m_skew.x);

			if (fabs(m_roughPerspective.x) > kFloatEpsilon || fabs(m_roughPerspective.y) > kFloatEpsilon)
			{
				float focalPointX = 0.0f;
				float focalPointY = 0.0f;
				if (fabs(m_roughPerspective.x) > kFloatEpsilon)
				{
					focalPointX = 1.0f / m_roughPerspective.x;
				}
				if (fabs(m_roughPerspective.y) > kFloatEpsilon)
				{
					focalPointY = 1.0f / m_roughPerspective.y;
				}
				float rotateAngle = 0.0f;
				if (fabs(focalPointX) < kFloatEpsilon)
				{
					if (focalPointY >= 0.0f)
					{
						rotateAngle = (float)M_PI_2;
					}
					else
					{
						rotateAngle = -(float)M_PI_2;
					}
				}
				else
				{
					rotateAngle = atan(focalPointY / focalPointX);
					if (focalPointX < 0.0f)
					{
						rotateAngle += (float)M_PI;
					}
				}
				rotateAngle += (float)M_PI_2;
				float focalDistance = (float)sqrt(focalPointX * focalPointX + focalPointY * focalPointY);

				transform = glm::rotate(transform, rotateAngle);
				transform = glm::translate(transform, glm::vec2(0.0f, -focalDistance));

				// Used transform matrix as following (roughly calculate inverse transform for a rect in focalDistance ~ 2 * focalDistance distance):
				// { {a, 0, 0}, { 0, 3 * a, -2 * a ^ 2 }, { 0, 1, 0 }}
				// -> { {a ^ (-1), 0, 0}, { 0, 0, 1 }, { 0, -1 / (2 a ^ 2), 3 / (2 a) }}
				glm::mat3 perspective(0.0f);
				perspective[0][0] = 1.0f / focalDistance;
				perspective[2][1] = 1.0f;
				perspective[1][2] = -1.0f / (2.0f * focalDistance * focalDistance);
				perspective[2][2] = 3.0f / (2.0f * focalDistance);
				transform = transform * perspective;

				transform = glm::translate(transform, glm::vec2(0.0f, focalDistance));
				transform = glm::rotate(transform, -rotateAngle);
			}

			transform = glm::translate(transform, -m_anchor);

			m_transform = transform;
			m_isInverseDirty = true;
			m_isTransformDirty = false;
		}


		//////////////////////////////////////////////////////////////////////////////// UIPerspectiveTransform

		//------------------------------------------------------------------------------
		UIPerspectiveTransform::UIPerspectiveTransform()
			: m_perspective(0.0f), m_anchor(0.0f)
		{
		}

		//------------------------------------------------------------------------------
		UIPerspectiveTransform::UIPerspectiveTransform(const UIPerspectiveTransform &rhs)
			: m_perspective(rhs.m_perspective), m_anchor(rhs.m_anchor)
		{
		}

		//------------------------------------------------------------------------------
		UIPerspectiveTransform::~UIPerspectiveTransform()
		{
		}

		//------------------------------------------------------------------------------
		UIVector2D UIPerspectiveTransform::ApplyTransform(UICoordinateType x, UICoordinateType y)
		{
			if (fabs(m_perspective.x) < kFloatEpsilon && fabs(m_perspective.y) < kFloatEpsilon)
			{
				return UIVector2D(x, y);
			}
			UICoordinateType focalLength = (UICoordinateType)(1.0f / sqrt(m_perspective.x * m_perspective.x + m_perspective.y * m_perspective.y));
			UIVector2D focalDirection((UICoordinateType)m_perspective.x, (UICoordinateType)m_perspective.y); focalDirection.DoNormalize();

			UIVector2D offsetPosition(x - (UICoordinateType)m_anchor.x, y - (UICoordinateType)m_anchor.y);
			UICoordinateType focalComponentLength = offsetPosition.GetDotWith(focalDirection);
			UIVector2D focalComponent(focalComponentLength * focalDirection);
			UIVector2D perpendicularComponent(offsetPosition - focalComponent);

			UICoordinateType perspectiveRatio = (focalLength - focalComponentLength) / focalLength;
			return (perspectiveRatio * focalComponent + perspectiveRatio * perpendicularComponent) + UIVector2D((UICoordinateType)m_anchor.x, (UICoordinateType)m_anchor.y);
		}

		//------------------------------------------------------------------------------
		UIVector2D UIPerspectiveTransform::ApplyInverseTransform(UICoordinateType x, UICoordinateType y)
		{
			if (fabs(m_perspective.x) < kFloatEpsilon && fabs(m_perspective.y) < kFloatEpsilon)
			{
				return UIVector2D(x, y);
			}
			UICoordinateType focalLength = (UICoordinateType)(1.0f / sqrt(m_perspective.x * m_perspective.x + m_perspective.y * m_perspective.y));
			UIVector2D focalDirection((UICoordinateType)m_perspective.x, (UICoordinateType)m_perspective.y); focalDirection.DoNormalize();

			UIVector2D offsetPosition(x + (UICoordinateType)m_anchor.x, y + (UICoordinateType)m_anchor.y);
			UICoordinateType focalComponentLength = offsetPosition.GetDotWith(focalDirection);
			UIVector2D focalComponent(focalComponentLength * focalDirection);
			UIVector2D perpendicularComponent(offsetPosition - focalComponent);

			UICoordinateType inversePerspectiveRatio = focalLength / (focalLength - focalComponentLength);
			return (inversePerspectiveRatio * focalComponent + inversePerspectiveRatio * perpendicularComponent) - UIVector2D((UICoordinateType)m_anchor.x, (UICoordinateType)m_anchor.y);
		}

		//------------------------------------------------------------------------------
		UIPerspectiveTransform &UIPerspectiveTransform::operator=(const UIPerspectiveTransform &rhs)
		{
			m_perspective = rhs.m_perspective;
			m_anchor = rhs.m_anchor;
			return *this;
		}

		//------------------------------------------------------------------------------
		UIVector2D UIPerspectiveTransform::GetPerspectiveFocus() const
		{
			if (fabs(m_perspective.x) < kFloatEpsilon && fabs(m_perspective.y) < kFloatEpsilon)
			{
				return UIVector2D::kZero;
			}
			UICoordinateType focalLength = (UICoordinateType)(1.0f / sqrt(m_perspective.x * m_perspective.x + m_perspective.y * m_perspective.y));
			UIVector2D focalDirection((UICoordinateType)m_perspective.x, (UICoordinateType)m_perspective.y); focalDirection.DoNormalize();
			return focalLength * focalDirection;
		}

		//------------------------------------------------------------------------------
		UICoordinateType UIPerspectiveTransform::GetPerspectiveFocusX() const
		{
			return GetPerspectiveFocus().x;
		}

		//------------------------------------------------------------------------------
		UICoordinateType UIPerspectiveTransform::GetPerspectiveFocusY() const
		{
			return GetPerspectiveFocus().y;
		}

		//------------------------------------------------------------------------------
		void UIPerspectiveTransform::SetPerspectiveFocus(UICoordinateType x, UICoordinateType y)
		{
			if (fabs(x) < kFloatEpsilon && fabs(y) < kFloatEpsilon)
			{
				m_perspective.x = 0.0f;
				m_perspective.y = 0.0f;
				return;
			}
			UIVector2D focalPoint(x, y);
			UICoordinateType focalLength = (UICoordinateType)focalPoint.Length();
			UICoordinateType paramA = (UICoordinateType)(1.0f / (focalLength * focalLength));
			if (fabs(x) < kFloatEpsilon)
			{
				m_perspective.x = 0.0f;
				m_perspective.y = (y > 0.0f) ? (float)sqrt(paramA) : -(float)sqrt(paramA);
				return;
			}
			else if (fabs(y) < kFloatEpsilon)
			{
				m_perspective.x = (x > 0.0f) ? (float)sqrt(paramA) : -(float)sqrt(paramA);
				m_perspective.y = 0.0f;
				return;
			}
			UICoordinateType paramB = x / y;
			m_perspective.y = (y > 0.0f) ? (float)sqrt(paramA / (paramB * paramB + 1)) : -(float)sqrt(paramA / (paramB * paramB + 1));
			m_perspective.x = paramB * m_perspective.y;
		}
	}
}
