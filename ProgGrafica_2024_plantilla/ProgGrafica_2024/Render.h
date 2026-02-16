#pragma once
#include <iostream>

// ========== Datos Render ==========
typedef struct {
    int width;
    int height;
    char** buffer;
} Render;

// Declaración de funciones
void resetBuffer(Render& r);

// Método encargado de crear el render
Render make_render(int width, int height) {

    Render r;
    r.width = width;
    r.height = height;
    r.buffer = new char* [height];

    for (int i = 0; i < height; i++) {
        r.buffer[i] = new char[width];
    }
    resetBuffer(r);

    return r;
}

// Método encargado de reiniciar el buffer
void resetBuffer(Render& r) {

    for (int i = 0; i < r.height; i++) {
        for (int j = 0; j < r.width; j++) {
            r.buffer[i][j] = 0;
        }
    }
}

// Método encargado de activar el pixel en la matriz
void PutPixel(Render& r, int x, int y) {

    int cx = r.width / 2;
    int cy = r.height / 2;

    int col = x + cx;
    int row = cy - y;

    if (row < 0 || row >= r.height || col < 0 || col >= r.width)
        return;

    r.buffer[row][col] = 1;
}

// Método que muestra por terminal el estado actual del buffer
void Draw(const Render& r) {

    for (int i = 0; i < r.height; i++) {
        for (int j = 0; j < r.width; j++) {

            if (r.buffer[i][j] == 0)
                std::cout << " ";
            else
                std::cout << "*";
        }
        std::cout << "\n";
    }
}