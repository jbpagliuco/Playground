#include "Quaternion.h"

#include "Core/Debug/Assert.h"
#include "Core/Math/Matrix.h"

namespace playground
{
	Quaternion::Quaternion() :
		x(0.0f),
		y(0.0f),
		z(0.0f),
		w(1.0f)
	{
	}

	Quaternion::Quaternion(float x, float y, float z, float w) :
		x(x),
		y(y),
		z(z),
		w(w)
	{
		CORE_ASSERT(FEquals(LengthSq(), 1), "Quaternion parameters are not unit length.");
	}

	Quaternion::Quaternion(const Vector3f &axis, float angleInRadians)
	{
		CORE_ASSERT(FEquals(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z, 1), "Quaternion parameters are not unit length.");

		w = cosf(angleInRadians / 2.0f);

		const float sinThetaDiv2 = sinf(angleInRadians / 2.0f);
		x = sinThetaDiv2 * axis.x;
		y = sinThetaDiv2 * axis.y;
		z = sinThetaDiv2 * axis.z;
	}

	Vector3f Quaternion::ToEuler()const
	{
		Vector3f euler;

		euler.x = atan2f(2.0f * (w * x + y * z), 1.0f - 2.0f * (x * x + y * y));
		euler.y = asinf(2.0f * (w * y - z * x));
		euler.z = atan2f(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z));

