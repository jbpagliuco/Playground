#include "Vector.h"

namespace playground
{
	void m128abs(__m128 &vector)
	{
		const __m128 vNegZero = _mm_set_ps1(-0.0f); // 0x80000000

		vector = _mm_andnot_ps(vNegZero, vector);
	}

	void m128cmp(const __m128 &first, const __m128 &second, float epsilon, float pOutData[4])
	{
		__m128 vSub = _mm_sub_ps(first, second);

		__m128 vAbsResult;
		m128abs(vAbsResult);
		__m128 vEpsilon = _mm_set_ps1(epsilon);

		__m128 vResult = _mm_cmple_ps(vAbsResult, vEpsilon);
		_mm_store_ps(pOutData, vResult);
	}


	Vector::Vector()
	{

	}

	Vector::Vector(float all)
	{
		mElems = _mm_set_ps1(all);
	}

	Vector::Vector(float x, float y, float z, float w)
	{
		CORE_ALIGN(float v[4], 16) = { x, y, z, w };
		mElems = _mm_load_ps(v);
	}

	Vector::Vector(const Vector2f &vector)
	{
		CORE_ALIGN(float v[4], 16) = { vector.x, vector.y, 0.0f, 0.0f };
		mElems = _mm_load_ps(v);
	}

	Vector::Vector(const Vector3f &vector)
	{
		CORE_ALIGN(float v[4], 16) = { vector.x, vector.y, vector.z, 0.0f };
		mElems = _mm_load_ps(v);
	}

	Vector::Vector(const Vector4f &vector)
	{
		CORE_ALIGN(float v[4], 16) = { vector.x, vector.y, vector.z, vector.w };
		mElems = _mm_load_ps(v);
	}

	Vector::Vector(__m128 vector)
	{
		mElems = vector;
	}



	Vector Vector::Zero()
	{
		return Vector(_mm_setzero_ps());
	}



	Vector2f Vector::AsVector2()const
	{
		CORE_ALIGN(float vector[4], 16);
		_mm_store_ps(vector, mElems);
		return Vector2f(vector);
	}

	Vector3f Vector::AsVector3()const
	{
		CORE_ALIGN(float vector[4], 16);
		_mm_store_ps(vector, mElems);
		return Vector3f(vector);
	}

	Vector4f Vector::AsVector4()const
	{
		CORE_ALIGN(float vector[4], 16);
		_mm_store_ps(vector, mElems);
		return Vector4f(vector);
	}


	Tuple2f Vector::AsTuple2()const
	{
		CORE_ALIGN(float vector[4], 16);
		_mm_store_ps(vector, mElems);
		return Tuple2f(vector[0], vector[1]);
	}

	Tuple3f Vector::AsTuple3()const
	{
		CORE_ALIGN(float vector[4], 16);
		_mm_store_ps(vector, mElems);
		return Tuple3f(vector[0], vector[1], vector[2]);
	}

	Tuple4f Vector::AsTuple4()const
	{
		CORE_ALIGN(float vector[4], 16);
		_mm_store_ps(vector, mElems);
		return Tuple4f(vector[0], vector[1], vector[2], vector[3]);
	}




	float Vector::GetX()const
	{
		return _mm_cvtss_f32(mElems);
	}

	float Vector::GetY()const
	{
		__m128 v = _mm_shuffle_ps(mElems, mElems, _MM_SHUFFLE(0, 0, 0, 1));
		return _mm_cvtss_f32(v);
	}

	float Vector::GetZ()const
	{
		__m128 v = _mm_shuffle_ps(mElems, mElems, _MM_SHUFFLE(0, 0, 0, 2));
		return _mm_cvtss_f32(v);
/*
		__declspec(align(16)) float v[4];
		_mm_store_ps(v, mElems);

		return v[2];*/
	}

	float Vector::GetW()const
	{
		__m128 v = _mm_shuffle_ps(mElems, mElems, _MM_SHUFFLE(0, 0, 0, 3));
		return _mm_cvtss_f32(v);
	}





