#include "Matrix.h"

#include <stdint.h>

#include "Core/Debug/Assert.h"
#include "Core/Math/Quaternion.h"

namespace playground
{
	Matrix::Matrix()
	{
	}

	Matrix::Matrix(const float pData[16])
	{
		CORE_ASSERT((0x0000000F & (uintptr_t)pData) == 0, "Matrix data is not aligned to 16 bytes"); // Check to see if aligned to a 16

		for (int i = 0; i < 4; i++) {
			mCols[i] = _mm_load_ps(&pData[4 * i]);
		}
	}

	Matrix::Matrix(const Vector &col1, const Vector &col2, const Vector &col3, const Vector &col4)
	{
		mCols[0] = col1.mElems;
		mCols[1] = col2.mElems;
		mCols[2] = col3.mElems;
		mCols[3] = col4.mElems;
	}



	void Matrix::AsFloats(float mat[4][4])const
	{
		for (int i = 0; i < 4; i++) {
			_mm_store_ps(mat[i], mCols[i]);
		}
	}

	void Matrix::AsFloats(float mat[16])const
	{
		for (int i = 0; i < 4; i++) {
			_mm_store_ps(&(mat[i * 4]), mCols[i]);
		}
	}
	


	float Matrix::operator()(int x, int y)const
	{
		CORE_ASSERT_RETURN_VALUE(x >= 0 && x <= 3, 0.0f, "Invalid X coordinate (%d)", x); // Check x bounds.
		CORE_ASSERT_RETURN_VALUE(y >= 0 && y <= 3, 0.0f, "Invalid Y coordinate (%d)", y); // Check y bounds.

		CORE_ALIGN(float col[4], 16);
		_mm_store_ps(col, mCols[y]);

		return col[x];
	}





	Vector Matrix::GetColumn(int col)const
	{
		return Vector(mCols[col]);
	}





	Matrix Matrix::Equals(const Matrix &other, float epsilon)const
	{
		Matrix result;

		for (int i = 0; i < 4; i++) {
			CORE_ALIGN(float v[4], 16);
			m128cmp(mCols[i], other.mCols[i], epsilon, v);

			for (int j = 0; j < 4; j++) {
				v[i] = (float)(v[i] != 0);
			}
			result.mCols[i] = _mm_load_ps(v);
		}

		return result;
	}

	Matrix Matrix::NotEquals(const Matrix &other, float epsilon)const
	{
		Matrix result;

		for (int i = 0; i < 4; i++) {
			CORE_ALIGN(float v[4], 16);
			m128cmp(mCols[i], other.mCols[i], epsilon, v);

			for (int j = 0; j < 4; j++) {
				v[i] = (float)(v[i] == 0);
			}

			result.mCols[i] = _mm_load_ps(v);
		}

		return result;
	}





	Matrix Matrix::Less(const Matrix &other)const
	{
		Matrix result;

		for (int i = 0; i < 4; i++) {
			CORE_ALIGN(float v[4], 16);
			__m128 vResult = _mm_cmplt_ps(mCols[i], other.mCols[i]);

			for (int j = 0; j < 4; j++) {
				v[i] = (float)(v[i] != 0);
			}

			result.mCols[i] = _mm_load_ps(v);
		}

		return result;
	}

	Matrix Matrix::LessEq(const Matrix &other)const
	{
		Matrix result;

		for (int i = 0; i < 4; i++) {
			CORE_ALIGN(float v[4], 16);
			__m128 vResult = _mm_cmple_ps(mCols[i], other.mCols[i]);

			for (int j = 0; j < 4; j++) {
				v[i] = (float)(v[i] != 0);
			}

			result.mCols[i] = _mm_load_ps(v);
		}

		return result;
	}

	Matrix Matrix::Greater(const Matrix &other)const
	{
		Matrix result;

		for (int i = 0; i < 4; i++) {
			CORE_ALIGN(float v[4], 16);
			__m128 vResult = _mm_cmpgt_ps(mCols[i], other.mCols[i]);

			for (int j = 0; j < 4; j++) {
				v[i] = (float)(v[i] != 0);
			}

			result.mCols[i] = _mm_load_ps(v);
		}

		return result;
	}

