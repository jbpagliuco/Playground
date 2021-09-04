#pragma once

#include "Vector.h"

namespace playground
{
	class Quaternion;

	CORE_ALIGN_MS(16) class Matrix
	{
	public:
		Matrix();

		// Creates a 4x4 matrix with the given data.
		// pData[0..3] = Column 1.
		// pData[4..7] = Column 2.
		// pData[8..11] = Column 3.
		// pData[12..15] = Column 4.
		// WARNING: pData MUST BE ALIGNED TO A 16-BYTE BOUNDARY
		explicit Matrix(const float pData[16]);

		/* Creates a 4x4 matrix with the 4 row vectors.
		| col1.x col2.x col3.x col4.x |
		| col1.y col2.y col3.y col4.y |
		| col1.z col2.z col3.z col4.z |
		| col1.w col2.w col3.w col4.w | */
		Matrix(const Vector &col1, const Vector &col2, const Vector &col3, const Vector &col4);


		// Creates a 4x4 identity matrix.
		static Matrix Identity();

		// Creates a left handed look at matrix.
		static Matrix LookAtLH(const Vector &position, const Vector &target, const Vector &up);
		// Creates a left handed perspective field of view matrix.
		static Matrix PerspectiveFOVLH(float fieldOfView, float aspectRatio, float near, float far);
		// Creates a left handed perspective field of view matrix.
		static Matrix OrthoLH(float width, float height, float near, float far);

		// Creates a translation matrix.
		static Matrix Translation(const Vector3f &translation);
		static Matrix Translation(const Vector &translation);
		// Creates a scaling matrix.
		static Matrix Scaling(const Vector &scaling);
		// Creates a rotation about the x-axis matrix.
		static Matrix RotationX(float angle);
		// Creates a rotation about the y-axis matrix.
		static Matrix RotationY(float angle);
		// Creates a rotation about the z-axis matrix.
		static Matrix RotationZ(float angle);
		// Creates a rotation matrix based off the quaternion.
		static Matrix RotationQuaternion(const Quaternion &qRot);
		// Creates a rotation matrix based off the euler angles.
		static Matrix RotationEuler(float x, float y, float z);


	public:
		void AsFloats(float mat[4][4])const;
		void AsFloats(float mat[16])const;

		// Returns the elements at (x, y).
		float operator()(int x, int y)const;

		Vector GetColumn(int col)const;

		// Checks for equality (within epsilon) in each entry of the matrix. Returns 1.0f in entry slot if equal, 0.0f otherwise.
		Matrix Equals(const Matrix &other, float epsilon = FLOAT_EPSILON)const;
		// Checks for inequality (within epsilon) in each entry of the matrix. Returns 1.0f in entry slot if not equal, 0.0f otherwise.
		Matrix NotEquals(const Matrix &other, float epsilon = FLOAT_EPSILON)const;

		// Compares the matrices. Returns a 1.0f for a lesser value or a 0.0f otherwise in the respective component.
		Matrix Less(const Matrix &other)const;
		// Compares the matrices. Returns a 1.0f for a lesser or equal value or a 0.0f otherwise in the respective component.
		Matrix LessEq(const Matrix &other)const;
		// Compares the matrices. Returns a 1.0f for a lesser value or a 0.0f otherwise in the respective component.
		Matrix Greater(const Matrix &other)const;
		// Compares the matrices. Returns a 1.0f for a lesser value or a 0.0f otherwise in the respective component.
		Matrix GreaterEq(const Matrix &other)const;

		// Adds the two matrices and returns the result.
		Matrix operator+(const Matrix & rhs)const;
		// Adds the two matrices and sets the result equal to this.
		Matrix &operator+=(const Matrix & rhs);

		// Subtracts the two matrices and returns the result.
		Matrix operator-(const Matrix & rhs)const;
		// Subtracts the two matrices and sets the result equal to this.
		Matrix &operator-=(const Matrix & rhs);

		// Performs 4x4 matrix multiplication on the two matrices and returns the result.
		Matrix operator*(const Matrix &rhs)const;
		// Performs 4x4 matrix multiplication on the two matrices and sets the result equal to this.
		Matrix &operator*=(const Matrix &rhs);

		// Performs 4x4 * 4x1 matrix multplication and returns the result.
		Vector operator*(const Vector &rhs)const;

		// Multiplies each element in the matrix by the scalar value and returns the result.
		Matrix operator*(float scalar)const;
		// Multiplies each element in the matrix by the scalar value and sets the result equal to this.
		Matrix &operator*=(float scalar);

		// Attempts to invert the matrix. pOutOptDet is an optional pointer to a Vector to hold the determinant of the matrix.
		// Note: If the determinant is equal to zero, the inverse does not exist, and thus the matrix will not change.
		void Invert(Vector *pOutOptDet = nullptr);
		Matrix Inverted(Vector *pOutOptDet = nullptr)const;

		// Caculates the determinant and broadcasts the value into each component.
		Vector Determinant()const;

		// Transposes this matrix.
		void Transpose();
		Matrix Transposed()const;

		void ExtractAngles(float& pitch, float& yaw, float& roll);

	private:
		// Returns true if det != 0.
		bool DeterminantVector(__m128 &minor0, __m128 &minor1, __m128 &minor2, __m128 &minor3, __m128 &pOutDet)const;

	public:
		__m128 mCols[4];
	} CORE_ALIGN_GCC(16);
}