#pragma once

#include <string>

#ifndef _MSC_VER
#error SIMD vector math not implemented for this configuration
#endif

#include <xmmintrin.h>

#include "MathUtil.h"

#include "Core/Memory/Memory.h"
#include "Core/Util/Util.h"

#define CORE_MM_SHUFFLE_PARAM(x, y, z, w) ((x) | (y << 2) | (z << 4) | (w << 6))

#define CORE_MM_REPLICATE_X_PS(v) _mm_shuffle_ps((v), (v), CORE_MM_SHUFFLE_PARAM(0, 0, 0, 0))
#define CORE_MM_REPLICATE_Y_PS(v) _mm_shuffle_ps((v), (v), CORE_MM_SHUFFLE_PARAM(1, 1, 1, 1))
#define CORE_MM_REPLICATE_Z_PS(v) _mm_shuffle_ps((v), (v), CORE_MM_SHUFFLE_PARAM(2, 2, 2, 2))
#define CORE_MM_REPLICATE_W_PS(v) _mm_shuffle_ps((v), (v), CORE_MM_SHUFFLE_PARAM(3, 3, 3, 3))

#define CORE_MM_MADD_PS(v1, v2, v3) _mm_add_ps(_mm_mul_ps((v1), (v2)), (v3))

namespace playground
{
	// 2D vector of 32 bit single floating point components.
	CORE_ALIGN_MS(16) struct Vector2f
	{
	public:
		Vector2f() = default;
		Vector2f(const Vector2f &other) = default;
		Vector2f(float _x, float _y) : x(_x), y(_y) { }
		explicit Vector2f(const float *pDataArray) : x(pDataArray[0]), y(pDataArray[1]) { }

		Tuple2f AsTuple()const { return Tuple2f(x, y); }
		
		Vector2f& operator=(const Vector2f &rhs) = default;

		bool operator==(const Vector2f &rhs) { return FEquals(x, rhs.x) && FEquals(y, rhs.y); }
		bool operator!=(const Vector2f &rhs) { return !FEquals(x, rhs.x) || !FEquals(y, rhs.y); }

		Vector2f operator+(const Vector2f &rhs) { return Vector2f(x + rhs.x, y + rhs.y); }
		Vector2f operator-(const Vector2f &rhs) { return Vector2f(x - rhs.x, y - rhs.y); }
		Vector2f operator*(const Vector2f &rhs) { return Vector2f(x * rhs.x, y * rhs.y); }
		Vector2f operator*(float c) { return Vector2f(x * c, y * c); }

		float operator[](int index) { return v[index]; }

	public:
		union {
			struct {
				float x;
				float y;
			};

			float v[2];
		};
	} CORE_ALIGN_GCC(16);

	// 3D vector of 32 bit single floating point components.
	CORE_ALIGN_MS(16) struct Vector3f
	{
		Vector3f() = default;
		Vector3f(const Vector3f &other) = default;
		Vector3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }
		explicit Vector3f(const float *pDataArray) : x(pDataArray[0]), y(pDataArray[1]), z(pDataArray[2]) { }

		Tuple3f AsTuple()const { return Tuple3f(x, y, z); }

		Vector3f& operator=(const Vector3f &rhs) = default;

		bool operator==(const Vector3f &rhs) { return FEquals(x, rhs.x) && FEquals(y, rhs.y) && FEquals(z, rhs.z); }
		bool operator!=(const Vector3f &rhs) { return !FEquals(x, rhs.x) || !FEquals(y, rhs.y) || !FEquals(z, rhs.z); }

		Vector3f operator+(const Vector3f &rhs) { return Vector3f(x + rhs.x, y + rhs.y, z + rhs.z); }
		Vector3f operator-(const Vector3f &rhs) { return Vector3f(x - rhs.x, y - rhs.y, z - rhs.z); }
		Vector3f operator*(const Vector3f &rhs) { return Vector3f(x * rhs.x, y * rhs.y, z * rhs.z); }
		Vector3f operator*(float c) { return Vector3f(x * c, y * c, z * c); }

		float operator[](int index) { return v[index]; }

	public:
		union {
			struct {
				float x;
				float y;
				float z;
			};