	Vector2f Vector::V2Equals(const Vector &other, float epsilon)const
	{
		CORE_ALIGN(float vResult[4], 16);
		m128cmp(mElems, other.mElems, epsilon, vResult);

		return Vector2f(vResult[0] != 0, vResult[1] != 0);
	}

	Vector2f Vector::V2NotEquals(const Vector &other, float epsilon)const
	{
		CORE_ALIGN(float vResult[4], 16);
		m128cmp(mElems, other.mElems, epsilon, vResult);

		return Vector2f(vResult[0] == 0, vResult[1] == 0);
	}

	Vector3f Vector::V3Equals(const Vector &other, float epsilon)const
	{
		CORE_ALIGN(float vResult[4], 16);
		m128cmp(mElems, other.mElems, epsilon, vResult);

		return Vector3f(vResult[0] != 0, vResult[1] != 0, vResult[2] != 0);
	}

	Vector3f Vector::V3NotEquals(const Vector &other, float epsilon)const
	{
		CORE_ALIGN(float vResult[4], 16);
		m128cmp(mElems, other.mElems, epsilon, vResult);

		return Vector3f(vResult[0] == 0, vResult[1] == 0, vResult[2] == 0);
	}

	Vector4f Vector::V4Equals(const Vector &other, float epsilon)const
	{
		CORE_ALIGN(float vResult[4], 16);
		m128cmp(mElems, other.mElems, epsilon, vResult);

		return Vector4f(vResult[0] != 0, vResult[1] != 0, vResult[2] != 0, vResult[3] != 0);
	}

	Vector4f Vector::V4NotEquals(const Vector &other, float epsilon)const
	{
		CORE_ALIGN(float vResult[4], 16);
		m128cmp(mElems, other.mElems, epsilon, vResult);

		return Vector4f(vResult[0] == 0, vResult[1] == 0, vResult[2] == 0, vResult[3] == 0);
	}





	Vector2f Vector::V2Less(const Vector &other)const
	{
		__m128 vResult = _mm_cmplt_ps(mElems, other.mElems);
		CORE_ALIGN(float f[4], 16);
		_mm_store_ps(f, vResult);

		return Vector2f(f[0] != 0, f[1] != 0);
	}

	Vector2f Vector::V2LessEq(const Vector &other)const
	{
		__m128 vResult = _mm_cmple_ps(mElems, other.mElems);
		CORE_ALIGN(float f[4], 16);
		_mm_store_ps(f, vResult);

		return Vector2f(f[0] != 0, f[1] != 0);
	}

	Vector2f Vector::V2Greater(const Vector &other)const
	{
		__m128 vResult = _mm_cmpgt_ps(mElems, other.mElems);
		CORE_ALIGN(float f[4], 16);
		_mm_store_ps(f, vResult);

		return Vector2f(f[0] != 0, f[1] != 0);
	}

	Vector2f Vector::V2GreaterEq(const Vector &other)const
	{
		__m128 vResult = _mm_cmpge_ps(mElems, other.mElems);
		CORE_ALIGN(float f[4], 16);
		_mm_store_ps(f, vResult);

		return Vector2f(f[0] != 0, f[1] != 0);
	}

	Vector3f Vector::V3Less(const Vector &other)const
	{
		__m128 vResult = _mm_cmplt_ps(mElems, other.mElems);
		CORE_ALIGN(float f[4], 16);
		_mm_store_ps(f, vResult);

		return Vector3f(f[0] != 0, f[1] != 0, f[2] != 0);
	}

	Vector3f Vector::V3LessEq(const Vector &other)const
	{
		__m128 vResult = _mm_cmple_ps(mElems, other.mElems);
		CORE_ALIGN(float f[4], 16);
		_mm_store_ps(f, vResult);

		return Vector3f(f[0] != 0, f[1] != 0, f[2] != 0);
	}

