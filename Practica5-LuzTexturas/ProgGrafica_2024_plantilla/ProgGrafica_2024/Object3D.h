#pragma once

#include "MathLib.h"
#include "Vertex.h"
#include "common.h"
#include "EventManager.h"
#include "Program.h"
#include "Material.h"
#include <string>
#include <vector>

using namespace std;

class Object3D
{
public:
	unsigned int objId;
	Vector4f posicion;
	Vector4f rotacion;
	Vector4f escala;
	Matriz4x4f modelMatrix;
	Program* prg;
	std::string attrPosName;
	std::string attrColorName;
	std::string attrNormalName;
	std::string attrTexCoordName;
	std::string uniformMVPName;

	Material* mat;

	vector<vertex_t> vertexList;
	vector<int> idList;

	Object3D();
	~Object3D();
	void createTriangle();
	void loadFromFile(const char* fileName);
	virtual void move(double timeStep);
	void updateModelMatrix();
};