	Matrix Matrix::GreaterEq(const Matrix &other)const
	{
		Matrix result;

		for (int i = 0; i < 4; i++) {
			CORE_ALIGN(float v[4], 16);
			__m128 vResult = _mm_cmpgt_ps(mCols[i], other.mCols[i]);

			for (int j = 0; j < 4; j++) {
				v[i] = (float)(v[i] != 0);
			}

			result.mCols[i] = _mm_load_ps(v);
		}

		return result;
	}





	Matrix Matrix::operator+(const Matrix &rhs)const
	{
		Matrix result;

		for (int i = 0; i < 4; i++) {
			result.mCols[i] = _mm_add_ps(mCols[i], rhs.mCols[i]);
		}

		return result;
	}

	Matrix & Matrix::operator+=(const Matrix &rhs)
	{
		for (int i = 0; i < 4; i++) {
			mCols[i] = _mm_add_ps(mCols[i], rhs.mCols[i]);
		}

		return *this;
	}

	Matrix Matrix::operator-(const Matrix &rhs)const
	{
		Matrix result;

		for (int i = 0; i < 4; i++) {
			result.mCols[i] = _mm_sub_ps(mCols[i], rhs.mCols[i]);
		}

		return result;
	}

	Matrix & Matrix::operator-=(const Matrix &rhs)
	{
		for (int i = 0; i < 4; i++) {
			mCols[i] = _mm_sub_ps(mCols[i], rhs.mCols[i]);
		}

		return *this;
	}

	Matrix Matrix::operator*(const Matrix &rhs)const
	{
		Matrix result;

		for (int i = 0; i < 4; i++) {
			result.mCols[i] = _mm_mul_ps(CORE_MM_REPLICATE_X_PS(rhs.mCols[i]), mCols[0]);
			result.mCols[i] = CORE_MM_MADD_PS(CORE_MM_REPLICATE_Y_PS(rhs.mCols[i]), mCols[1], result.mCols[i]);
			result.mCols[i] = CORE_MM_MADD_PS(CORE_MM_REPLICATE_Z_PS(rhs.mCols[i]), mCols[2], result.mCols[i]);
			result.mCols[i] = CORE_MM_MADD_PS(CORE_MM_REPLICATE_W_PS(rhs.mCols[i]), mCols[3], result.mCols[i]);
		}

		return result;
	}

	Matrix & Matrix::operator*=(const Matrix &rhs)
	{
		*this = operator*(rhs);

		return *this;
	}

	Vector Matrix::operator*(const Vector &rhs)const
	{
		__m128 vResult;

		vResult = _mm_mul_ps(CORE_MM_REPLICATE_X_PS(rhs.mElems), mCols[0]);
		vResult = CORE_MM_MADD_PS(CORE_MM_REPLICATE_Y_PS(rhs.mElems), mCols[1], vResult);
		vResult = CORE_MM_MADD_PS(CORE_MM_REPLICATE_Z_PS(rhs.mElems), mCols[2], vResult);
		vResult = CORE_MM_MADD_PS(CORE_MM_REPLICATE_W_PS(rhs.mElems), mCols[3], vResult);

		return Vector(vResult);
	}

	Matrix Matrix::operator*(float scalar)const
	{
		Matrix result;

		__m128 scalarBrod = _mm_set_ps1(scalar);

		for (int i = 0; i < 4; i++) {
			result.mCols[i] = _mm_mul_ps(scalarBrod, mCols[i]);
		}

		return result;
	}

	Matrix & Matrix::operator*=(float scalar)
	{
		__m128 scalarBrod = _mm_set_ps1(scalar);

		for (int i = 0; i < 4; i++) {
			mCols[i] = _mm_mul_ps(scalarBrod, mCols[i]);
		}

		return *this;
	}





