#pragma once
#define SQ(x) ((x) * (x))
#define _USE_MATH_DEFINES
#define M_PI       3.14159265358979323846

#include <cmath>
#include <algorithm>
#include <iostream>
#include <string>

//Linear Interpolate (lerp)
template<typename T>
static T lerp(const T a, const T b, float t)
{
	return a * (1.0f - t) + (b * t);
}

template<typename T>
static T clamp(const T value, const T minValue, const T maxValue)
{
	return max(min(value, maxValue), minValue);
}

class Vec3
{
public:
	union
	{
		float v[3];
		struct { float x, y, z; };
	};

	Vec3() : 
		x(0.f),
		y(0.f),
		z(0.f) 
	{}

	Vec3(float _x, float _y, float _z) : 
		x(_x),
		y(_y),
		z(_z) 
	{
	}

	//plus
	Vec3 operator+(const Vec3& pVec) const
	{
		return Vec3(v[0] + pVec.v[0], v[1] + pVec.v[1], v[2] + pVec.v[2]);
	}

	Vec3& operator+=(const Vec3& pVec)
	{
		v[0] += pVec.v[0];
		v[1] += pVec.v[1];
		v[2] += pVec.v[2];
		return *this;
	}
	
	//negative
	Vec3 operator-() const
	{
		return Vec3(-v[0], -v[1], -v[2]);
	}

	//Subtraction
	Vec3 operator-(const Vec3& pVec) const
	{
		return Vec3(v[0] - pVec.v[0], v[1] - pVec.v[1], v[2] - pVec.v[2]);
	}

	Vec3& operator-=(const Vec3& pVec)
	{
		v[0] -= pVec.v[0];
		v[1] -= pVec.v[1];
		v[2] -= pVec.v[2];
		return *this;
	}

	//multiplication
	//multiply each element
	Vec3 operator*(const Vec3& pVec) const
	{
		return Vec3(v[0] * pVec.v[0], v[1] * pVec.v[1], v[2] * pVec.v[2]);
	}

	Vec3& operator*=(const Vec3& pVec)
	{
		v[0] *= pVec.v[0];
		v[1] *= pVec.v[1];
		v[2] *= pVec.v[2];
		return *this;
	}

	//multiply a value
	Vec3 operator*(const float val) const
	{
		return Vec3(v[0] * val, v[1] * val, v[2] * val);
	}

	Vec3& operator*=(const float val)
	{
		v[0] *= val;
		v[1] *= val;
		v[2] *= val;
		return *this;
	}

	//division
	//divide each element
	Vec3 operator/(const Vec3& pVec) const
	{
		return Vec3(v[0] / pVec.v[0], v[1] / pVec.v[1], v[2] / pVec.v[2]);
	}

	Vec3& operator/=(const Vec3& pVec)
	{
		v[0] /= pVec.v[0];
		v[1] /= pVec.v[1];
		v[2] /= pVec.v[2];
		return *this;
	}

	//divide a value
	Vec3 operator/(const float val) const
	{
		return Vec3(v[0] / val, v[1] / val, v[2] / val);
	}

	Vec3& operator/=(const float val)
	{
		v[0] /= val;
		v[1] /= val;
		v[2] /= val;
		return *this;
	}

	//length of vector
	float length() const
	{
		return sqrtf(SQ(v[0]) + SQ(v[1]) + SQ(v[2]));
	}

	//length square
	float length_Square() const
	{
		return SQ(v[0]) + SQ(v[1]) + SQ(v[2]);
	}

	//return normalize the vector
	Vec3 Normalize(void) const
	{
		float len = 1.0f / length();
		return Vec3(x * len, y * len, z * len);
	}

	//normalize the vector,return length
	float Normalize_GetLength()
	{
		float length = sqrtf(SQ(v[0]) + SQ(v[1]) + SQ(v[2]));
		float len = 1.0f / length;
		v[0] *= len; v[1] *= len; v[2] *= len;
		return length;
	}

	//dot product
	float Dot(const Vec3& pVec) const
	{
		return v[0] * pVec.v[0] + v[1] * pVec.v[1] + v[2] * pVec.v[2];
	}

	//cross product
	Vec3 Cross(const Vec3& pVec)
	{
		return Vec3(pVec.v[1] * v[2] - pVec.v[2] * v[1],
			pVec.v[2] * v[0] - pVec.v[0] * v[2],
			pVec.v[0] * v[1] - pVec.v[1] * v[0]);
	}

	Vec3 rotateX(float angleRadians) const {
		float cosA = cosf(angleRadians);
		float sinA = sinf(angleRadians);
		return Vec3(
			x,
			y * cosA - z * sinA,
			y * sinA + z * cosA
		);
	}

	// 3. 绕Y轴旋转（俯仰角，向上看为正，向下看为负）
	Vec3 rotateY(float angleRadians) const {
		float cosA = cosf(angleRadians);
		float sinA = sinf(angleRadians);
		return Vec3(
			x * cosA + z * sinA,
			y,
			-x * sinA + z * cosA
		);
	}

