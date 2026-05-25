#pragma once

#include "InputManager.h"
#include "Object3D.h"
#include "Camera.h"
#include "Light.h"
#include "common.h"

class Render
{
public:
	Render(float anchura, float altura);

	typedef struct {
		unsigned int bufferId;
		unsigned int vertexBufferId;
		unsigned int indexBufferId;
		unsigned int shaderProgram;
	} bufferObject_t;

	Camera* cam;
	Light* light;

	GLFWwindow* window;
	std::vector<Object3D*> objectList;
	std::map<int, bufferObject_t> bufferList;

	void initGL();
	void putObject(Object3D* obj);
	void removeObject(Object3D* obj);
	void putCamera(Camera* cam);
	void putLight(Light* light);
	Light* getLight();
	void DrawGL();
	void mainLoop();
};