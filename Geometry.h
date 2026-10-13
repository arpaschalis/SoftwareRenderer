#pragma once

#include <cmath>

struct Vector2f {
public:
	union {
		struct { float u, v; };
		struct { float x, y; };
		float raw[2];
	};

	Vector2f() : u(0), v(0) {}
	Vector2f(float u_, float v_) : u(u_), v(v_) {}
	inline Vector2f operator +(const Vector2f &V) const { return Vector2f(u + V.u, v + V.v); }
	inline Vector2f operator -(const Vector2f &V) const { return Vector2f(u - V.u, v - V.v); }
	inline Vector2f operator *(float f)			  const { return Vector2f(u * f, v * f); }
};

struct Vector3f {
public:
	union {
		struct { float x, y, z; };
		float raw[3];
	};

	Vector3f() : x(0), y(0), z(0) {}
	Vector3f(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
	inline Vector3f operator ^(const Vector3f &v) const { return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }	// cross product
	inline Vector3f operator +(const Vector3f &v) const { return Vector3f(x + v.x, y + v.y, z + v.z); }
	inline Vector3f operator -(const Vector3f &v) const { return Vector3f(x - v.x, y - v.y, z - v.z); }
	inline Vector3f operator *(float f)			  const { return Vector3f(x * f, y * f, z * f); }
	inline float	operator *(const Vector3f &v) const { return x * v.x + y * v.y + z * v.z; }	// dot product
	float norm() const { return std::sqrt(x * x + y * y + z * z); }
	Vector3f& normalize() { *this = (*this)*(1 / norm()); return *this; }
};


class Matrix {
public:
	float	m[4][4] = { 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, };

	Matrix() {}

	Matrix(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33) {
		m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
		m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
		m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
		m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
	}


	static Matrix Identity() {
		return Matrix(1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
	}

	// Transforms a point: (x, y, z) -> (x, y, z, 1), multiply, then divide by w.
	// The division by w is the perspective effect.
	Vector3f operator *(const Vector3f &v) const {
		float result[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		for (int y = 0; y < 4; y++) {
			for (int x = 0; x < 3; x++) {
				result[y] += m[y][x] * v.raw[x];
			}
			result[y] += m[y][3];
		}
		Vector3f vecResult;
		vecResult.x = result[0] / result[3];
		vecResult.y = result[1] / result[3];
		vecResult.z = result[2] / result[3];
		return vecResult;
	};

	Matrix operator *(const Matrix &matr) const {
		Matrix result;
		for (int y = 0; y < 4; y++) {
			for (int x = 0; x < 4; x++) {
				for (int step = 0; step < 4; step++) {
					result.m[y][x] += m[y][step] * matr.m[step][x];
				}
			}
		}
		return result;
	};

	// Helpers for invert(): element of the matrix with wrap-around indices.
	inline float e(int i, int j, int a, int b) {
		return m[((i + a) % 4)][((j + b) % 4)];
	}

	// One cofactor of the matrix (the 3x3 determinant that is left when
	// row i and column j are removed, with the right sign).
	float invf(int i, int j) {

		int o = 2 + (j - i);

		i += 4 + o;
		j += 4 - o;

		float inv =
			+ e(i, j, +1,-1) * e(i, j, +0,+0) * e(i, j, -1,+1)
			+ e(i, j, +1,+1) * e(i, j, +0,-1) * e(i, j, -1,+0)
			+ e(i, j, -1,-1) * e(i, j, +1,+0) * e(i, j, +0,+1)
			- e(i, j, -1,-1) * e(i, j, +0,+0) * e(i, j, +1,+1)
			- e(i, j, -1,+1) * e(i, j, +0,-1) * e(i, j, +1,+0)
			- e(i, j, +1,-1) * e(i, j, -1,+0) * e(i, j, +0,+1);

		return (o % 2) ? inv : -inv;
	}

	// Inverse = adjugate (matrix of cofactors) divided by the determinant.
	Matrix invert() {
		Matrix inverted;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				inverted.m[i][j] = invf(i, j);

		float D = 0;
		for (int k = 0; k < 4; k++)
			D += m[0][k] * inverted.m[k][0];

		if (D == 0)
			return Matrix::Identity();

		D = 1.0f / D;

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				inverted.m[i][j] = inverted.m[i][j] * D;

		return inverted;
	}

	Matrix transpose() {
		Matrix transposed;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				transposed.m[j][i] = m[i][j];

		return transposed;
	}
};
