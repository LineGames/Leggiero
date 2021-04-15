////////////////////////////////////////////////////////////////////////////////
// Math/Vector.h (Leggiero - Utility)
//
// Mathmatical Vectors (elements of Vector Space)
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__MATH__VECTOR_H
#define __UTILITY__MATH__VECTOR_H


// Standard Library
#include <cmath>
#include <limits>


namespace Leggiero
{
	namespace Utility
	{
		namespace Math
		{
			//////////////////////////////////////////////////////////////////////////////// Vector2D<ComponentT>

			// 2-Dimensional Vector
			template <typename ComponentT>
			struct Vector2D
			{
			public:
				ComponentT x;
				ComponentT y;

				using ScalarType = ComponentT;

			public:
				Vector2D(ComponentT x, ComponentT y)
					: x(x), y(y)
				{ }

				template <typename OtherComponentT>
				Vector2D(const Vector2D<OtherComponentT> &rhs)
					: x(static_cast<ComponentT>(rhs.x)), y(static_cast<ComponentT>(rhs.y))
				{ }

			public:
				static const Vector2D<ComponentT> kZero;
				static const Vector2D<ComponentT> kInvalid;
				static const Vector2D<ComponentT> kX;
				static const Vector2D<ComponentT> kY;
				static const Vector2D<ComponentT> kOne;

				static Vector2D<ComponentT> SameScalar(ComponentT val)
				{
					return Vector2D<ComponentT>(val, val);
				}

			public:
				Vector2D<ComponentT> operator+() const { return *this; }
				Vector2D<ComponentT> operator-() const { return Vector2D<ComponentT>(-x, -y); }
				Vector2D<ComponentT> operator+(const Vector2D<ComponentT> &b) const { return Vector2D<ComponentT>(x + b.x, y + b.y); }
				Vector2D<ComponentT> operator-(const Vector2D<ComponentT> &b) const { return Vector2D<ComponentT>(x - b.x, y - b.y); }
				Vector2D<ComponentT> &operator+=(const Vector2D<ComponentT> &b) { x += b.x; y += b.y; return *this; }
				Vector2D<ComponentT> &operator-=(const Vector2D<ComponentT> &b) { x -= b.x; y -= b.y; return *this; }

				bool operator==(const Vector2D<ComponentT> &b) const { return ((x == b.x) && (y == b.y)); }
				bool operator!=(const Vector2D<ComponentT> &b) const { return !(*this == b); }

				template <typename ScalarT>
				Vector2D<ComponentT> &operator*=(const ScalarT &b) { x = static_cast<ComponentT>(x * b); y = static_cast<ComponentT>(y * b); return *this; }
				template <typename ScalarT>
				Vector2D<ComponentT> &operator/=(const ScalarT &b) { x = static_cast<ComponentT>(x / b); y = static_cast<ComponentT>(y / b); return *this; }

			public:
				ComponentT Length() const { return std::sqrt((x * x) + (y * y)); }
				ComponentT SquareLength() const { return ((x * x) + (y * y)); }

				bool IsValid() const { return (std::isfinite(x) && std::isfinite(y)); }

				ComponentT GetDotWith(const Vector2D<ComponentT> &b) const { return ((x * b.x) + (y * b.y)); }
				
				static ComponentT Dot(const Vector2D<ComponentT> &a, const Vector2D<ComponentT> &b)
				{
					return ((a.x * b.x) + (a.y * b.y));
				}

				Vector2D<ComponentT> GetElemwiseMultiplyWith(const Vector2D<ComponentT> &b) const { return Vector2D<ComponentT>((x * b.x), (y * b.y)); }

				Vector2D<ComponentT> DoElemwiseMultiplyWith(const Vector2D<ComponentT> &b)
				{
					x *= b.x; y *= b.y;
					return *this;
				}

				static Vector2D<ComponentT> ElemwiseMultiply(const Vector2D<ComponentT> &a, const Vector2D<ComponentT> &b)
				{
					return Vector2D<ComponentT>((a.x * b.x), (a.y * b.y));
				}

				Vector2D<ComponentT> GetNormalized() const
				{
					if (!IsValid()) { return kInvalid; }
					else if (x == 0.0 && y == 0.0) { return kZero; }
					ComponentT length = Length();
					return Vector2D<ComponentT>(x / length, y / length);
				};

				bool DoNormalize()
				{
					if (!IsValid()) { return false; }
					else if (x == 0.0 && y == 0.0) { return false; }
					ComponentT length = Length();
					x /= length; y /= length;
					return true;
				}

			public:
				Vector2D<ComponentT> LerpWith(const Vector2D<ComponentT> &withVector, ComponentT param) const
				{
					return Lerp(*this, withVector, param);
				}