	// 4. 绕Z轴旋转（偏航角，向右转为正，向左转为负）
	Vec3 rotateZ(float angleRadians) const {
		float cosA = cosf(angleRadians);
		float sinA = sinf(angleRadians);
		return Vec3(
			x * cosA - y * sinA,
			x * sinA + y * cosA,
			z
		);
	}

	float Max() const
	{
		return max(x, max(y, z));
	}

	float Min() const
	{
		return min(x, min(y, z));
	}

	std::string get_string() const
	{
		return "{" + std::to_string(v[0]) + ", " + std::to_string(v[1]) + ", " + std::to_string(v[2]) + "}";
	}

	float operator[] (unsigned int index)
	{
		return v[index];
	}
};

//4 dimention vector
class Vec4
{
public:
	union
	{
		float v[4];
		struct { float x, y, z, w; };
	};

	Vec4(float _a = 0, float _b = 0, float _c = 0, float _w = 1): 
		x(_a),
		y(_b),
		z(_c),
		w(_w)
	{}

	//plus
	Vec4 operator+(const Vec4& pVec) const
	{
		return Vec4(v[0] + pVec.v[0], v[1] + pVec.v[1], v[2] + pVec.v[2], v[3] + pVec.v[3]);
	}

	Vec4& operator+=(const Vec4& pVec)
	{
		v[0] += pVec.v[0];
		v[1] += pVec.v[1];
		v[2] += pVec.v[2];
		v[3] += pVec.v[3];
		return *this;
	}

	//negative
	Vec4 operator-() const
	{
		return Vec4(-v[0], -v[1], -v[2], -v[3]);
	}

	//subtraction
	Vec4 operator-(const Vec4& pVec) const
	{
		return Vec4(v[0] - pVec.v[0], v[1] - pVec.v[1], v[2] - pVec.v[2], v[3] - pVec.v[3]);
	}

	Vec4 operator-(const Vec3& pVec) const
	{
		return Vec4(v[0] - pVec.v[0], v[1] - pVec.v[1], v[2] - pVec.v[2], v[3]);
	}

	Vec4& operator-=(const Vec4& pVec)
	{
		v[0] -= pVec.v[0];
		v[1] -= pVec.v[1];
		v[2] -= pVec.v[2];
		v[3] -= pVec.v[3];
		return *this;
	}

	Vec4& operator-=(const Vec3& pVec)
	{
		v[0] -= pVec.v[0];
		v[1] -= pVec.v[1];
		v[2] -= pVec.v[2];
		return *this;
	}

	//multiplication
	//multiply a value
	Vec4 operator*(const float val) const
	{
		return Vec4(v[0] * val, v[1] * val, v[2] * val, v[3] * val);
	}

	Vec4& operator*=(const float val)
	{
		v[0] *= val;
		v[1] *= val;
		v[2] *= val;
		v[3] *= val;
		return *this;
	}

	//multiply each element
	Vec4 operator*(const Vec4& pVec) const
	{
		return Vec4(v[0] * pVec.v[0], v[1] * pVec.v[1], v[2] * pVec.v[2], v[3] * pVec.v[3]);
	}

	Vec4& operator*=(const Vec4& pVec)
	{
		v[0] *= pVec.v[0];
		v[1] *= pVec.v[1];
		v[2] *= pVec.v[2];
		v[3] *= pVec.v[3];
		return *this;
	}

	// division
	//divide a value
	Vec4 operator/(const float val) const
	{
		return Vec4(v[0] / val, v[1] / val, v[2] / val, v[3] / val);
	}

	Vec4& operator/=(const float val)
	{
		v[0] /= val;
		v[1] /= val;
		v[2] /= val;
		v[3] /= val;
		return *this;
	}

	//divide each element
	Vec4 operator/(const Vec4& pVec) const
	{
		return Vec4(v[0] / pVec.v[0], v[1] / pVec.v[1], v[2] / pVec.v[2], w);
	}

	Vec4& operator/=(const Vec4& pVec)
	{
		v[0] /= pVec.v[0];
		v[1] /= pVec.v[1];
		v[2] /= pVec.v[2];
		return *this;
	}

	//length of vector
	float length() const
	{
		return sqrt(SQ(x) + SQ(y) + SQ(z) + SQ(w));
	}

	//length square
	float length_Square() const
	{
		return SQ(v[0]) + SQ(v[1]) + SQ(v[2]);
	}

	//cross product
	Vec4 Cross(const Vec4& v1)
	{
		return Vec4(v1.v[1] * v[2] - v1.v[2] * v[1],
			v1.v[2] * v[0] - v1.v[0] * v[2],
			v1.v[0] * v[1] - v1.v[1] * v[0]);
	}

	//dot product
	float Dot(const Vec4& pVec) const
	{
		return v[0] * pVec.v[0] + v[1] * pVec.v[1] + v[2] * pVec.v[2];
	}

