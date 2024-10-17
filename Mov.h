#pragma once
#define NOMINMAX  // Desactivar las macros min y max de windows.h
#include <iostream>
#include <queue>
#include <vector>
#include <limits>
#include <algorithm>
#include <ctime>  // Para srand y rand
#include <windows.h>  // Para HWND y SetWindowText
#include <sstream>  // Para la conversión manual a wstring
#include "Grafo.h"
#include "Tank.h"

// Función para convertir int a wstring si std::to_wstring no está disponible
std::wstring to_wstring(int value) {
    std::wstringstream wss;
    wss << value;
    return wss.str();
}

void MoveTankWithTrajectory(Tank* tank, int targetX, int targetY, HWND hwnd);


// Función para encontrar el camino más corto usando Dijkstra
std::vector<int> Dijkstra(const Grafo& grafo, int start, int target, HWND hRouteText) {
    int N = grafo.N;
    std::vector<int> dist(N * N, std::numeric_limits<int>::max());
    std::vector<int> prev(N * N, -1);
    std::vector<bool> visited(N * N, false);
    dist[start] = 0;

    for (int count = 0; count < N * N; count++) {
        int u = -1;
        for (int i = 0; i < N * N; i++) {
            if (!visited[i] && (u == -1 || dist[i] < dist[u])) {
                u = i;
            }
        }

        if (dist[u] == std::numeric_limits<int>::max()) break;

        visited[u] = true;

        for (int v = 0; v < N * N; v++) {
            if (grafo.adjMatrix[u][v] != 0 && !visited[v] && dist[u] + grafo.adjMatrix[u][v] < dist[v]) {
                dist[v] = dist[u] + grafo.adjMatrix[u][v];
                prev[v] = u;
            }
        }
    }

    std::vector<int> path;
    for (int at = target; at != -1; at = prev[at]) {
        path.push_back(at);
    }
    std::reverse(path.begin(), path.end());

    std::wstring routeText = L"Recorrido Dijkstra: ";
    for (int pos : path) {
        int x = pos / N;
        int y = pos % N;
        routeText += L"(" + to_wstring(x) + L", " + to_wstring(y) + L") ";
    }
    SetWindowText(hRouteText, routeText.c_str());

    return path;
}

// Función para realizar búsqueda BFS
std::vector<int> BFS(const Grafo& grafo, int start, int target, HWND hRouteText) {
    int N = grafo.N;
    std::vector<int> dist(N * N, -1);
    std::vector<int> prev(N * N, -1);
    std::queue<int> q;
    dist[start] = 0;
    q.push(start);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v = 0; v < N * N; v++) {
            if (grafo.adjMatrix[u][v] != 0 && dist[v] == -1) {
                dist[v] = dist[u] + 1;
                prev[v] = u;
                q.push(v);
            }
        }
    }

    std::vector<int> path;
    for (int at = target; at != -1; at = prev[at]) {
        path.push_back(at);
    }
    std::reverse(path.begin(), path.end());

    std::wstring routeText = L"Recorrido BFS: ";
    for (int pos : path) {
        int x = pos / N;
        int y = pos % N;
        routeText += L"(" + to_wstring(x) + L", " + to_wstring(y) + L") ";
    }
    SetWindowText(hRouteText, routeText.c_str());

    return path;
}

// Función para mover el tanque a una celda vacía y sin obstáculos
void moveTankToEmptyCell(Tank* tank, const Grafo& grafo, HWND hRouteText) {
    int N = grafo.N;
    int currentPos = tank->x * N + tank->y;

    std::vector<int> possibleMoves;

    if (tank->x > 0 && grafo.isEmpty(tank->x - 1, tank->y)) {
        possibleMoves.push_back(currentPos - N);
    }
    if (tank->x < N - 1 && grafo.isEmpty(tank->x + 1, tank->y)) {
        possibleMoves.push_back(currentPos + N);
    }
    if (tank->y > 0 && grafo.isEmpty(tank->x, tank->y - 1)) {
        possibleMoves.push_back(currentPos - 1);
    }
    if (tank->y < N - 1 && grafo.isEmpty(tank->x, tank->y + 1)) {
        possibleMoves.push_back(currentPos + 1);
    }

    if (!possibleMoves.empty()) {
        int randomMove = rand() % possibleMoves.size();
        int newX = possibleMoves[randomMove] / N;
        int newY = possibleMoves[randomMove] % N;

        // Actualizar la posición del tanque
        tank->x = newX;
        tank->y = newY;

        // Actualizar la interfaz visualmente
        std::wstring routeText = L"Tanque movido a: (" + to_wstring(newX) + L", " + to_wstring(newY) + L")";
        SetWindowText(hRouteText, routeText.c_str());
    }
    else {
        // Si no hay celdas vacías, no mover el tanque
        SetWindowText(hRouteText, L"No hay celdas vacías para moverse.");
    }
}


void moveTankWithProbabilities(Tank* tank, Grafo& grafo, HWND hwnd) {
    srand(static_cast<unsigned int>(time(0)));  // Semilla para números aleatorios
    int randomValue = rand() % 100;  // Número aleatorio entre 0 y 99

    if (tank->tankText == L"TCE" || tank->tankText == L"TAZ") {
        // Tanques celeste y azul (50% de usar BFS, 50% movimiento aleatorio)
        if (randomValue < 50) {
            // Usar BFS y pedir al jugador que elija una celda
            MessageBox(hwnd, L"Usando BFS. Selecciona una celda", L"Movimiento", MB_OK);
            // Aquí llamas a la lógica de BFS
        }
        else {
            // Mover a una celda vacía aleatoria
            int randomX, randomY;
            do {
                randomX = rand() % grafo.N;
                randomY = rand() % grafo.N;
            } while (!grafo.isEmpty(randomX, randomY));  // Asegurarse de que la celda esté vacía

            MoveTankWithTrajectory(tank, randomX, randomY, hwnd);
            MessageBox(hwnd, L"Moviendo a celda aleatoria", L"Movimiento", MB_OK);
        }
    }
    else if (tank->tankText == L"TAM" || tank->tankText == L"TRO") {
        // Tanques amarillo y rojo (80% de usar Dijkstra, 20% movimiento aleatorio)
        if (randomValue < 80) {
            MessageBox(hwnd, L"Usando Dijkstra. Selecciona una celda", L"Movimiento", MB_OK);
            // Aquí llamas a la lógica de Dijkstra
        }
        else {
            // Mover a una celda vacía aleatoria
            int randomX, randomY;
            do {
                randomX = rand() % grafo.N;
                randomY = rand() % grafo.N;
            } while (!grafo.isEmpty(randomX, randomY));  // Asegurarse de que la celda esté vacía

            MoveTankWithTrajectory(tank, randomX, randomY, hwnd);
            MessageBox(hwnd, L"Moviendo a celda aleatoria", L"Movimiento", MB_OK);
        }
    }
}





