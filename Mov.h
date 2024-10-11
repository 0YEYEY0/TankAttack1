#pragma once
#include <iostream>
#include <queue>
#include <vector>
#include <limits>
#include <algorithm>
#include "Grafo.h"

// Función para encontrar el camino más corto usando Dijkstra
std::vector<int> Dijkstra(const Grafo& grafo, int start, int target, HWND hRouteText) {
    int N = grafo.N;
    std::vector<int> dist(N * N, std::numeric_limits<int>::max());  // Distancias
    std::vector<int> prev(N * N, -1);  // Predecesores para reconstruir el camino
    std::vector<bool> visited(N * N, false);  // Nodos visitados
    dist[start] = 0;  // La distancia al nodo inicial es 0

    for (int count = 0; count < N * N; count++) {
        int u = -1;
        for (int i = 0; i < N * N; i++) {
            if (!visited[i] && (u == -1 || dist[i] < dist[u])) {
                u = i;
            }
        }

        if (dist[u] == std::numeric_limits<int>::max()) break;  // No hay más nodos alcanzables

        visited[u] = true;

        // Actualizar las distancias de los vecinos
        for (int v = 0; v < N * N; v++) {
            if (grafo.adjMatrix[u][v] != 0 && !visited[v] && dist[u] + grafo.adjMatrix[u][v] < dist[v]) {
                dist[v] = dist[u] + grafo.adjMatrix[u][v];
                prev[v] = u;  // Almacenar el predecesor
            }
        }
    }

    // Reconstruir el camino si se puede llegar al destino
    std::vector<int> path;
    for (int at = target; at != -1; at = prev[at]) {
        path.push_back(at);
    }
    std::reverse(path.begin(), path.end());

    // Mostrar el recorrido en el cuadro de texto
    std::wstring routeText = L"Recorrido Dijkstra: ";
    for (int pos : path) {
        int x = pos / N;
        int y = pos % N;
        routeText += L"(" + std::to_wstring(x) + L", " + std::to_wstring(y) + L") ";
    }

    // Actualizar el cuadro de texto
    SetWindowText(hRouteText, routeText.c_str());

    return dist;
}

// Función para realizar búsqueda en anchura (BFS)
std::vector<int> BFS(const Grafo& grafo, int start, int target, HWND hRouteText) {
    int N = grafo.N;
    std::vector<int> dist(N * N, -1);  // Distancias (inicialmente -1 para nodos no alcanzables)
    std::vector<int> prev(N * N, -1);  // Predecesores para reconstruir el camino
    std::queue<int> q;  // Cola para BFS
    dist[start] = 0;  // La distancia al nodo inicial es 0
    q.push(start);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        // Visitar los vecinos del nodo actual
        for (int v = 0; v < N * N; v++) {
            if (grafo.adjMatrix[u][v] != 0 && dist[v] == -1) {  // Nodo no visitado
                dist[v] = dist[u] + 1;
                prev[v] = u;  // Almacenar el predecesor
                q.push(v);
            }
        }
    }

    // Reconstruir el camino si se puede llegar al destino
    std::vector<int> path;
    for (int at = target; at != -1; at = prev[at]) {
        path.push_back(at);
    }
    std::reverse(path.begin(), path.end());

    // Mostrar el recorrido en el cuadro de texto
    std::wstring routeText = L"Recorrido BFS: ";
    for (int pos : path) {
        int x = pos / N;
        int y = pos % N;
        routeText += L"(" + std::to_wstring(x) + L", " + std::to_wstring(y) + L") ";
    }

    // Actualizar el cuadro de texto
    SetWindowText(hRouteText, routeText.c_str());

    return dist;
}
