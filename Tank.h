#pragma once
#include <windows.h>
#include <string>
#include "Grafo.h"

class Tank {
public:
    int x, y;  // Posición del tanque en el grafo
    std::wstring tankText;  // Texto que representa el tanque (e.g., "TCE" para tanque celeste)
    HWND hButton;  // El botón que representa el tanque
    int damage;  // Daño recibido por el tanque

    // Constructor
    Tank(const std::wstring& text, Grafo& grafo, HWND parent, HINSTANCE hInst) : tankText(text), damage(0) {
        placeRandomly(grafo);
        createButton(parent, hInst);
    }

    // Colocar el tanque en una celda vacía del grafo
    void placeRandomly(Grafo& grafo) {
        bool placed = false;
        while (!placed) {
            int randX = rand() % grafo.N;
            int randY = rand() % grafo.N;
            if (grafo.isEmpty(randX, randY)) {
                x = randX;
                y = randY;
                grafo.grid[x][y] = 'T';  // Marcar la posición del tanque en el grafo
                placed = true;
            }
        }
    }

    // Crear el botón del tanque y establecer el texto
    void createButton(HWND parent, HINSTANCE hInst) {
        hButton = CreateWindow(
            L"BUTTON", tankText.c_str(), WS_VISIBLE | WS_CHILD,
            50 + y * 80, 50 + x * 80, 70, 70, parent, NULL, hInst, NULL
        );
    }
};

// Clase para el tanque celeste
class CelesteTank : public Tank {
public:
    CelesteTank(Grafo& grafo, HWND parent, HINSTANCE hInst) : Tank(L"TCE", grafo, parent, hInst) {}
};

// Clase para el tanque amarillo
class AmarilloTank : public Tank {
public:
    AmarilloTank(Grafo& grafo, HWND parent, HINSTANCE hInst) : Tank(L"TAM", grafo, parent, hInst) {}
};

// Clase para el tanque rojo
class RojoTank : public Tank {
public:
    RojoTank(Grafo& grafo, HWND parent, HINSTANCE hInst) : Tank(L"TRO", grafo, parent, hInst) {}
};

// Clase para el tanque azul
class AzulTank : public Tank {
public:
    AzulTank(Grafo& grafo, HWND parent, HINSTANCE hInst) : Tank(L"TAZ", grafo, parent, hInst) {}
};