	//divide by w, return a new vector
	Vec4 Perspctive_Divide()
	{
		if (w == 0)
		{
			std::cout << "Can't divide, w is 0." << std::endl;
			return *this;
		}

		return { v[0] / w, v[1] / w, v[2] / w, w };
	}

	float Max() const
	{
		return max(x, max(y, z));
	}

	float Min() const
	{
		return min(x, min(y, z));
	}

	std::string operator<< (const Vec4& pVec)
	{
		return "{" + std::to_string(v[0]) + ", " + std::to_string(v[1]) + ", " + std::to_string(v[2]) + ", " + std::to_string(v[3]) + "}";
	}
};

static float Dot(const Vec3& v1, const Vec3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

static float Dot(const Vec4& v1, const Vec4& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

static Vec3 Cross(const Vec3& v1, const Vec3& v2)
{
	return Vec3(v1.v[1] * v2.v[2] - v1.v[2] * v2.v[1],
		v1.v[2] * v2.v[0] - v1.v[0] * v2.v[2],
		v1.v[0] * v2.v[1] - v1.v[1] * v2.v[0]);
}

static Vec4 Cross(const Vec4& v1, const Vec4& v2)
{
	return Vec4(v1.v[1] * v2.v[2] - v1.v[2] * v2.v[1],
		v1.v[2] * v2.v[0] - v1.v[0] * v2.v[2],
		v1.v[0] * v2.v[1] - v1.v[1] * v2.v[0],
		0.f);
}

static Vec3 Max(const Vec3& v1, const Vec3& v2)
{
	return Vec3(max(v1.v[0], v2.v[0]),
		max(v1.v[1], v2.v[1]),
		max(v1.v[2], v2.v[2]));
}

static Vec3 Min(const Vec3& v1, const Vec3& v2)
{
	return Vec3(min(v1.v[0], v2.v[0]),
		min(v1.v[1], v2.v[1]),
		min(v1.v[2], v2.v[2]));
}

//spherical coordinate
class Spherical
{
public:
	union
	{
		float s[3];
		struct { float r, theta, phi; };
	};

	Spherical(float _r = 0, float _theta = 0, float _phi = 0) : 
		r(_r),
		theta(_theta),
		phi(_phi)
	{}

	Spherical Cartesian_To_Spherical(float x, float y, float z)
	{
		r = sqrtf(SQ(x) + SQ(y) + SQ(z));
		theta = 0.f;
		phi = 0.f;

		if (r > 0)
		{
			theta = std::acosf(z / r);

			phi = std::atan2(y, x);
			if (phi < 0)
				phi += 2 * M_PI;
		}
		return *this;
	}

	void Cartesian_To_Spherical(Vec3& pVec)
	{
		r = sqrtf(SQ(pVec.x) + SQ(pVec.y) + SQ(pVec.z ));
		theta = 0.f;
		phi = 0.f;

		if (r > 0)
		{
			theta = std::acosf(pVec.z / r);

			phi = std::atan2(pVec.y, pVec.x);
			if (phi < 0)
				phi += 2 * M_PI;
		}
	}

	//spherical coordinate to cartesian coordinate
	Vec3 spherical_To_Cartesian(float _r, float _theta, float _phi) {
		return { _r * std::sinf(_theta) * std::cosf(_phi), _r * std::sinf(_theta) * std::sinf(_phi), _r * std::cosf(_theta) };
	}

	//r = 1
	Vec3 spherical_To_Cartesian(float _theta, float _phi) {
		return { std::sinf(_theta) * std::cosf(_phi), std::sinf(_theta) * std::sinf(_phi), std::cosf(_theta) };
	}

	Vec3 spherical_To_Cartesian() {
		return { r * std::sinf(theta) * std::cosf(phi), r * std::sinf(theta) * std::sinf(phi), r * std::cosf(theta) };
	}

	static float get_X(float theta, float phi) {
		return std::sin(theta) * std::cos(phi);
	}

	static float get_Y(float theta, float phi) {
		return std::sin(theta) * std::sin(phi);
	}

	static float get_Z(float theta, float phi) {
		return std::cos(theta);
	}
};

class alignas(64) Matrix
{
public:
	union
	{
		float a[4][4];
		float m[16];
	};

	Matrix()
	{
		m[0] = 1.f;
		m[1] = 0.f;
		m[2] = 0.f;
		m[3] = 0.f;
		m[4] = 0.f;
		m[5] = 1.f;
		m[6] = 0.f;
		m[7] = 0.f;
		m[8] = 0.f;
		m[9] = 0.f;
		m[10] = 1.f;
		m[11] = 0.f;
		m[12] = 0.f;
		m[13] = 0.f;
		m[14] = 0.f;
		m[15] = 1.f;
	}

	Matrix(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
	{
		a[0][0] = m00;
		a[0][1] = m01;
		a[0][2] = m02;
		a[0][3] = m03;
		a[1][0] = m10;
		a[1][1] = m11;
		a[1][2] = m12;
		a[1][3] = m13;
		a[2][0] = m20;
		a[2][1] = m21;
		a[2][2] = m22;
		a[2][3] = m23;
		a[3][0] = m30;
		a[3][1] = m31;
		a[3][2] = m32;
		a[3][3] = m33;
	}

	Matrix Normalize()
	{
		m[0] = 1.f;
		m[1] = 0.f;
		m[2] = 0.f;
		m[3] = 0.f;
		m[4] = 0.f;
		m[5] = 1.f;
		m[6] = 0.f;
		m[7] = 0.f;
		m[8] = 0.f;
		m[9] = 0.f;
		m[10] = 1.f;
		m[11] = 0.f;
		m[12] = 0.f;
		m[13] = 0.f;
		m[14] = 0.f;
		m[15] = 1.f;
		return *this;
	}

	Vec4 mul(const Vec4& v)
	{
		return Vec4(
			(v.x * m[0] + v.y * m[1] + v.z * m[2] + v.w * m[3]),
			(v.x * m[4] + v.y * m[5] + v.z * m[6] + v.w * m[7]),
			(v.x * m[8] + v.y * m[9] + v.z * m[10] + v.w * m[11]),
			(v.x * m[12] + v.y * m[13] + v.z * m[14] + v.w * m[15]));
	}

	Vec3 mulPoint(const Vec3& v) const
	{
		return Vec3(
			(v.x * m[0] + v.y * m[1] + v.z * m[2]) + m[3],
			(v.x * m[4] + v.y * m[5] + v.z * m[6]) + m[7],
			(v.x * m[8] + v.y * m[9] + v.z * m[10]) + m[11]);
	}

	Vec3 mulVec  (const Vec3& v) const
	{
		return Vec3(
			(v.x * m[0] + v.y * m[1] + v.z * m[2]),
			(v.x * m[4] + v.y * m[5] + v.z * m[6]),
			(v.x * m[8] + v.y * m[9] + v.z * m[10]));
	}

	Vec4 Translate(float x, float y, float z, Vec4& pVec)
	{
		Normalize();
		m[3] = x;
		m[7] = y;
		m[11] = z;
		return mul(pVec);
	}

	Matrix Translate(Vec4 pVec)
	{
		Matrix a;
		a[3] = pVec.x;
		a[7] = pVec.y;
		a[11] = pVec.z;
		return a;
	}

	static Matrix Translate(const Vec3& v)
	{
		Matrix mat;
		mat.a[0][3] = v.x;
		mat.a[1][3] = v.y;
		mat.a[2][3] = v.z;
		return mat;
	}

	Matrix Rotate_X(float angle)
	{
		Normalize();
		float cos = cosf(angle);
		m[5] = cos;
		m[6] = -sqrtf(1 - cos * cos);
		m[9] = sqrtf(1 - cos * cos);
		m[10] = cos;
		return *this;
	}

	Matrix Rotate_Y(float angle)
	{
		Normalize();
		float cos = cosf(angle);
		m[0] = cos;
		m[8] = -sqrtf(1 - cos * cos);
		m[2] = sqrtf(1 - cos * cos);
		m[10] = cos;
		return *this;
	}

	Matrix Rotate_Z(float angle)
	{
		Normalize();
		float cos = cosf(angle);
		m[0] = cos;
		m[1] = -sqrtf(1 - cos * cos);
		m[4] = sqrtf(1 - cos * cos);
		m[5] = cos;
		return *this;
	}

	static Matrix rotateX(float theta)
	{
		Matrix mat;
		float ct = cosf(theta);
		float st = sinf(theta);
		mat.m[5] = ct;
		mat.m[6] = st;
		mat.m[9] = -st;
		mat.m[10] = ct;
		return mat;
	}
	static Matrix rotateY(float theta)
	{
		Matrix mat;
		float ct = cosf(theta);
		float st = sinf(theta);
		mat.m[0] = ct;
		mat.m[2] = -st;
		mat.m[8] = st;
		mat.m[10] = ct;
		return mat;
	}
	static Matrix rotateZ(float theta)
	{
		Matrix mat;
		float ct = cosf(theta);
		float st = sinf(theta);
		mat.m[0] = ct;
		mat.m[1] = st;
		mat.m[4] = -st;
		mat.m[5] = ct;
		return mat;
	}

	Matrix Scaling(float Sx, float Sy, float Sz)
	{
		Normalize();
		m[0] = Sx;
		m[5] = Sy;
		m[10] = Sz;
		return *this;
	}

	static Matrix Scaling(const Vec3& v)
	{
		Matrix mat;
		mat.m[0] = v.x;
		mat.m[5] = v.y;
		mat.m[10] = v.z;
		return mat;
	}
	static Matrix Scaling(const float v)
	{
		Matrix mat;
		mat.m[0] = v;
		mat.m[5] = v;
		mat.m[10] = v;
		return mat;
	}

	Matrix mul(const Matrix& matrix) const
	{
		Matrix ret;
		ret.m[0] = m[0] * matrix.m[0] + m[1] * matrix.m[4] + m[2] * matrix.m[8] + m[3] * matrix.m[12];
		ret.m[1] = m[0] * matrix.m[1] + m[1] * matrix.m[5] + m[2] * matrix.m[9] + m[3] * matrix.m[13];
		ret.m[2] = m[0] * matrix.m[2] + m[1] * matrix.m[6] + m[2] * matrix.m[10] + m[3] * matrix.m[14];
		ret.m[3] = m[0] * matrix.m[3] + m[1] * matrix.m[7] + m[2] * matrix.m[11] + m[3] * matrix.m[15];
		ret.m[4] = m[4] * matrix.m[0] + m[5] * matrix.m[4] + m[6] * matrix.m[8] + m[7] * matrix.m[12];
		ret.m[5] = m[4] * matrix.m[1] + m[5] * matrix.m[5] + m[6] * matrix.m[9] + m[7] * matrix.m[13];
		ret.m[6] = m[4] * matrix.m[2] + m[5] * matrix.m[6] + m[6] * matrix.m[10] + m[7] * matrix.m[14];
		ret.m[7] = m[4] * matrix.m[3] + m[5] * matrix.m[7] + m[6] * matrix.m[11] + m[7] * matrix.m[15];
		ret.m[8] = m[8] * matrix.m[0] + m[9] * matrix.m[4] + m[10] * matrix.m[8] + m[11] * matrix.m[12];
		ret.m[9] = m[8] * matrix.m[1] + m[9] * matrix.m[5] + m[10] * matrix.m[9] + m[11] * matrix.m[13];
		ret.m[10] = m[8] * matrix.m[2] + m[9] * matrix.m[6] + m[10] * matrix.m[10] + m[11] * matrix.m[14];
		ret.m[11] = m[8] * matrix.m[3] + m[9] * matrix.m[7] + m[10] * matrix.m[11] + m[11] * matrix.m[15];
		ret.m[12] = m[12] * matrix.m[0] + m[13] * matrix.m[4] + m[14] * matrix.m[8] + m[15] * matrix.m[12];
		ret.m[13] = m[12] * matrix.m[1] + m[13] * matrix.m[5] + m[14] * matrix.m[9] + m[15] * matrix.m[13];
		ret.m[14] = m[12] * matrix.m[2] + m[13] * matrix.m[6] + m[14] * matrix.m[10] + m[15] * matrix.m[14];
		ret.m[15] = m[12] * matrix.m[3] + m[13] * matrix.m[7] + m[14] * matrix.m[11] + m[15] * matrix.m[15];
		return ret;
	}

	Matrix operator*(const Matrix& matrix)
	{
		return mul(matrix);
	}

	Matrix Transpose(const Matrix& matrix)
	{
		Matrix mat;
		mat.m[0] = matrix.m[0];
		mat.m[1] = matrix.m[4];
		mat.m[2] = matrix.m[8];
		mat.m[3] = matrix.m[12];
		mat.m[4] = matrix.m[1];
		mat.m[5] = matrix.m[5];
		mat.m[6] = matrix.m[9];
		mat.m[7] = matrix.m[13];
		mat.m[8] = matrix.m[2];
		mat.m[9] = matrix.m[6];
		mat.m[10] = matrix.m[10];
		mat.m[11] = matrix.m[14];
		mat.m[12] = matrix.m[3];
		mat.m[13] = matrix.m[7];
		mat.m[14] = matrix.m[11];
		mat.m[15] = matrix.m[15];
		return mat;
	}

	Matrix Transpose()
	{
		return Matrix(a[0][0], a[1][0], a[2][0], a[3][0],
			a[0][1], a[1][1], a[2][1], a[3][1],
			a[0][2], a[1][2], a[2][2], a[3][2],
			a[0][3], a[1][3], a[2][3], a[3][3]);
	}

	float& operator[](unsigned int index)
	{
		return m[index];
	}

	Matrix invert()
	{
		Matrix inv;
		inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
		inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
		inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
		inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
		inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
		inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
		inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
		inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
		inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
		inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
		inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
		inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
		inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
		inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
		inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
		inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];
		float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
		if (det == 0) {
			// Handle this case 
		}
		det = 1.0 / det;
		for (int i = 0; i < 16; i++) {
			inv[i] = inv[i] * det;
		}
		return inv;
	}

	static Matrix perspective(const float fov, float aspect, const float n, const float f) // FOV in degrees, outputs transposed Matrix for DX
	{
		Matrix pers;
		memset(pers.m, 0, sizeof(float) * 16);
		float t = 1.0f / (tanf(fov * 0.5f * M_PI / 180.0f));
		pers.a[0][0] = t / aspect;
		pers.a[1][1] = t;
		pers.a[2][2] = f / (f - n);
		pers.a[2][3] = -(f * n) / (f - n);
		pers.a[3][2] = 1.0f;
		return pers;
	}

	static Matrix perspective_PI(float fov, float aspect, float _near, float _far) {
		Matrix proM;
		memset(proM.m, 0, 16 * sizeof(float));
		proM.a[0][0] = 1 / (aspect * (tan(fov / 2)));//no pi
		proM.a[1][1] = 1 / (tan(fov / 2));
		proM.a[2][2] = _far / (_far - _near);
		proM.a[2][3] = -(_far * _near) / (_far - _near);
		proM.a[3][2] = 1;
		return proM;
	}

	static Matrix lookAtMatrix(Vec3 from, Vec3 to, Vec3 up) {
		Matrix lookat;
		memset(lookat.m, 0, 16 * sizeof(float));
		Vec3 dir = (to - from) / (to - from).length();
		Vec3 right = up.Cross(dir);//only cross x,y,z. 
		Vec3 realUp = dir.Cross(right);//real up vector

		lookat.a[0][0] = right.x;  lookat.a[0][1] = right.y;  lookat.a[0][2] = right.z;  lookat.a[0][3] = -(from.Dot(right));
		lookat.a[1][0] = realUp.x; lookat.a[1][1] = realUp.y; lookat.a[1][2] = realUp.z; lookat.a[1][3] = -(from.Dot(realUp));
		lookat.a[2][0] = dir.x;    lookat.a[2][1] = dir.y;    lookat.a[2][2] = dir.z;    lookat.a[2][3] = -(from.Dot(dir));
		lookat.a[3][3] = 1;
		return lookat;
	}

	static Matrix lookAt(const Vec3& from, const Vec3& to, const Vec3& up)
	{
		Matrix mat;
		Vec3 dir = (to - from).Normalize();
		Vec3 left = Cross(up, dir).Normalize();
		Vec3 newUp = Cross(dir, left);
		mat.a[0][0] = left.x;
		mat.a[0][1] = left.y;
		mat.a[0][2] = left.z;
		mat.a[1][0] = newUp.x;
		mat.a[1][1] = newUp.y;
		mat.a[1][2] = newUp.z;
		mat.a[2][0] = dir.x;
		mat.a[2][1] = dir.y;
		mat.a[2][2] = dir.z;
		mat.a[0][3] = -Dot(from, left);
		mat.a[1][3] = -Dot(from, newUp);
		mat.a[2][3] = -Dot(from, dir);
		mat.a[3][3] = 1;
		return mat;
	}

	static Matrix rotateAxis(const Vec3& axis, float angle)
	{
		Vec3 u = axis.Normalize();
		float c = cosf(angle);
		float s = sinf(angle);
		float t = 1.0f - c;

		return Matrix(
			t * u.x * u.x + c, t * u.x * u.y + s * u.z, t * u.x * u.z - s * u.y, 0,
			t * u.x * u.y - s * u.z, t * u.y * u.y + c, t * u.y * u.z + s * u.x, 0,
			t * u.x * u.z + s * u.y, t * u.y * u.z - s * u.x, t * u.z * u.z + c, 0,
			0, 0, 0, 1
		);
	}
};

class Quaternion
{
public:
	union {
		struct { float a, b, c, d; };
		float q[4];
	};

	Quaternion() : a(0.f), b(0.f), c(0.f), d(0.f) {}

	Quaternion(Vec4& pVec) : a(pVec.x), b(pVec.y), c(pVec.z), d(pVec.w) {}

	Quaternion(float _a, float _b, float _c, float _d) : a(_a), b(_b), c(_c), d(_d) {}

	float Mangnitude() const
	{
		return sqrtf(SQ(a) + SQ(b) + SQ(c) + SQ(d));
	}

	float Mangnitude(Quaternion q) const
	{
		return sqrtf(SQ(q.a) + SQ(q.b) + SQ(q.c) + SQ(q.d));
	}

	Quaternion Normalize()
	{
		float mag = Mangnitude();
		return { a / mag,  b / mag , c / mag , d / mag };
	}

	Quaternion operator*(const float val) const
	{
		return Quaternion(a * val, b * val, c * val, d * val);
	}

	Quaternion operator*(Quaternion q1)
	{
		Quaternion v;
		v.a = ((d * q1.a) + (a * q1.d) + (b * q1.c) - (c * q1.b));
		v.b = ((d * q1.b) - (a * q1.c) + (b * q1.d) + (c * q1.a));
		v.c = ((d * q1.c) + (a * q1.b) - (b * q1.a) + (c * q1.d));
		v.d = ((d * q1.d) - (a * q1.a) - (b * q1.b) - (c * q1.c));
		return v;
	}

	Quaternion& operator*=(const float val)
	{
		a *= val;
		b *= val;
		c *= val;
		d *= val;
		return *this;
	}

	Quaternion Multuply(Quaternion q1, Quaternion q2)
	{
		return {
			q1.d * q2.a + q1.a * q2.d + q1.b * q2.c - q1.c * q2.b,
			q1.d * q2.b - q1.a * q2.c + q1.b * q2.d + q1.c * q2.a,
			q1.d * q2.c + q1.a * q2.b - q1.b * q2.a + q1.c * q2.d,
			q1.d * q2.d - q1.a * q2.a - q1.b * q2.b - q1.c * q2.c
		};
	}

	float dot(Quaternion q1, Quaternion q2)
	{
		return q1.d * q2.d + q1.a * q2.a + q1.b * q2.b + q1.c * q2.c;
	}

	float theta(Quaternion q1, Quaternion q2)
	{
		float d = dot(q1, q2);
		return d < 0 ? acosf(-d) : acosf(d);
	}

	Quaternion Conjugate()
	{
		a = -a;
		b = -b;
		c = -c;
		return Quaternion(-a, -b, -c, d);
	}

	void invert()
	{
		Conjugate();
		Normalize();
	}

	Quaternion Unary_negate()
	{
		return Quaternion(-a, -b, -c, -d);
	}

	//Spherical Linear Interpolate
	Quaternion Slerp(Quaternion a, Quaternion b, float t)
	{
		Quaternion A = a.Normalize();
		Quaternion B = b.Normalize();
		float ta = theta(A, B);
		float q_A = sinf(ta * (1 - t)) / sinf(ta);
		float q_B = sinf(ta * t) / sinf(ta);
		return {
			q_A * a.a + q_B * b.a,
			q_A * a.b + q_B * b.b,
			q_A * a.c + q_B * b.c,
			q_A * a.d + q_B * b.d,
		};
	}

	static Quaternion slerp(Quaternion q1, Quaternion q2, float t)
	{
		Quaternion qr;
		float dp = q1.a * q2.a + q1.b * q2.b + q1.c * q2.c + q1.d * q2.d;
		Quaternion q11 = dp < 0 ? -q1 : q1;
		dp = dp > 0 ? dp : -dp;
		float theta = acosf(clamp(dp, -1.0f, 1.0f));
		if (theta == 0)
		{
			return q1;
		}
		float d = sinf(theta);
		float a = sinf((1 - t) * theta);
		float b = sinf(t * theta);
		float coeff1 = a / d;
		float coeff2 = b / d;
		qr.a = coeff1 * q11.a + coeff2 * q2.a;
		qr.b = coeff1 * q11.b + coeff2 * q2.b;
		qr.c = coeff1 * q11.c + coeff2 * q2.c;
		qr.d = coeff1 * q11.d + coeff2 * q2.d;
		qr.Normalize();
		return qr;
	}

	Quaternion Slerp_with_length(Quaternion a, Quaternion b, float t)
	{
		float a_len = a.Mangnitude();
		float b_len = b.Mangnitude();

		Quaternion dir_lerp = Slerp(a, b, t);

		float len_lerp = a_len + (b_len - a_len) * t;

		return dir_lerp * len_lerp;
	}

	Matrix toMatrix() const
	{
		float aa = a * a, ab = a * b, ac = a * c;
		float bb = b * b, bc = b * c, cc = c * c;
		float da = d * a, db = d * b, dc = d * c;
		Matrix m;
		m[0] = 1 - 2 * (bb + cc); m[1] = 2 * (ab - dc); m[2] = 2 * (ac + db); m[3] = 0;
		m[4] = 2 * (ab + dc); m[5] = 1 - 2 * (aa + cc); m[6] = 2 * (bc - da); m[7] = 0;
		m[8] = 2 * (ac - db); m[9] = 2 * (bc + da); m[10] = 1 - 2 * (aa + bb); m[11] = 0;
		m[12] = m[13] = m[14] = 0; m[15] = 1;
		return m;
	}

	void rotateAboutAxis(Vec3 pt, float angle, Vec3 axis)
	{
		Quaternion q1, p, qinv;
		q1.a = sinf(0.5f * angle) * axis.x;
		q1.b = sinf(0.5f * angle) * axis.y;
		q1.c = sinf(0.5f * angle) * axis.z;
		q1.d = cosf(0.5f * angle);
		p.a = pt.x;
		p.b = pt.y;
		p.c = pt.z;
		p.d = 0;
		qinv = q1;
		qinv.invert();
		q1 = q1 * p;
		q1 = q1 * qinv;
		a = q1.a;
		b = q1.b;
		c = q1.c;
		d = q1.d;
	}

	Quaternion operator-()
	{
		return Quaternion(-a, -b, -c, -d);
	}
	//Quaternuion Inverse()
	//{
	//	return Conjugate() / SQ(Mangnitude());
	//}
};

class Colour
{
public:
	union {
		struct {
			float r;
			float g;
			float b;
			float a;
		};
		float c[4];
	};
	Colour() : r(0), g(0), b(0), a(0) {}

	Colour(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}

	Colour(float _r, float _g, float _b) : r(_r), g(_g), b(_b), a(0) {}

	Colour(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a) : r(_r / 255.f), g(_g / 255.f), b(_b / 255.f), a(_a / 255.f) {}

	Colour(unsigned char _r, unsigned char _g, unsigned char _b) : r(_r / 255.f), g(_g / 255.f), b(_b / 255.f), a(0) {}
	
	Colour operator+(const Colour& colour) const
	{
		return Colour(r + colour.r, g + colour.g, b + colour.b, a + colour.a);
	}

	Colour operator*(const Colour& colour) const
	{
		return Colour(r * colour.r, g * colour.g, b * colour.b, a * colour.a);
	}
	Colour operator*(const float _a) const
	{
		return Colour(r * _a, g * _a, b * _a, a * _a);
	}
	Colour operator/(const float _a) const
	{
		return Colour(r / _a, g / _a, b / _a, a / _a);
	}
};

class Colour_rgb
{
public:
	union {
		float c[3];
		struct { float r, g, b; };
	};
	Colour_rgb() : r(0), g(0), b(0) {}

	Colour_rgb(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {}

	Colour_rgb(unsigned char _r, unsigned char _g, unsigned char _b) : r(_r / 255.f), g(_g / 255.f), b(_b / 255.f) {}

	Colour_rgb operator+(const Colour_rgb& colour) const
	{
		return Colour_rgb(r + colour.r, g + colour.g, b + colour.b);
	}

	Colour_rgb operator*(const Colour_rgb& colour) const
	{
		return Colour_rgb(r * colour.r, g * colour.g, b * colour.b);
	}
	Colour_rgb operator*(const float _a) const
	{
		return Colour_rgb(r * _a, g * _a, b * _a);
	}
	Colour_rgb operator/(const float _a) const
	{
		return Colour_rgb(r / _a, g / _a, b / _a);
	}
};

class ShadingFrame
{
public:
	union {
		float sf[16];
		struct {
			Vec3 tangent;   // 切线向量 (X轴)
			Vec3 bitangent; // 副切线向量 (Y轴) 
			Vec3 normal;    // 法线向量 (Z轴)
		};
	};
	void buildFromNormal(const Vec3& n) {
		normal = n.Normalize();

		// 选择与法线不平行的基础向量
		Vec3 t;
		if (std::abs(normal.x) > std::abs(normal.y)) {
			t = Vec3(normal.z, 0, -normal.x);
		}
		else {
			t = Vec3(0, -normal.z, normal.y);
		}

		// Gram-Schmidt 正交化
		tangent = (t - normal * t.Dot(normal)).Normalize();
		bitangent = normal.Cross(tangent);
	}

	void buildFromTangent(const Vec3& t, const Vec3& n) {
		normal = n.Normalize();

		// Gram-Schmidt: 确保切线与法线正交
		tangent = (t - normal * t.Dot(normal)).Normalize();
		bitangent = normal.Cross(tangent);
	}

public:
	// 构造函数
	ShadingFrame(const Vec3& n) { buildFromNormal(n); }
	ShadingFrame(const Vec3& t, const Vec3& n) { buildFromTangent(t, n); }

	// 正向变换：世界空间 → 切空间
	Vec3 worldToTangent(const Vec3& worldVec) const {
		return Vec3(
			worldVec.Dot(tangent),
			worldVec.Dot(bitangent),
			worldVec.Dot(normal)
		);
	}

	// 逆向变换：切空间 → 世界空间  
	Vec3 tangentToWorld(const Vec3& tangentVec) const {
		return tangent * tangentVec.x +
			bitangent * tangentVec.y +
			normal * tangentVec.z;
	}

	// 获取基向量
	const Vec3& getTangent() const { return tangent; }
	const Vec3& getBitangent() const { return bitangent; }
	const Vec3& getNormal() const { return normal; }

	// 构建变换矩阵
	//Matrix getWorldToTangentMatrix() const {
	//	return Transpose(Matrix(tangent, bitangent, normal));
	//}

	//Matrix getTangentToWorldMatrix() const {
	//	return Matrix(tangent, bitangent, normal);
	//}

};

class Frame
{
public:
	Vec3 u;
	Vec3 v;
	Vec3 w;
	void fromVector(const Vec3& n)
	{
		// Gram-Schmit
		w = n.Normalize();
		if (fabsf(w.x) > fabsf(w.y))
		{
			float l = 1.0f / sqrtf(w.x * w.x + w.z * w.z);
			u = Vec3(w.z * l, 0.0f, -w.x * l);
		}
		else
		{
			float l = 1.0f / sqrtf(w.y * w.y + w.z * w.z);
			u = Vec3(0, w.z * l, -w.y * l);
		}
		v = Cross(w, u);
	}
	void fromVectorTangent(const Vec3& n, const Vec3& t)
	{
		w = n.Normalize();
		u = t.Normalize();
		v = Cross(w, u);
	}
	Vec3 toLocal(const Vec3& vec) const
	{
		return Vec3(Dot(vec, u), Dot(vec, v), Dot(vec, w));
	}
	Vec3 toWorld(const Vec3& vec) const
	{
		return ((u * vec.x) + (v * vec.y) + (w * vec.z));
	}
};