	Vector3f Vector::V3Greater(const Vector &other)const
	{
		__m128 vResult = _mm_cmpgt_ps(mElems, other.mElems);
		CORE_ALIGN(float f[4], 16);
		_mm_store_ps(f, vResult);

		return Vector3f(f[0] != 0, f[1] != 0, f[2] != 0);
	}

	Vector3f Vector::V3GreaterEq(const Vector &other)const
	{
		__m128 vResult = _mm_cmpge_ps(mElems, other.mElems);
		CORE_ALIGN(float f[4], 16);
		_mm_store_ps(f, vResult);

		return Vector3f(f[0] != 0, f[1] != 0, f[2] != 0);
	}

	Vector4f Vector::V4Less(const Vector &other)const
	{
		__m128 vResult = _mm_cmplt_ps(mElems, other.mElems);
		CORE_ALIGN(float f[4], 16);
		_mm_store_ps(f, vResult);

		return Vector4f(f[0] != 0, f[1] != 0, f[2] != 0, f[3] != 0);
	}

	Vector4f Vector::V4LessEq(const Vector &other)const
	{
		__m128 vResult = _mm_cmple_ps(mElems, other.mElems);
		CORE_ALIGN(float f[4], 16);
		_mm_store_ps(f, vResult);

		return Vector4f(f[0] != 0, f[1] != 0, f[2] != 0, f[3] != 0);
	}

	Vector4f Vector::V4Greater(const Vector &other)const
	{
		__m128 vResult = _mm_cmpgt_ps(mElems, other.mElems);
		CORE_ALIGN(float f[4], 16);
		_mm_store_ps(f, vResult);

		return Vector4f(f[0] != 0, f[1] != 0, f[2] != 0, f[3] != 0);
	}

	Vector4f Vector::V4GreaterEq(const Vector &other)const
	{
		__m128 vResult = _mm_cmpge_ps(mElems, other.mElems);
		CORE_ALIGN(float f[4], 16);
		_mm_store_ps(f, vResult);

		return Vector4f(f[0] != 0, f[1] != 0, f[2] != 0, f[3] != 0);
	}





	Vector Vector::operator+(const Vector &other)const
	{
		return Vector(_mm_add_ps(mElems, other.mElems));
	}

	Vector & Vector::operator+=(const Vector &other)
	{
		mElems = _mm_add_ps(mElems, other.mElems);
		return *this;
	}

	Vector Vector::operator-(const Vector &other)const
	{
		return Vector(_mm_sub_ps(mElems, other.mElems));
	}

	Vector & Vector::operator-=(const Vector &other)
	{
		mElems = _mm_sub_ps(mElems, other.mElems);
		return *this;
	}

	Vector Vector::operator*(const Vector &other)const
	{
		return Vector(_mm_mul_ps(mElems, other.mElems));
	}

	Vector Vector::operator*=(const Vector &other)
	{
		mElems = _mm_mul_ps(mElems, other.mElems);
		return *this;
	}

	Vector Vector::operator*(float scalar)const
	{
		__m128 scalarAsVector = _mm_set_ps1(scalar);
		return Vector(_mm_mul_ps(mElems, scalarAsVector));
	}

	Vector Vector::operator*=(float scalar)
	{
		__m128 scalarAsVector = _mm_set_ps1(scalar);
		mElems = _mm_mul_ps(mElems, scalarAsVector);
		return *this;
	}

	Vector Vector::operator/(const Vector &other)const
	{
		return Vector(_mm_div_ps(mElems, other.mElems));
	}

	Vector Vector::operator/=(const Vector &other)
	{
		mElems = _mm_div_ps(mElems, other.mElems);
		return *this;
	}

	Vector Vector::operator/(float scalar)const
	{
		__m128 scalarAsVector = _mm_set_ps1(scalar);
		return Vector(_mm_div_ps(mElems, scalarAsVector));
	}

	Vector Vector::operator/=(float scalar)
	{
		__m128 scalarAsVector = _mm_set_ps1(scalar);
		mElems = _mm_div_ps(mElems, scalarAsVector);
		return *this;
	}