	void Matrix::Invert(Vector * pOutOptDet)
	{
		/** ftp://download.intel.com/design/PentiumIII/sml/24504301.pdf **/

		__m128 minor0, minor1, minor2, minor3;
		__m128 det;
		if (DeterminantVector(minor0, minor1, minor2, minor3, det)) {
			mCols[0] = minor0;
			mCols[1] = minor1;
			mCols[2] = minor2;
			mCols[3] = minor3;

			if (pOutOptDet)
				pOutOptDet->mElems = det;
		}
		else {
			if (pOutOptDet) {
				pOutOptDet->mElems = _mm_setzero_ps();
			}
		}
	}

	Matrix Matrix::Inverted(Vector *pOutOptDet)const
	{
		Matrix temp = *this;
		temp.Invert(pOutOptDet);
		return temp;
	}

	Vector Matrix::Determinant()const
	{
		__m128 m0, m1, m2, m3, det;
		if (DeterminantVector(m0, m1, m2, m3, det)) {
			return Vector(det);
		}

		return Vector::Zero();
	}





	void Matrix::Transpose()
	{
		_MM_TRANSPOSE4_PS(mCols[0], mCols[1], mCols[2], mCols[3]);
	}

	Matrix Matrix::Transposed()const
	{
		Matrix temp = *this;
		temp.Transpose();
		return temp;
	}




	void Matrix::ExtractAngles(float& pitch, float& yaw, float& roll)
	{
		float elems[4][4];
		AsFloats(elems);

		pitch = atan2f(elems[1][2], elems[2][2]);
		yaw = atan2f(-elems[0][2], sqrtf(elems[1][2] * elems[1][2] + elems[2][2] * elems[2][2]));
		roll = atan2f(elems[0][1], elems[0][0]);
	}





	bool Matrix::DeterminantVector(__m128 & minor0, __m128 & minor1, __m128 & minor2, __m128 & minor3, __m128 & pOutDet)const
	{
		/** ftp://download.intel.com/design/PentiumIII/sml/24504301.pdf **/

		__m128 row0, row1, row2, row3;
		__m128 det, tmp1;

		row0 = mCols[0];

		row1 = mCols[1];
		row1 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(1, 0, 3, 2));

		row2 = mCols[2];

		row3 = mCols[3];
		row3 = _mm_shuffle_ps(row3, row3, _MM_SHUFFLE(1, 0, 3, 2));

