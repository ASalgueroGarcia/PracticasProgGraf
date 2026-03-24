#pragma once
#include <math.h>

// ========== Vector de 4 floats ==========
typedef struct {
	union {
		struct {
			float x;
			float y;
			float z;
			float w;
		};
		struct {
			float r;
			float g;
			float b;
			float a;
		};
		float v[4];
	};
}Vector4f;

// ========== Metodos Vector4f ==========

// M?todo que crea un vector
inline Vector4f make_vector4f(float x, float y, float z, float w) {
	Vector4f vres;
	vres.x = x, vres.y = y, vres.z = z,	vres.w = w;

	return vres;
}

// M?todo que normaliza un vector
inline Vector4f normalize(Vector4f v) {
	float modulo = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

	if (modulo > 0.0f) {
		v.x /= modulo, v.y /= modulo, v.z /= modulo;
	}

	return v;
}

// M?todos de operaciones de vectores
inline Vector4f operator +(Vector4f v1, Vector4f v2) {
	Vector4f vres = { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w };
	return vres;
}

inline Vector4f operator -(Vector4f v1, Vector4f v2) {
	Vector4f vres = { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w };
	return vres;
}

inline float dot_product(Vector4f v1, Vector4f v2) {
	return v1.x * v2.x + v1.y* v2.y + v1.z * v2.z + v1.w* v2.w;
}

inline Vector4f cross_product(Vector4f v1, Vector4f v2) {
	return make_vector4f(v1.y*v2.z - v1.z*v2.y, -(v1.x*v2.z - v1.z*v2.x), v1.x*v2.y - v1.y*v2.x, 0.0f);
}

inline Vector4f operator *(Vector4f v, float s) {
	return make_vector4f(v.x * s, v.y * s, v.z * s, v.w * s);
}

inline Vector4f operator *(float s, Vector4f v) {
	return make_vector4f(v.x * s, v.y * s, v.z * s, v.w * s);
}

// ========== Matriz de 4 vectores ==========
typedef struct {
	union {
		float max[16];
		Vector4f matV[4];
	};
}Matriz4x4f;

// ========== M?todos matriz 4x4 ==========

// Operador * necesario para la rotaci?n
inline Matriz4x4f operator *(Matriz4x4f m1, Matriz4x4f m2) {
	Matriz4x4f mres{};

	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			for (int i = 0; i < 4; i++) {
				mres.matV[row].v[col] += m1.matV[row].v[i] * m2.matV[i].v[col];
			}
		}
	}

	return mres;
}

// M?todo que crea una matriz de tipo identidad
inline Matriz4x4f make_identity() {
	Matriz4x4f matrix{};
	matrix.matV[0].x = 1, matrix.matV[1].y = 1, matrix.matV[2].z = 1, matrix.matV[3].w = 1;

	return matrix;
}

// M?todo que crea una matriz de traslaci?n de N unidades en los ejes x, y, z
inline Matriz4x4f make_translate(float x, float y, float z) {
	Matriz4x4f matrix = make_identity();

	matrix.matV[0].w = x, matrix.matV[1].w = y, matrix.matV[2].w = z;

	return matrix;
}

// M?todo que crea una matriz de rotaci?n de N radianes en los ejes x, y, z
inline Matriz4x4f make_rotate(float angleX, float angleY, float angleZ) {
	Matriz4x4f rotationX = make_identity();
	Matriz4x4f rotationY = make_identity();
	Matriz4x4f rotationZ = make_identity();
	Matriz4x4f matrix{};

	float cosX = cos(angleX), sinX = sin(angleX);
	float cosY = cos(angleY), sinY = sin(angleY);
	float cosZ = cos(angleZ), sinZ = sin(angleZ);

	// Rotaci?n en X
	rotationX.matV[1].y = cosX;
	rotationX.matV[1].z = -sinX;
	rotationX.matV[2].y = sinX;
	rotationX.matV[2].z = cosX;

	// Rotaci?n en Y
	rotationY.matV[0].x = cosY;
	rotationY.matV[0].z = sinY;
	rotationY.matV[2].x = -sinY;
	rotationY.matV[2].z = cosY;

	// Rotaci?n en Z
	rotationZ.matV[0].x = cosZ;
	rotationZ.matV[0].y = -sinZ;
	rotationZ.matV[1].x = sinZ;
	rotationZ.matV[1].y = cosZ;

	matrix = rotationZ * rotationY * rotationX;

	return matrix;
}

// M?todo que crea una matriz de escalado de N unidades en los ejes x, y, z
inline Matriz4x4f make_scale(float x, float y, float z) {
	Matriz4x4f matrix = make_identity();
	
	matrix.matV[0].x = x, matrix.matV[1].y = y, matrix.matV[2].z = z;

	return matrix;
}

// Multiplicaci?n vector por matriz
inline Vector4f operator *(Matriz4x4f matrix, Vector4f vector) {
	Vector4f res{};
	for (int row = 0; row < 4; row++) {
		res.v[row] =
			matrix.matV[row].v[0] * vector.v[0] +
			matrix.matV[row].v[1] * vector.v[1] +
			matrix.matV[row].v[2] * vector.v[2] +
			matrix.matV[row].v[3] * vector.v[3];
	}

	return res;
}

// ========== Cuaterniones ==========

// M?todo encargado de crear cuaterni?n de rotaci?n a partir de eje y ?ngulo
inline Vector4f make_quaternion(float x, float y, float z, float angle) {

	// Normalizar eje
	float length = sqrt(x * x + y * y + z * z);
	if (length == 0.0f) {
		return make_vector4f(0, 0, 0, 1);
	}

	x /= length;
	y /= length;
	z /= length;

	float half = angle * 0.5f;
	float s = sin(half);
	float c = cos(half);

	return make_vector4f(x * s, y * s, z * s, c);
}

// M?todo encargado de crear una matriz de rotaci?n a partir de un cuaterni?n
inline Matriz4x4f make_rotate(Vector4f vector) {

	Matriz4x4f matrix = make_identity();

	float x = vector.x;
	float y = vector.y;
	float z = vector.z;
	float w = vector.w;

	matrix.matV[0].x = 1 - 2 * y * y - 2 * z * z;
	matrix.matV[0].y = 2 * x * y - 2 * z * w;
	matrix.matV[0].z = 2 * x * z + 2 * y * w;

	matrix.matV[1].x = 2 * x * y + 2 * z * w;
	matrix.matV[1].y = 1 - 2 * x * x - 2 * z * z;
	matrix.matV[1].z = 2 * y * z - 2 * x * w;

	matrix.matV[2].x = 2 * x * z - 2 * y * w;
	matrix.matV[2].y = 2 * y * z + 2 * x * w;
	matrix.matV[2].z = 1 - 2 * x * x - 2 * y * y;

	return matrix;
}