	Vector & Vector::AbsoluteValue()
	{
		m128abs(mElems);

		return *this;
	}

	Vector & Vector::Negate()
	{
		mElems = _mm_sub_ps(_mm_set_ps1(0.0f), mElems);

		return *this;
	}

	Vector Vector::Lerp(const Vector &other, float t)const
	{
		Vector v1 = *this;
		Vector v2 = other;

		v1 *= (1.0f - t);
		v2 *= t;

		v1 += v2;

		return v1;
	}




	float Vector::V2Dot(const Vector &other)const
	{
		__m128 vMul = _mm_mul_ps(mElems, other.mElems);
		__m128 vMulYXZW = _mm_shuffle_ps(vMul, vMul, _MM_SHUFFLE(3, 2, 0, 1));
		__m128 vResult = _mm_add_ps(vMul, vMulYXZW);

		return _mm_cvtss_f32(vResult);
	}

	float Vector::V3Dot(const Vector &other)const
	{
		__m128 vMul = _mm_mul_ps(mElems, other.mElems);
		__m128 vMulYXXX = _mm_shuffle_ps(vMul, vMul, _MM_SHUFFLE(0, 0, 0, 1));
		__m128 vMulZXXX = _mm_shuffle_ps(vMul, vMul, _MM_SHUFFLE(0, 0, 0, 2));
		__m128 vResult = _mm_add_ps(vMul, vMulYXXX);
		vResult = _mm_add_ps(vResult, vMulZXXX);

		return _mm_cvtss_f32(vResult);
	}

	float Vector::V4Dot(const Vector &other)const
	{
		__m128 vMul = _mm_mul_ps(mElems, other.mElems);
		__m128 vMulWZYX = _mm_shuffle_ps(vMul, vMul, _MM_SHUFFLE(0, 1, 2, 3));
		__m128 vResult = _mm_add_ps(vMul, vMulWZYX);
		__m128 vResultYXXX = _mm_shuffle_ps(vResult, vResult, _MM_SHUFFLE(0, 0, 0, 1));
		vResult = _mm_add_ps(vResult, vResultYXXX);

		return _mm_cvtss_f32(vResult);
	}





	float Vector::V2Length()const
	{
		__m128 vMul = _mm_mul_ps(mElems, mElems);
		__m128 vMulYXZW = _mm_shuffle_ps(vMul, vMul, _MM_SHUFFLE(3, 2, 0, 1));
		__m128 vResult = _mm_add_ps(vMul, vMulYXZW);

		return _mm_cvtss_f32(_mm_sqrt_ss(vResult));
	}

	float Vector::V2LengthSq()const
	{
		__m128 vMul = _mm_mul_ps(mElems, mElems);
		__m128 vMulYXZW = _mm_shuffle_ps(vMul, vMul, _MM_SHUFFLE(3, 2, 0, 1));
		__m128 vResult = _mm_add_ps(vMul, vMulYXZW);

		return _mm_cvtss_f32(vResult);
	}

	float Vector::V3Length()const
	{
		__m128 vMul = _mm_mul_ps(mElems, mElems);
		__m128 vMulYXXX = _mm_shuffle_ps(vMul, vMul, _MM_SHUFFLE(0, 0, 0, 1));
		__m128 vMulZXXX = _mm_shuffle_ps(vMul, vMul, _MM_SHUFFLE(0, 0, 0, 2));
		__m128 vResult = _mm_add_ps(vMul, vMulYXXX);
		vResult = _mm_add_ps(vResult, vMulZXXX);

		return _mm_cvtss_f32(_mm_sqrt_ss(vResult));
	}