		tmp1 = _mm_mul_ps(row2, row3);
		tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
		minor0 = _mm_mul_ps(row1, tmp1);
		minor1 = _mm_mul_ps(row0, tmp1);
		tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
		minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
		minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
		minor1 = _mm_shuffle_ps(minor1, minor1, 0x4E);
		// -----------------------------------------------
		tmp1 = _mm_mul_ps(row1, row2);
		tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
		minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
		minor3 = _mm_mul_ps(row0, tmp1);
		tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
		minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
		minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
		minor3 = _mm_shuffle_ps(minor3, minor3, 0x4E);
		// -----------------------------------------------
		tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, 0x4E), row3);
		tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
		row2 = _mm_shuffle_ps(row2, row2, 0x4E);
		minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
		minor2 = _mm_mul_ps(row0, tmp1);
		tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
		minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
		minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
		minor2 = _mm_shuffle_ps(minor2, minor2, 0x4E);
		// -----------------------------------------------
		tmp1 = _mm_mul_ps(row0, row1);
		tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
		minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
		minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);
		tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
		minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
		minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));
		// -----------------------------------------------
		tmp1 = _mm_mul_ps(row0, row3);
		tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
		minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
		minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);
		tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
		minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
		minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));
		// -----------------------------------------------
		tmp1 = _mm_mul_ps(row0, row2);
		tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
		minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
		minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));
		tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
		minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
		minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);
		// -----------------------------------------------
		det = _mm_mul_ps(row0, minor0);
		det = _mm_add_ps(_mm_shuffle_ps(det, det, 0x4E), det);
		det = _mm_add_ss(_mm_shuffle_ps(det, det, 0xB1), det);

		// Check to see if det is 0.
		float fDet = _mm_cvtss_f32(det);
		if (fabsf(fDet) < FLOAT_EPSILON) {
			return false;
		}

		tmp1 = _mm_rcp_ss(det);
		det = _mm_sub_ss(_mm_add_ss(tmp1, tmp1), _mm_mul_ss(det, _mm_mul_ss(tmp1, tmp1)));
		det = _mm_shuffle_ps(det, det, 0x00);
		minor0 = _mm_mul_ps(det, minor0);
		minor1 = _mm_mul_ps(det, minor1);
		minor2 = _mm_mul_ps(det, minor2);
		minor3 = _mm_mul_ps(det, minor3);

		_MM_TRANSPOSE4_PS(minor0, minor1, minor2, minor3);

		pOutDet = det;
		return true;
	}





	Matrix Matrix::Identity()
	{
		Matrix m;

		CORE_ALIGN(float col0[4], 16) = { 1.0f, 0.0f, 0.0f, 0.0f };
		CORE_ALIGN(float col1[4], 16) = { 0.0f, 1.0f, 0.0f, 0.0f };
		CORE_ALIGN(float col2[4], 16) = { 0.0f, 0.0f, 1.0f, 0.0f };
		CORE_ALIGN(float col3[4], 16) = { 0.0f, 0.0f, 0.0f, 1.0f };

		m.mCols[0] = _mm_load_ps(col0);
		m.mCols[1] = _mm_load_ps(col1);
		m.mCols[2] = _mm_load_ps(col2);
		m.mCols[3] = _mm_load_ps(col3);

		return m;
	}

	Matrix Matrix::LookAtLH(const Vector &position, const Vector &target, const Vector &up)
	{
		Matrix mView;

		Vector zaxis = (target - position).V3Normalize();
		Vector xaxis = (up.V3Cross(zaxis)).V3Normalize();
		Vector yaxis = zaxis.V3Cross(xaxis);

		float m03 = -(xaxis.V4Dot(position));
		float m13 = -(yaxis.V4Dot(position));
		float m23 = -(zaxis.V4Dot(position));

		CORE_ALIGN(float fX[4], 16);
		CORE_ALIGN(float fY[4], 16);
		CORE_ALIGN(float fZ[4], 16);

		_mm_store_ps(fX, xaxis.mElems);
		_mm_store_ps(fY, yaxis.mElems);
		_mm_store_ps(fZ, zaxis.mElems);

		CORE_ALIGN(float col0[4], 16) = { fX[0], fY[0], fZ[0], 0.0f };
		CORE_ALIGN(float col1[4], 16) = { fX[1], fY[1], fZ[1], 0.0f };
		CORE_ALIGN(float col2[4], 16) = { fX[2], fY[2], fZ[2], 0.0f };
		CORE_ALIGN(float col3[4], 16) = { m03, m13, m23, 1.0f };

		mView.mCols[0] = _mm_load_ps(col0);
		mView.mCols[1] = _mm_load_ps(col1);
		mView.mCols[2] = _mm_load_ps(col2);
		mView.mCols[3] = _mm_load_ps(col3);

		return mView;
	}

	Matrix Matrix::PerspectiveFOVLH(float fieldOfView, float aspectRatio, float near, float far)
	{
		Matrix mProj;

		float rTanFOVDiv2 = 1.0f / tanf(fieldOfView / 2.0f);
		float fminusn = far - near;

		CORE_ALIGN(float col0[4], 16) = { (1.0f / aspectRatio) * rTanFOVDiv2, 0.0f, 0.0f, 0.0f };
		CORE_ALIGN(float col1[4], 16) = { 0.0f, rTanFOVDiv2, 0.0f, 0.0f };
		CORE_ALIGN(float col2[4], 16) = { 0.0f, 0.0f, far / fminusn, 1.0f };
		CORE_ALIGN(float col3[4], 16) = { 0.0f, 0.0f, -(far * near) / fminusn, 0.0f };

		mProj.mCols[0] = _mm_load_ps(col0);
		mProj.mCols[1] = _mm_load_ps(col1);
		mProj.mCols[2] = _mm_load_ps(col2);
		mProj.mCols[3] = _mm_load_ps(col3);

		return mProj;
	}

	Matrix Matrix::OrthoLH(float width, float height, float near, float far)
	{
		Matrix mOrtho;

		CORE_ALIGN(float col0[4], 16) = { 2.0f / width, 0.0f, 0.0f, 0.0f };
		CORE_ALIGN(float col1[4], 16) = { 0.0f, 2.0f / height, 0.0f, 0.0f };
		CORE_ALIGN(float col2[4], 16) = { 0.0f, 0.0f, 1.0f / (far - near), near / (near - far) };
		CORE_ALIGN(float col3[4], 16) = { 0.0f, 0.0f, 0.0f, 1.0f };

		mOrtho.mCols[0] = _mm_load_ps(col0);
		mOrtho.mCols[1] = _mm_load_ps(col1);
		mOrtho.mCols[2] = _mm_load_ps(col2);
		mOrtho.mCols[3] = _mm_load_ps(col3);

		return mOrtho;
	}

	Matrix Matrix::Translation(const Vector3f &t)
	{
		Matrix mTrans;

		CORE_ALIGN(float col0[4], 16) = { 1.0f, 0.0f, 0.0f, 0.0f };
		CORE_ALIGN(float col1[4], 16) = { 0.0f, 1.0f, 0.0f, 0.0f };
		CORE_ALIGN(float col2[4], 16) = { 0.0f, 0.0f, 1.0f, 0.0f };
		CORE_ALIGN(float col3[4], 16) = { t.x, t.y, t.z, 1.0f };

		mTrans.mCols[0] = _mm_load_ps(col0);
		mTrans.mCols[1] = _mm_load_ps(col1);
		mTrans.mCols[2] = _mm_load_ps(col2);
		mTrans.mCols[3] = _mm_load_ps(col3);

		return mTrans;
	}

	Matrix Matrix::Translation(const Vector &t)
	{
		return Matrix::Translation(t.AsVector3());
	}

	Matrix Matrix::Scaling(const Vector &scaling)
	{
		Matrix mScale;

		const Vector3f s = scaling.AsVector3();

		CORE_ALIGN(float col0[4], 16) = { s.x, 0.0f, 0.0f, 0.0f };
		CORE_ALIGN(float col1[4], 16) = { 0.0f, s.y, 0.0f, 0.0f };
		CORE_ALIGN(float col2[4], 16) = { 0.0f, 0.0f, s.z, 0.0f };
		CORE_ALIGN(float col3[4], 16) = { 0.0f, 0.0f, 0.0f, 1.0f };

		mScale.mCols[0] = _mm_load_ps(col0);
		mScale.mCols[1] = _mm_load_ps(col1);
		mScale.mCols[2] = _mm_load_ps(col2);
		mScale.mCols[3] = _mm_load_ps(col3);

		return mScale;
	}

	Matrix Matrix::RotationX(float angle)
	{
		Matrix mRot;

		const float s = sinf(angle);
		const float c = cosf(angle);

		CORE_ALIGN(float col0[4], 16) = { 1.0f, 0.0f, 0.0f, 0.0f };
		CORE_ALIGN(float col1[4], 16) = { 0.0f,    c,    s, 0.0f };
		CORE_ALIGN(float col2[4], 16) = { 0.0f,   -s,    c, 0.0f };
		CORE_ALIGN(float col3[4], 16) = { 0.0f, 0.0f, 0.0f, 1.0f };

		mRot.mCols[0] = _mm_load_ps(col0);
		mRot.mCols[1] = _mm_load_ps(col1);
		mRot.mCols[2] = _mm_load_ps(col2);
		mRot.mCols[3] = _mm_load_ps(col3);

		return mRot;
	}

	Matrix Matrix::RotationY(float angle)
	{
		Matrix mRot;

		const float s = sinf(angle);
		const float c = cosf(angle);

		CORE_ALIGN(float col0[4], 16) = { c,    0.0f,   -s, 0.0f };
		CORE_ALIGN(float col1[4], 16) = { 0.0f, 1.0f, 0.0f, 0.0f };
		CORE_ALIGN(float col2[4], 16) = { s,    0.0f,    c, 0.0f };
		CORE_ALIGN(float col3[4], 16) = { 0.0f, 0.0f, 0.0f, 1.0f };

		mRot.mCols[0] = _mm_load_ps(col0);
		mRot.mCols[1] = _mm_load_ps(col1);
		mRot.mCols[2] = _mm_load_ps(col2);
		mRot.mCols[3] = _mm_load_ps(col3);

		return mRot;
	}

	Matrix Matrix::RotationZ(float angle)
	{
		Matrix mRot;

		const float s = sinf(angle);
		const float c = cosf(angle);

		CORE_ALIGN(float col0[4], 16) = { c,    s,    0.0f, 0.0f };
		CORE_ALIGN(float col1[4], 16) = {-s,    c,    0.0f, 0.0f };
		CORE_ALIGN(float col2[4], 16) = { 0.0f, 0.0f, 1.0f, 0.0f };
		CORE_ALIGN(float col3[4], 16) = { 0.0f, 0.0f, 0.0f, 1.0f };

		mRot.mCols[0] = _mm_load_ps(col0);
		mRot.mCols[1] = _mm_load_ps(col1);
		mRot.mCols[2] = _mm_load_ps(col2);
		mRot.mCols[3] = _mm_load_ps(col3);

		return mRot;
	}

	Matrix Matrix::RotationQuaternion(const Quaternion &qRot)
	{
		Matrix m;

		const float xy2 = 2 * qRot.x * qRot.y;
		const float zw2 = 2 * qRot.z * qRot.w;
		const float xz2 = 2 * qRot.x * qRot.z;
		const float yw2 = 2 * qRot.y * qRot.w;
		const float yz2 = 2 * qRot.y * qRot.z;
		const float xw2 = 2 * qRot.x * qRot.w;

		const float xx2 = 2 * qRot.x * qRot.x;
		const float yy2 = 2 * qRot.y * qRot.y;
		const float zz2 = 2 * qRot.z * qRot.z;

		CORE_ALIGN(float col0[4], 16) = { 1 - yy2 - zz2, xy2 - zw2, xz2 + yw2, 0.0f };
		CORE_ALIGN(float col1[4], 16) = { xy2 + zw2, 1 - xx2 - zz2, yz2 - xw2, 0.0f };
		CORE_ALIGN(float col2[4], 16) = { xz2 - yw2, yz2 + xw2, 1 - xx2 - yy2, 0.0f };
		CORE_ALIGN(float col3[4], 16) = { 0.0f, 0.0f, 0.0f, 1.0f };

		m.mCols[0] = _mm_load_ps(col0);
		m.mCols[1] = _mm_load_ps(col1);
		m.mCols[2] = _mm_load_ps(col2);
		m.mCols[3] = _mm_load_ps(col3);

		// Not sure why I need to transpose this.
		return m.Transposed();
	}

	Matrix Matrix::RotationEuler(float x, float y, float z)
	{
		Matrix m;

		const float cX = cosf(x);
		const float cY = cosf(y);
		const float cZ = cosf(z);

		const float sX = sinf(x);
		const float sY = sinf(y);
		const float sZ = sinf(z);

		CORE_ALIGN(float col0[4], 16) = { cZ * cY, sZ * cY, -sY, 0.0f };
		CORE_ALIGN(float col1[4], 16) = { (cZ * sY * sX) - (sZ * cX), (sZ * sY * sX) + (cZ * cX), cY * sX, 0.0f };
		CORE_ALIGN(float col2[4], 16) = { (cZ * sY * cX) + (sZ * sX), (sZ * sY * cX) - (cZ * sX), cY * cX, 0.0f };
		CORE_ALIGN(float col3[4], 16) = { 0.0f, 0.0f, 0.0f, 1.0f };

		m.mCols[0] = _mm_load_ps(col0);
		m.mCols[1] = _mm_load_ps(col1);
		m.mCols[2] = _mm_load_ps(col2);
		m.mCols[3] = _mm_load_ps(col3);

		return m;
	}
}