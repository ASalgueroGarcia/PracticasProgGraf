#pragma once
#include "MathLib.h"
#include "EventManager.h"

class Camera
{
public:

	Vector4f pos;
	Vector4f rot;

	Vector4f lookAt;
	Vector4f up;

	float fovy;
	float aspectRatio;
	float zNear;
	float zFar;

	Camera(Vector4f pos, Vector4f rot, Vector4f lookAt,
		Vector4f up, float fovy, float aspectRatio,
		float zNear, float zFar);

	Matriz4x4f getMatrixLookAt();

	Matriz4x4f getMatrixPerspective();

	void move(double timeStep);
};