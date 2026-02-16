#include <iostream>
#include "MathLib.h"
#include "Render.h"

void DrawBorder(Render& render);

using namespace std;
int main(int argc, char** argv)
{
	Render render = make_render(11, 11);
	Vector4f punto = make_vector4f(3, 3, 0, 1);
	Vector4f puntoAux = make_vector4f(3, 3, 0, 1);
	Matriz4x4f translation = make_translate(-6, 0, 0);
	Matriz4x4f Rotate = make_rotate(0, 0, 3.14159265f / 4.0f);	// Rotación con vector
	// Rotación con cuaternión
	Vector4f quaternion = make_quaternion(0, 0, 1, 3.14159265f / 4.0f);
	Matriz4x4f RotateQ = make_rotate(quaternion);	

	// Punto inicial
	cout << "Punto inicial\n";

	resetBuffer(render);
	DrawBorder(render);
	PutPixel(render, (int)punto.x, (int)punto.y);
	Draw(render);

	// Trasladar 6 unidades a la izquierda
	cout << "\nMatriz trasladada\n";
	punto = translation * punto;
	puntoAux = translation * puntoAux;
	

	resetBuffer(render);
	DrawBorder(render);
	PutPixel(render, (int)punto.x, (int)punto.y);
	Draw(render);

	// Rotar 45 grados en el eje Z
	cout << "\nRotacion (vector)\n";
	puntoAux = Rotate * puntoAux;
	resetBuffer(render);
	DrawBorder(render);
	PutPixel(render, (int)puntoAux.x, (int)puntoAux.y);
	Draw(render);

	cout << "\nRotacion (cuaterion)\n";
	punto = RotateQ * punto;

	resetBuffer(render);
	DrawBorder(render);
	PutPixel(render, (int)punto.x, (int)punto.y);
	Draw(render);

	return 0;
}

// Método encargado de mostrar los bordes de la matriz 11 x 11
void DrawBorder(Render& render) {
	int halfW = render.width / 2;
	int halfH = render.height / 2;

	// Bordes horizontales
	for (int x = -halfW; x <= halfW; x++) {
		PutPixel(render, x, halfH);
		PutPixel(render, x, -halfH);
	}

	// Bordes verticales
	for (int y = -halfH; y <= halfH; y++) {
		PutPixel(render, halfW, y);
		PutPixel(render, -halfW, y);
	}
}