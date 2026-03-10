#include "Camera.h"

Camera::Camera(Vector4f pos, Vector4f rot, Vector4f lookAt,
	Vector4f up, float fovy, float aspectRatio,
	float zNear, float zFar)
{
	this->pos = pos;
	this->rot = rot;
	this->lookAt = lookAt;
	this->up = up;
	this->fovy = fovy;
	this->aspectRatio = aspectRatio;
	this->zNear = zNear;
	this->zFar = zFar;
}

Matriz4x4f Camera::getMatrixLookAt()
{
	Vector4f forward = normalize(lookAt - pos);
	Vector4f right = normalize(cross_product(forward, up));
	Vector4f newUp = cross_product(right, forward);

	Matriz4x4f view = make_identity();

	view.matV[0] = make_vector4f(right.x, right.y, right.z, -dot_product(right, pos));
	view.matV[1] = make_vector4f(newUp.x, newUp.y, newUp.z, -dot_product(newUp, pos));
	view.matV[2] = make_vector4f(-forward.x, -forward.y, -forward.z, dot_product(forward, pos));

	return view;
}

Matriz4x4f Camera::getMatrixPerspective()
{
	Matriz4x4f proj{};

	// OJO: tan() usa radianes
	float fovyRad = fovy * (3.14159265f / 180.0f);
	float f = 1.0f / tan(fovyRad * 0.5f);

	proj.matV[0].x = f / aspectRatio;
	proj.matV[1].y = f;

	proj.matV[2].z = -(zFar + zNear) / (zFar - zNear);
	proj.matV[2].w = -(2 * zFar * zNear) / (zFar - zNear);

	proj.matV[3].z = -1.0f;
	proj.matV[3].w = 0.0f;

	return proj;
}

void Camera::move(double timeStep)
{
    static bool firstMouse = true;
    static double lastX = 0.0;
    static double lastY = 0.0;

    double xpos, ypos;
    glfwGetCursorPos(EventManager::window, &xpos, &ypos);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double dx = xpos - lastX;
    double dy = ypos - lastY;

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.002f;

    // ROTACION CORREGIDA
    rot.y -= (float)dx * sensitivity;
    rot.x -= (float)dy * sensitivity;

    float limit = 1.5f;
    if (rot.x > limit) rot.x = limit;
    if (rot.x < -limit) rot.x = -limit;

    Vector4f forward;

    forward.x = cos(rot.x) * sin(rot.y);
    forward.y = sin(rot.x);
    forward.z = cos(rot.x) * cos(rot.y);
    forward.w = 0;

    forward = normalize(forward);

    lookAt = pos + forward;

    float speed = 2.0f * (float)timeStep;

    if (EventManager::keyMap[GLFW_KEY_UP])
        pos = pos + forward * speed;

    if (EventManager::keyMap[GLFW_KEY_DOWN])
        pos = pos - forward * speed;

    Vector4f right = normalize(cross_product(forward, up));

    if (EventManager::keyMap[GLFW_KEY_RIGHT])
        pos = pos + right * speed;

    if (EventManager::keyMap[GLFW_KEY_LEFT])
        pos = pos - right * speed;

    lookAt = pos + forward;
}