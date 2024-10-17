#pragma once
#include <windows.h>
#include <vector>
#include <queue>
#include <cmath>
#include <thread>
#include <chrono>
#include "Tank.h"
#include "Grafo.h"

class Bala {
public:
    int startX, startY;
    int targetX, targetY;
    bool hasHit;
    Tank* shooter;
    HWND hRouteText;
    HWND hDamageText;
    Grafo& grafo;
    std::vector<Tank*>& tanks;

    // Constructor de la bala
    Bala(Tank* t, int tx, int ty, HWND routeText, HWND damageText, Grafo& g, std::vector<Tank*>& ts)
        : shooter(t), targetX(tx), targetY(ty), hRouteText(routeText), hDamageText(damageText), grafo(g), tanks(ts) {
        startX = shooter->x;
        startY = shooter->y;
        hasHit = false;
    }

    // Estructura para representar las celdas del grafo
    struct Cell {
        int x, y;
        std::vector<Cell> path;  // Mantener el camino hacia la celda
    };

    // Funci�n BFS para encontrar el camino de la bala hasta el objetivo
    std::vector<Cell> BFS(int startX, int startY, int targetX, int targetY) {
        std::vector<std::vector<bool>> visited(grafo.N, std::vector<bool>(grafo.N, false));
        std::queue<Cell> q;
        q.push({ startX, startY, {} });
        visited[startX][startY] = true;

        // Movimientos posibles: arriba, abajo, izquierda, derecha
        int moves[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };

        while (!q.empty()) {
            Cell current = q.front();
            q.pop();

            // Si alcanzamos el objetivo, devolver el camino
            if (current.x == targetX && current.y == targetY) {
                return current.path;
            }

            // Explorar celdas adyacentes
            for (auto& move : moves) {
                int newX = current.x + move[0];
                int newY = current.y + move[1];

                // Verificar si la nueva celda est� dentro de los l�mites y no es un obst�culo
                if (newX >= 0 && newX < grafo.N && newY >= 0 && newY < grafo.N && grafo.grid[newX][newY] != 'X' && !visited[newX][newY]) {
                    visited[newX][newY] = true;
                    Cell nextCell = { newX, newY, current.path };
                    nextCell.path.push_back({ newX, newY });
                    q.push(nextCell);
                }
            }
        }

        // Si no se encuentra un camino, devolver un vector vac�o
        return {};
    }

    // Funci�n para mover la bala usando BFS
    void moveBala() {
        // Obtener el camino usando BFS
        std::vector<Cell> path = BFS(startX, startY, targetX, targetY);
        if (path.empty()) {
            SetWindowText(hRouteText, L"No se encontr� un camino");
            return;
        }

        std::wstring pathText = L"Trayectoria de la bala: ";
        HWND hButton = nullptr;

        // Recorrer el camino y mostrar la bala movi�ndose
        for (auto& cell : path) {
            // Mostrar la "Q" en el bot�n de la posici�n actual
            hButton = GetDlgItem(GetParent(hRouteText), cell.x * grafo.N + cell.y);
            if (hButton) {
                SetWindowText(hButton, L"Q");
                std::this_thread::sleep_for(std::chrono::milliseconds(500));  // Pausa de 0.5 segundos
                SetWindowText(hButton, L"O");  // Restaurar la celda a su estado original despu�s de que la bala pasa
            }

            // Agregar la posici�n al texto de la trayectoria
            pathText += L"(" + std::to_wstring(cell.x) + L", " + std::to_wstring(cell.y) + L") ";

            // Verificar si la bala golpea un tanque
            for (Tank* tank : tanks) {
                if (tank->x == cell.x && tank->y == cell.y) {
                    applyDamage(tank);  // Aplicar da�o al tanque
                    hasHit = true;
                    SetWindowText(hRouteText, pathText.c_str());
                    return;
                }
            }
        }

        // Mostrar la trayectoria completa en la interfaz
        SetWindowText(hRouteText, pathText.c_str());
    }

    // Funci�n para aplicar el da�o al tanque
    void applyDamage(Tank* tank) {
        std::wstring damageText;

        // Aplicar el da�o seg�n el color del tanque
        if (tank->tankText == L"TCE" || tank->tankText == L"TAZ") {  // Tanques celeste y azul
            tank->health -= 25;  // 25% de da�o
            damageText = tank->tankText + L" recibe 25% de da�o";
        }
        else if (tank->tankText == L"TAM" || tank->tankText == L"TRO") {  // Tanques amarillo y rojo
            tank->health -= 50;  // 50% de da�o
            damageText = tank->tankText + L" recibe 50% de da�o";
        }

        // Mostrar el da�o actualizado en la interfaz
        SetWindowText(hDamageText, damageText.c_str());

        // Asegurarse de que la vida no baje de 0
        if (tank->health < 0) {
            tank->health = 0;
        }
    }
};