	float Vector::V3LengthSq()const
	{
		__m128 vMul = _mm_mul_ps(mElems, mElems);
		__m128 vMulYXXX = _mm_shuffle_ps(vMul, vMul, _MM_SHUFFLE(0, 0, 0, 1));
		__m128 vMulZXXX = _mm_shuffle_ps(vMul, vMul, _MM_SHUFFLE(0, 0, 0, 2));
		__m128 vResult = _mm_add_ps(vMul, vMulYXXX);
		vResult = _mm_add_ps(vResult, vMulZXXX);

		return _mm_cvtss_f32(vResult);
	}

	float Vector::V4Length()const
	{
		__m128 vMul = _mm_mul_ps(mElems, mElems);
		__m128 vMulWZYX = _mm_shuffle_ps(vMul, vMul, _MM_SHUFFLE(0, 1, 2, 3));
		__m128 vResult = _mm_add_ps(vMul, vMulWZYX);
		__m128 vResultYXXX = _mm_shuffle_ps(vResult, vResult, _MM_SHUFFLE(0, 0, 0, 1));
		vResult = _mm_add_ps(vResult, vResultYXXX);

		return _mm_cvtss_f32(_mm_sqrt_ss(vResult));
	}

	float Vector::V4LengthSq()const
	{
		__m128 vMul = _mm_mul_ps(mElems, mElems);
		__m128 vMulWZYX = _mm_shuffle_ps(vMul, vMul, _MM_SHUFFLE(0, 1, 2, 3));
		__m128 vResult = _mm_add_ps(vMul, vMulWZYX);
		__m128 vResultYXXX = _mm_shuffle_ps(vResult, vResult, _MM_SHUFFLE(0, 0, 0, 1));
		vResult = _mm_add_ps(vResult, vResultYXXX);

		return _mm_cvtss_f32(vResult);
	}





	Vector & Vector::V2Normalize()
	{
		const float length = V2Length();

		__m128 vLength = _mm_set_ps1(length); // Could make reciprocal?
		mElems = _mm_div_ps(mElems, vLength);

		return *this;
	}

	Vector & Vector::V3Normalize()
	{
		const float length = V3Length();

		__m128 vLength = _mm_set_ps1(length); // Could make reciprocal?
		mElems = _mm_div_ps(mElems, vLength);

		return *this;
	}

	Vector & Vector::V4Normalize()
	{
		const float length = V4Length();

		__m128 vLength = _mm_set_ps1(length); // Could make reciprocal?
		mElems = _mm_div_ps(mElems, vLength);

		return *this;
	}






	Vector Vector::V3Cross(const Vector &other)const
	{
		__m128 result = _mm_sub_ps(
			_mm_mul_ps(mElems, _mm_shuffle_ps(other.mElems, other.mElems, _MM_SHUFFLE(3, 0, 2, 1))),
			_mm_mul_ps(other.mElems, _mm_shuffle_ps(mElems, mElems, _MM_SHUFFLE(3, 0, 2, 1))));

		return Vector(_mm_shuffle_ps(result, result, _MM_SHUFFLE(3, 0, 2, 1)));
	}





	float Vector::V2AngleBetween(const Vector &other)const
	{
		return acosf(V2Dot(other) / (V2Length() * other.V2Length()));
	}

	float Vector::V2AngleBetweenNormals(const Vector &other)const
	{
		return acosf(V2Dot(other));
	}

	float Vector::V3AngleBetween(const Vector &other)const
	{
		return acosf(V3Dot(other) / (V3Length() * other.V3Length()));
	}

	float Vector::V3AngleBetweenNormals(const Vector &other)const
	{
		return acosf(V3Dot(other));
	}

	float Vector::V4AngleBetween(const Vector &other)const
	{
		return acosf(V4Dot(other) / (V4Length() * other.V4Length()));
	}

	float Vector::V4AngleBetweenNormals(const Vector &other)const
	{
		return acosf(V4Dot(other));
	}



	std::string Vector::ToString()const
	{
		const Vector4f me = AsVector4();

		char temp[512];
		_snprintf_s(temp, 512, "(%.4f, %.4f, %.4f, %.4f)", me.x, me.y, me.z, me.w);

		return std::string(temp);
	}
}