				static Vector2D<ComponentT> Lerp(const Vector2D<ComponentT> &v1, const Vector2D<ComponentT> &v2, ComponentT param)
				{
					return static_cast<ComponentT>(1.0 - param) * v1 + static_cast<ComponentT>(param) * v2;
				}
			};

			template <typename ComponentT, typename ScalarT>
			inline Vector2D<ComponentT> operator*(ScalarT a, const Vector2D<ComponentT> &b) { return Vector2D<ComponentT>(static_cast<ComponentT>(a * b.x), static_cast<ComponentT>(a * b.y)); }

			template <typename ComponentT>
			const Vector2D<ComponentT> Vector2D<ComponentT>::kZero(static_cast<ComponentT>(0.0), static_cast<ComponentT>(0.0));
			template <typename ComponentT>
			const Vector2D<ComponentT> Vector2D<ComponentT>::kInvalid(std::numeric_limits<ComponentT>::quiet_NaN(), std::numeric_limits<ComponentT>::quiet_NaN());
			template <typename ComponentT>
			const Vector2D<ComponentT> Vector2D<ComponentT>::kX(static_cast<ComponentT>(1.0), static_cast<ComponentT>(0.0));
			template <typename ComponentT>
			const Vector2D<ComponentT> Vector2D<ComponentT>::kY(static_cast<ComponentT>(0.0), static_cast<ComponentT>(1.0));
			template <typename ComponentT>
			const Vector2D<ComponentT> Vector2D<ComponentT>::kOne(static_cast<ComponentT>(1.0), static_cast<ComponentT>(1.0));


			//////////////////////////////////////////////////////////////////////////////// Vector3D<ComponentT>

			// 3-Dimensional Vector
			template <typename ComponentT>
			struct Vector3D
			{
			public:
				ComponentT x;
				ComponentT y;
				ComponentT z;

				using ScalarType = ComponentT;

			public:
				Vector3D(ComponentT x, ComponentT y, ComponentT z)
					: x(x), y(y), z(z)
				{ }

				Vector3D(const Vector2D<ComponentT> &rhs)
					: x(rhs.x), y(rhs.y), z(static_cast<ComponentT>(0.0))
				{ }

				template <typename OtherComponentT>
				Vector3D(const Vector3D<OtherComponentT> &rhs)
					: x(static_cast<ComponentT>(rhs.x)), y(static_cast<ComponentT>(rhs.y)), z(static_cast<ComponentT>(rhs.z))
				{ }

			public:
				static const Vector3D<ComponentT> kZero;
				static const Vector3D<ComponentT> kInvalid;
				static const Vector3D<ComponentT> kX;
				static const Vector3D<ComponentT> kY;
				static const Vector3D<ComponentT> kZ;
				static const Vector3D<ComponentT> kOne;

				static Vector3D<ComponentT> SameScalar(ComponentT val)
				{
					return Vector3D<ComponentT>(val, val, val);
				}

			public:
				Vector3D<ComponentT> operator+() const { return *this; }
				Vector3D<ComponentT> operator-() const { return Vector3D<ComponentT>(-x, -y, -z); }
				Vector3D<ComponentT> operator+(const Vector3D<ComponentT> &b) const { return Vector3D<ComponentT>(x + b.x, y + b.y, z + b.z); }
				Vector3D<ComponentT> operator-(const Vector3D<ComponentT> &b) const { return Vector3D<ComponentT>(x - b.x, y - b.y, z - b.z); }
				Vector3D<ComponentT> &operator+=(const Vector3D<ComponentT> &b) { x += b.x; y += b.y; z += b.z; return *this; }
				Vector3D<ComponentT> &operator-=(const Vector3D<ComponentT> &b) { x -= b.x; y -= b.y; z -= b.z; return *this; }

				template <typename ScalarT>
				Vector3D<ComponentT> &operator*=(const ScalarT &b) { x = static_cast<ComponentT>(x * b); y = static_cast<ComponentT>(y * b); z = static_cast<ComponentT>(z * b); return *this; }
				template <typename ScalarT>
				Vector3D<ComponentT> &operator/=(const ScalarT &b) { x = static_cast<ComponentT>(x / b); y = static_cast<ComponentT>(y / b); z = static_cast<ComponentT>(z / b); return *this; }

				bool operator==(const Vector3D<ComponentT> &b) const { return ((x == b.x) && (y == b.y) && (z == b.z)); }
				bool operator!=(const Vector3D<ComponentT> &b) const { return !(*this == b); }

