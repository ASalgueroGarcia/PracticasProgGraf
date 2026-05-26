#pragma once
#include "common.h"
#include "libMath.h"
#include "Colliders.h"

using namespace libMath;
using namespace std;

class Camera
{
public:
    vec4float pos;
    vec4float rot;
    vec4float lookAt;
    vec4float lookAtPrime;
    vec4float forward, right, up;

    Sphere* coll = nullptr;

    Camera(vec4float pos, vec4float lookAt, vec4float up = { 0,1,0,0 });
    matrix4x4f computeViewMatrix();
    matrix4x4f computeProjectionMatrix(float zNear, float zFar, float fovy, float aspectRatio);

    void moveObject(double timeStep);
};