#include "Render.h"
#include "Object3D.h"
#include "EventManager.h"
#include <cstring>
#include <algorithm>

Render::Render(float anchura, float altura)
{
	this->window = glfwCreateWindow(anchura, altura, "Triangulos Rotando", nullptr, nullptr);
	this->light = nullptr;
	initGL();
}

void Render::initGL()
{
	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glEnable(GL_DEPTH_TEST);

	int fbw = 0, fbh = 0;
	glfwGetFramebufferSize(window, &fbw, &fbh);
	if (fbw > 0 && fbh > 0) {
		glViewport(0, 0, fbw, fbh);
	}

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	EventManager::initEventManager(this->window);
}

void Render::putObject(Object3D* obj)
{
	bufferObject_t bo = { 0, 0, 0 };
	glGenVertexArrays(1, &bo.bufferId);
	glGenBuffers(1, &bo.vertexBufferId);
	glGenBuffers(1, &bo.indexBufferId);

	glBindVertexArray(bo.bufferId);

	glBindBuffer(GL_ARRAY_BUFFER, bo.vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vertex_t) * obj->vertexList.size(),
		obj->vertexList.data(),
		GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo.indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int) * obj->idList.size(),
		obj->idList.data(),
		GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex_t),
		(void*)offsetof(vertex_t, posicion)
	);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex_t),
		(void*)offsetof(vertex_t, color)
	);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex_t),
		(void*)offsetof(vertex_t, normal)
	);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(
		3,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex_t),
		(void*)offsetof(vertex_t, texCoord)
	);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	bufferList[obj->objId] = bo;
	objectList.push_back(*obj);
}

void Render::removeObject(Object3D* obj)
{
	auto object = bufferList.find(obj->objId);

	if (object != bufferList.end())
	{
		bufferObject_t& bo = object->second;

		glDeleteBuffers(1, &bo.vertexBufferId);
		glDeleteBuffers(1, &bo.indexBufferId);
		glDeleteVertexArrays(1, &bo.bufferId);

		bufferList.erase(object);
	}

	objectList.erase(
		std::remove_if(objectList.begin(), objectList.end(),
			[&](const Object3D& o)
			{
				return o.objId == obj->objId;
			}),
		objectList.end()
	);
}

void Render::putCamera(Camera* cam)
{
	this->cam = cam;
}

void Render::putLight(Light* light)
{
	this->light = light;
}

Light* Render::getLight()
{
	return this->light;
}

void Render::DrawGL()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto& obj : objectList)
	{
		auto& bo = bufferList[obj.objId];

		obj.prg->use();

		Matriz4x4f view = cam->getMatrixLookAt();
		Matriz4x4f proj = cam->getMatrixPerspective();
		Matriz4x4f MVP = proj * view * obj.modelMatrix;

		obj.prg->setUniformData(obj.uniformMVPName, MVP);

		if (light != nullptr)
		{
			obj.prg->setUniformLight(light);
		}

		if (obj.mat != nullptr)
		{
			obj.prg->setUniformMaterial(obj.mat);

			if (obj.mat->texture != nullptr && obj.mat->texture->textureId != 0)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, obj.mat->texture->textureId);
				obj.prg->setUniformInt("textureData", 0);
			}
		}

		glBindVertexArray(bo.bufferId);
		glBindBuffer(GL_ARRAY_BUFFER, bo.vertexBufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo.indexBufferId);

		obj.prg->setAttributeData(
			obj.attrPosName,
			4,
			GL_FLOAT,
			GL_FALSE,
			sizeof(vertex_t),
			(void*)offsetof(vertex_t, posicion)
		);

		obj.prg->setAttributeData(
			obj.attrColorName,
			4,
			GL_FLOAT,
			GL_FALSE,
			sizeof(vertex_t),
			(void*)offsetof(vertex_t, color)
		);

		if (!obj.attrNormalName.empty())
		{
			obj.prg->setAttributeData(
				obj.attrNormalName,
				3,
				GL_FLOAT,
				GL_FALSE,
				sizeof(vertex_t),
				(void*)offsetof(vertex_t, normal)
			);
		}

		if (!obj.attrTexCoordName.empty())
		{
			obj.prg->setAttributeData(
				obj.attrTexCoordName,
				2,
				GL_FLOAT,
				GL_FALSE,
				sizeof(vertex_t),
				(void*)offsetof(vertex_t, texCoord)
			);
		}

		glDrawElements(
			GL_TRIANGLES,
			static_cast<GLsizei>(obj.idList.size()),
			GL_UNSIGNED_INT,
			0
		);

		glBindVertexArray(0);
	}
}

void Render::mainLoop()
{
	float lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		EventManager::updateEvents();

		if (cam)
			cam->move(deltaTime);

		if (light)
			light->move(deltaTime);

		for (auto& obj : objectList)
			obj.move(deltaTime);

		DrawGL();

		glfwSwapBuffers(window);
	}
}