			public:
				ComponentT Length() const { return std::sqrt((x * x) + (y * y) + (z * z)); }
				ComponentT SquareLength() const { return ((x * x) + (y * y) + (z * z)); }

				bool IsValid() const { return (std::isfinite(x) && std::isfinite(y) && std::isfinite(z)); }

				ComponentT GetDotWith(const Vector3D<ComponentT> &b) const { return ((x * b.x) + (y * b.y) + (z * b.z)); }

				static ComponentT Dot(const Vector3D<ComponentT> &a, const Vector3D<ComponentT> &b)
				{
					return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
				}

				Vector3D<ComponentT> GetCrossWith(const Vector3D<ComponentT> &b) const { return Vector3D<ComponentT>(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x); }

				Vector3D<ComponentT> &DoCrossWith(const Vector3D<ComponentT> &b)
				{
					ComponentT tx = y * b.z - z * b.y; ComponentT ty = z * b.x - x * b.z; ComponentT tz = x * b.y - y * b.x;
					x = tx; y = ty; z = tz;
					return *this;
				}

				Vector3D<ComponentT> GetElemwiseMultiplyWith(const Vector3D<ComponentT> &b) const { return Vector3D<ComponentT>((x * b.x), (y * b.y), (z * b.z)); }

				Vector3D<ComponentT> DoElemwiseMultiplyWith(const Vector3D<ComponentT> &b)
				{
					x *= b.x; y *= b.y; z *= b.y;
					return *this;
				}

				static Vector3D<ComponentT> ElemwiseMultiply(const Vector3D<ComponentT> &a, const Vector3D<ComponentT> &b)
				{
					return Vector3D<ComponentT>((a.x * b.x), (a.y * b.y), (a.z * b.z));
				}

				Vector3D<ComponentT> GetNormalized() const
				{
					if (!IsValid()) { return kInvalid; }
					else if (x == 0.0 && y == 0.0 && z == 0.0) { return kZero; }
					ComponentT length = Length();
					return Vector3D<ComponentT>(x / length, y / length, z / length);
				};

				bool DoNormalize()
				{
					if (!IsValid()) { return false; }
					else if (x == 0.0 && y == 0.0 && z == 0.0) { return false; }
					ComponentT length = Length();
					x /= length; y /= length; z /= length;
					return true;
				}

			public:
				static Vector3D<ComponentT> FromVector2D(const Vector2D<ComponentT> &v2d)
				{
					return Vector3D<ComponentT>(v2d.x, v2d.y, static_cast<ComponentT>(0.0));
				}

			public:
				Vector3D<ComponentT> LerpWith(const Vector3D<ComponentT> &withVector, ComponentT param) const
				{
					return Lerp(*this, withVector, param);
				}

				static Vector3D<ComponentT> Lerp(const Vector3D<ComponentT> &v1, const Vector3D<ComponentT> &v2, ComponentT param)
				{
					return static_cast<ComponentT>(1.0 - param) * v1 + static_cast<ComponentT>(param) * v2;
				}
			};

			template <typename ComponentT, typename ScalarT>
			inline Vector3D<ComponentT> operator*(ScalarT a, const Vector3D<ComponentT> &b) { return Vector3D<ComponentT>(a * b.x, a * b.y, a * b.z); }

			template <typename ComponentT>
			const Vector3D<ComponentT> Vector3D<ComponentT>::kZero(static_cast<ComponentT>(0.0), static_cast<ComponentT>(0.0), static_cast<ComponentT>(0.0));
			template <typename ComponentT>
			const Vector3D<ComponentT> Vector3D<ComponentT>::kInvalid(std::numeric_limits<ComponentT>::quiet_NaN(), std::numeric_limits<ComponentT>::quiet_NaN(), std::numeric_limits<ComponentT>::quiet_NaN());
			template <typename ComponentT>
			const Vector3D<ComponentT> Vector3D<ComponentT>::kX(static_cast<ComponentT>(1.0), static_cast<ComponentT>(0.0), static_cast<ComponentT>(0.0));
			template <typename ComponentT>
			const Vector3D<ComponentT> Vector3D<ComponentT>::kY(static_cast<ComponentT>(0.0), static_cast<ComponentT>(1.0), static_cast<ComponentT>(0.0));
			template <typename ComponentT>
			const Vector3D<ComponentT> Vector3D<ComponentT>::kZ(static_cast<ComponentT>(0.0), static_cast<ComponentT>(0.0), static_cast<ComponentT>(1.0));
			template <typename ComponentT>
			const Vector3D<ComponentT> Vector3D<ComponentT>::kOne(static_cast<ComponentT>(1.0), static_cast<ComponentT>(1.0), static_cast<ComponentT>(1.0));
		}
	}
}

#endif