		return euler;
	}

	Quaternion& Quaternion::Invert()
	{
		x *= -1;
		y *= -1;
		z *= -1;

		return *this;
	}

	float Quaternion::Length()const
	{
		return sqrtf(LengthSq());
	}

	float Quaternion::LengthSq()const
	{
		return x * x + y * y + z * z + w * w;
	}

	Quaternion Quaternion::operator*(const Quaternion &rhs)const
	{
		Quaternion q;

		const Vector v1(x, y, z, 0.0f);
		const Vector v2(rhs.x, rhs.y, rhs.z, 0.0f);

		const float dot = v1.V3Dot(v2);
		const Vector cross = v1.V3Cross(v2);

		q.w = w * rhs.w - dot;
		
		const Vector3f temp = ((v1 * rhs.w) + (v2 * w) + cross).AsVector3();
		q.x = temp.x;
		q.y = temp.y;
		q.z = temp.z;

		return q;
	}

	Quaternion& Quaternion::operator*=(const Quaternion &rhs)
	{
		const Vector v1(x, y, z, 0.0f);
		const Vector v2(rhs.x, rhs.y, rhs.z, 0.0f);

		const float dot = v1.V3Dot(v2);
		const Vector cross = v1.V3Cross(v2);

		w = w * rhs.w - dot;

		const Vector3f temp = ((v1 * rhs.w) + (v2 * w) + cross).AsVector3();
		x = temp.x;
		y = temp.y;
		z = temp.z;

		return *this;
	}

	Vector3f Quaternion::operator*(const Vector3f &rhs)const
	{
		Vector v1(x, y, z, 0.0f);
		Vector v2(rhs.x, rhs.y, rhs.z, 0.0f);

		const Vector v1Crossv2 = v1.V3Cross(v2);
		return (v2 + (v1Crossv2 * (2 * w)) + (v1.V3Cross(v1Crossv2) * 2)).AsVector3();
	}

	Vector4f Quaternion::operator*(const Vector4f &rhs)const
	{
		Vector v1(x, y, z, 0.0f);
		Vector v2(rhs);

		const Vector v1Crossv2 = v1.V3Cross(v2);
		return (v2 + (v1Crossv2 * (2 * w)) + (v1.V3Cross(v1Crossv2) * 2)).AsVector4();
	}

	Vector Quaternion::operator*(const Vector &rhs)const
	{
		Vector v1(x, y, z, 0.0f);

		const Vector v1Crossv2 = v1.V3Cross(rhs);
		return (rhs + (v1Crossv2 * (2 * w)) + (v1.V3Cross(v1Crossv2) * 2));
	}


	Quaternion Quaternion::Lerp(const Quaternion &other, float t)const
	{
		return Interpolate(*this, other, 1.0f - t, t);
	}

	Quaternion Quaternion::Slerp(const Quaternion &other, float t)const
	{
		float theta = acosf(x * other.x + y * other.y + z * other.z + w * other.w);
		float sinTheta = sinf(theta);

		float weightA = sinf((1.0f - t) * theta) / sinTheta;
		float weightB = sinf((t)* theta) / sinTheta;

		return Interpolate(*this, other, weightA, weightB);
	}


	Quaternion Quaternion::Identity()
	{
		Quaternion q;
		q.x = q.y = q.z = 0.0f;
		q.w = 1.0f;

		return q;
	}

	Quaternion Quaternion::FromEuler(float x, float y, float z)
	{
		const Quaternion qX(Vector3f(1.0f, 0.0f, 0.0f), x);
		const Quaternion qY(Vector3f(0.0f, 1.0f, 0.0f), y);
		const Quaternion qZ(Vector3f(0.0f, 0.0f, 1.0f), z);

		return qZ * qY * qX;
	}

	inline float SIGN(float x) { return (x >= 0.0f) ? +1.0f : -1.0f; }
	inline float NORM(float a, float b, float c, float d) { return sqrtf(a * a + b * b + c * c + d * d); }

	Quaternion Quaternion::FromMatrix(const Matrix &rotMatrix)
	{
		Quaternion quat;
		CORE_ALIGN(float R[4][4], 16);
		for (int i = 0; i < 4; i++) {
			_mm_store_ps(R[i], rotMatrix.mCols[i]);
		}

		float q[4];

		q[0] = (R[0][0] + R[1][1] + R[2][2] + 1.0f) / 4.0f;
		q[1] = (R[0][0] - R[1][1] - R[2][2] + 1.0f) / 4.0f;
		q[2] = (-R[0][0] + R[1][1] - R[2][2] + 1.0f) / 4.0f;
		q[3] = (-R[0][0] - R[1][1] + R[2][2] + 1.0f) / 4.0f;
		if (q[0] < 0.0f) q[0] = 0.0f;
		if (q[1] < 0.0f) q[1] = 0.0f;
		if (q[2] < 0.0f) q[2] = 0.0f;
		if (q[3] < 0.0f) q[3] = 0.0f;
		q[0] = sqrtf(q[0]);
		q[1] = sqrtf(q[1]);
		q[2] = sqrtf(q[2]);
		q[3] = sqrtf(q[3]);
		if (q[0] >= q[1] && q[0] >= q[2] && q[0] >= q[3]) {
			q[0] *= +1.0f;
			q[1] *= SIGN(R[2][1] - R[1][2]);
			q[2] *= SIGN(R[0][2] - R[2][0]);
			q[3] *= SIGN(R[1][0] - R[0][1]);
		}
		else if (q[1] >= q[0] && q[1] >= q[2] && q[1] >= q[3]) {
			q[0] *= SIGN(R[2][1] - R[1][2]);
			q[1] *= +1.0f;
			q[2] *= SIGN(R[1][0] + R[0][1]);
			q[3] *= SIGN(R[0][2] + R[2][0]);
		}
		else if (q[2] >= q[0] && q[2] >= q[1] && q[2] >= q[3]) {
			q[0] *= SIGN(R[0][2] - R[2][0]);
			q[1] *= SIGN(R[1][0] + R[0][1]);
			q[2] *= +1.0f;
			q[3] *= SIGN(R[2][1] + R[1][2]);
		}
		else if (q[3] >= q[0] && q[3] >= q[1] && q[3] >= q[2]) {
			q[0] *= SIGN(R[1][0] - R[0][1]);
			q[1] *= SIGN(R[2][0] + R[0][2]);
			q[2] *= SIGN(R[2][1] + R[1][2]);
			q[3] *= +1.0f;
		}

		const float r = NORM(q[0], q[1], q[2], q[3]);
		q[0] /= r;
		q[1] /= r;
		q[2] /= r;
		q[3] /= r;

		quat.w = q[0];
		quat.x = q[1];
		quat.y = q[2];
		quat.z = q[3];

		return quat;
	}


	Quaternion Quaternion::Interpolate(const Quaternion & q1, const Quaternion & q2, float weightA, float weightB)const
	{
		Quaternion q;

		Vector v1(q1.x, q1.y, q1.z, q1.w);
		Vector v2(q2.x, q2.y, q2.z, q2.w);

		v1 *= weightA;
		v2 *= weightB;

		v1 += v2;

		v1.V4Normalize();

		CORE_ALIGN(float vector[4], 16);
		_mm_store_ps(vector, v1.mElems);

		q.x = vector[0];
		q.y = vector[1];
		q.z = vector[2];
		q.w = vector[3];

		return q;
	}
}