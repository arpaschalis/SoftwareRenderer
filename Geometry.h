#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <iostream>

template<typename T> struct Vector2 {
public:
	union {
		struct { T u, v; };
		struct { T x, y; };
		T raw[2];
	};

	Vector2() : u(0), v(0) {}
	Vector2(T u_, T v_) : u(u_), v(v_) {}
	inline Vector2<T> operator +(const Vector2<T> &V) const { return Vector2<T>(u + V.u, v + V.v); }
	inline Vector2<T> operator -(const Vector2<T> &V) const { return Vector2<T>(u - V.u, v - V.v); }
	inline Vector2<T> operator *(float f)			  const { return Vector2<T>(u * f, v * f); }
	template <class> friend std::ostream& operator <<(std::ostream& s, Vector2<T>& v);
};

template<typename T> struct Vector3 {
public:
	union {
		struct { T x, y, z; };
		struct { T ivert, iuv, inorm; };
		T raw[3];
	};

	Vector3() : x(0), y(0), z(0) {}
	Vector3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {}
	inline Vector3<T> operator ^(const Vector3<T> &v) const { return Vector3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
	inline Vector3<T> operator +(const Vector3<T> &v) const { return Vector3<T>(x + v.x, y + v.y, z + v.z); }
	inline Vector3<T> operator -(const Vector3<T> &v) const { return Vector3<T>(x - v.x, y - v.y, z - v.z); }
	inline Vector3<T> operator *(float f)			  const { return Vector3<T>(x * f, y * f, z * f); }
	inline T		  operator *(const Vector3<T> &v) const { return x * v.x + y * v.y + z * v.z; }
	float norm() const { return std::sqrt(x * x + y * y + z * z); }
	Vector3<T>& normalize(T l = 1) { *this = (*this)*(1 / norm()); return *this; }
	template <class> friend std::ostream& operator <<(std::ostream& s, Vector3<T>& v);
};

typedef Vector2<float>	Vector2f;
typedef Vector2<int>	Vector2i;
typedef Vector3<float>	Vector3f;
typedef Vector3<int>	Vector3i;

template <class T> std::ostream& operator <<(std::ostream& s, Vector2<T>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class T> std::ostream& operator <<(std::ostream& s, Vector3<T>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}


class Matrix {
public:
	float	m[4][4] = { 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, };

	Matrix() {
		for (int x = 0; x < 4; x++) {
			for (int y = 0; y < 4; y++) {
				m[y][x] = 0.0f;
			}
		}
	}

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

	Vector3<float> operator *(const Vector3<float> &v) const {
		float result[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		for (int y = 0; y < 4; y++) {
			for (int x = 0; x < 3; x++) {
				result[y] += m[y][x] * v.raw[x];
			}
			result[y] += m[y][3];
		}
		Vector3<float> vecResult;
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

};


#endif // __GEOMETRY_H__