			float v[3];
		};
	} CORE_ALIGN_GCC(16);

	// 4D vector of 32 bit single floating point components.
	CORE_ALIGN_MS(16) struct Vector4f
	{
		Vector4f() = default;
		Vector4f(const Vector4f &other) = default;
		Vector4f(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) { }
		explicit Vector4f(const float *pDataArray) : x(pDataArray[0]), y(pDataArray[1]), z(pDataArray[2]), w(pDataArray[3]) { }

		Tuple4f AsTuple()const { return Tuple4f(x, y, z, w); }

		Vector4f& operator=(const Vector4f &rhs) = default;

		bool operator==(const Vector4f &rhs) { return FEquals(x, rhs.x) && FEquals(y, rhs.y) && FEquals(z, rhs.z) && FEquals(w, rhs.w); }
		bool operator!=(const Vector4f &rhs) { return !FEquals(x, rhs.x) || !FEquals(y, rhs.y) || !FEquals(z, rhs.z) || !FEquals(w, rhs.w); }

		Vector4f operator+(const Vector4f &rhs) { return Vector4f(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }
		Vector4f operator-(const Vector4f &rhs) { return Vector4f(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); }
		Vector4f operator*(const Vector4f &rhs) { return Vector4f(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w); }
		Vector4f operator*(float c) { return Vector4f(x * c, y * c, z * c, w * c); }

		float operator[](int index) { return v[index]; }

	public:
		union {
			struct {
				float x;
				float y;
				float z;
				float w;
			};

			float v[4];
		};
	} CORE_ALIGN_GCC(16);



	CORE_ALIGN_MS(16) class Vector
	{
	public:
		Vector();
		// Creates a vector with all component set equal to "all".
		explicit Vector(float all);
		// Creates a vector with the four components (x, y, z, w).
		Vector(float x, float y, float z, float w);
		// Creates a vector from 2D vector.
		explicit Vector(const Vector2f &vector);
		// Creates a vector from 3D vector.
		explicit Vector(const Vector3f &vector);
		// Creates a vector from 4D vector.
		explicit Vector(const Vector4f &vector);

		explicit Vector(__m128 vector);

		
		static Vector Zero();


		// Stores the data into a Vector2f object.
		Vector2f AsVector2()const;
		// Stores the data into a Vector3f object.
		Vector3f AsVector3()const;
		// Stores the data into a Vector4f object.
		Vector4f AsVector4()const;

		// Stores the data into a Tuple2f object.
		Tuple2f AsTuple2()const;
		// Stores the data into a Tuple3f object.
		Tuple3f AsTuple3()const;
		// Stores the data into a Tuple4f object.
		Tuple4f AsTuple4()const;


		// Get the x component.
		float GetX()const;
		// Get the y component.
		float GetY()const;
		// Get the z component.
		float GetZ()const;
		// Get the w component.
		float GetW()const;


		// Adds the two vectors component wise.
		Vector operator+(const Vector &other)const;
		// Adds the two vectors component wise and sets the result to this.
		Vector & operator+=(const Vector &other);

		// Subtracts the two vectors component wise.
		Vector operator-(const Vector &other)const;
		// Subtracts the two vectors component wise and sets the result to this.
		Vector & operator-=(const Vector &other);

		// Multiplies the two vectors component wise.
		Vector operator*(const Vector &other)const;
		// Multiplies the two vectors component wise and sets the result to this.
		Vector operator*=(const Vector &other);

		// Multiplies each component by the scalar.
		Vector operator*(float scalar)const;
		// Multiplies each component by the scalar and sets the result equal to this.
		Vector operator*=(float scalar);

		// Divides the two vectors component wise.
		Vector operator/(const Vector &other)const;
		// Divides the two vectors component wise ands sets the result equal to this.
		Vector operator/=(const Vector &other);

		// Divides each component by the scalar.
		Vector operator/(float scalar)const;
		// Divides each component by the scalar and sets the result equal to this.
		Vector operator/=(float scalar);


		// Sets this vector to the absolute value of itself.
		// Returns a reference to this vector.
		Vector & AbsoluteValue();
		// Negates this vector.
		// Returns a reference to this vector.
		Vector & Negate();
		// Linearly interpolates the two vectors by the amount t and returns the resulting vector.
		Vector Lerp(const Vector &other, float t)const;



		// Returns a string representation of this vector.
		std::string ToString()const;




		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 2D Vector OPERATIONS ////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//// COMPARISONS ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Checks for equality (within epsilon) of the 2D vectors. Returns a 1.0f for equals or a 0.0f for not equals in the respective component.
		Vector2f V2Equals(const Vector &other, float epsilon = FLOAT_EPSILON)const;
		// Checks for equality (within epsilon) of the 2D vectors. Returns a 1.0f for not equals or a 0.0f for equals in the respective component.
		Vector2f V2NotEquals(const Vector &other, float epsilon = FLOAT_EPSILON)const;

		// Compares the 2D vectors. Returns a 1.0f for a lesser value or a 0.0f otherwise in the respective component.
		Vector2f V2Less(const Vector &other)const;
		// Compares the 2D vectors. Returns a 1.0f for a lesser or equal value or a 0.0f otherwise in the respective component.
		Vector2f V2LessEq(const Vector &other)const;
		// Compares the 2D vectors. Returns a 1.0f for a greater value or a 0.0f otherwise in the respective component.
		Vector2f V2Greater(const Vector &other)const;
		// Compares the 2D vectors. Returns a 1.0f for a greater equal value or a 0.0f otherwise in the respective component.
		Vector2f V2GreaterEq(const Vector &other)const;


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//// VECTOR OPERATIONS /////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Calculates the dot product of the two 2D vectors.
		float V2Dot(const Vector &other)const;

		// Calculates the length of the 2D vector.
		float V2Length()const;
		// Calculates the squared length of the 2D vector.
		float V2LengthSq()const;

		// Normalizes this 2D vector.
		// Returns a reference to this vector.
		Vector & V2Normalize();

		// Calculates the angle (in radians) between the two 2D vectors. If both vectors are of unit length, use V2AngleBetweenNormals
		// for speed.
		float V2AngleBetween(const Vector &other)const;
		// Calculates the angle (in radians) between the two unit 2D vectors.
		float V2AngleBetweenNormals(const Vector &other)const;






		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 3D Vector OPERATIONS ////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//// COMPARISONS ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Checks for equality (within epsilon) of the 3D vectors. Returns a 1.0f for equals or a 0.0f for not equals in the respective component.
		Vector3f V3Equals(const Vector &other, float epsilon = FLOAT_EPSILON)const;
		// Checks for equality (within epsilon) of the 3D vectors. Returns a 1.0f for not equals or a 0.0f for equals in the respective component.
		Vector3f V3NotEquals(const Vector &other, float epsilon = FLOAT_EPSILON)const;

		// Compares the 3D vectors. Returns a 1.0f for a lesser value or a 0.0f otherwise in the respective component.
		Vector3f V3Less(const Vector &other)const;
		// Compares the 3D vectors. Returns a 1.0f for a lesser or equal value or a 0.0f otherwise in the respective component.
		Vector3f V3LessEq(const Vector &other)const;
		// Compares the 3D vectors. Returns a 1.0f for a greater value or a 0.0f otherwise in the respective component.
		Vector3f V3Greater(const Vector &other)const;
		// Compares the 3D vectors. Returns a 1.0f for a greater equal value or a 0.0f otherwise in the respective component.
		Vector3f V3GreaterEq(const Vector &other)const;


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//// VECTOR OPERATIONS /////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Calculates the dot product of the two 3D vectors.
		float V3Dot(const Vector &other)const;

		// Calculates the length of the 3D vector.
		float V3Length()const;
		// Calculates the squared length of the 3D vector.
		float V3LengthSq()const;

		// Normalizes this 3D vector.
		// Returns a reference to this vector.
		Vector & V3Normalize();

		// Calculates a vector that is perpendicular to both vectors.
		Vector V3Cross(const Vector &other)const;

		// Calculates the angle (in radians) between the two 3D vectors. If both vectors are of unit length, use V3AngleBetweenNormals
		// for speed.
		float V3AngleBetween(const Vector &other)const;
		// Calculates the angle (in radians) between the two unit 3D vectors.
		float V3AngleBetweenNormals(const Vector &other)const;






		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 4D Vector OPERATIONS ////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//// COMPARISONS ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Checks for equality (within epsilon) of the 4D vectors. Returns a 1.0f for equals or a 0.0f for not equals in the respective component.
		Vector4f V4Equals(const Vector &other, float epsilon = FLOAT_EPSILON)const;
		// Checks for equality (within epsilon) of the 4D vectors. Returns a 1.0f for not equals or a 0.0f for equals in the respective component.
		Vector4f V4NotEquals(const Vector &other, float epsilon = FLOAT_EPSILON)const;

		// Compares the 4D vectors. Returns a 1.0f for a lesser value or a 0.0f otherwise in the respective component.
		Vector4f V4Less(const Vector &other)const;
		// Compares the 4D vectors. Returns a 1.0f for a lesser or equal value or a 0.0f otherwise in the respective component.
		Vector4f V4LessEq(const Vector &other)const;
		// Compares the 4D vectors. Returns a 1.0f for a greater value or a 0.0f otherwise in the respective component.
		Vector4f V4Greater(const Vector &other)const;
		// Compares the 4D vectors. Returns a 1.0f for a greater equal value or a 0.0f otherwise in the respective component.
		Vector4f V4GreaterEq(const Vector &other)const;


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//// VECTOR OPERATIONS /////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Calculates the dot product of the two 4D vectors.
		float V4Dot(const Vector &other)const;

		// Calculates the length of the 4D vector.
		float V4Length()const;
		// Calculates the squared length of the 4D vector.
		float V4LengthSq()const;

		// Normalizes this 4D vector.
		// Returns a reference to this vector.
		Vector & V4Normalize();

		// Calculates the angle (in radians) between the two 4D vectors. If both vectors are of unit length, use V4AngleBetweenNormals
		// for speed.
		float V4AngleBetween(const Vector &other)const;
		// Calculates the angle (in radians) between the two unit 4D vectors.
		float V4AngleBetweenNormals(const Vector &other)const;

	public:
		__m128 mElems;
	} CORE_ALIGN_GCC(16);

	void m128abs(__m128 &vector);
	void m128cmp(const __m128 &first, const __m128 &second, float epsilon, float pOutData[4]);
}