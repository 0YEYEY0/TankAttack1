#pragma once
#include <vector>
#include <queue>
#include <limits>
#include "Grafo.h"

// Función para encontrar el camino más corto usando Dijkstra
std::vector<int> Dijkstra(const Grafo& grafo, int start) {
    int N = grafo.N;
    std::vector<int> dist(N * N, std::numeric_limits<int>::max());  // Distancias
    std::vector<bool> visited(N * N, false);  // Nodos visitados
    dist[start] = 0;  // La distancia al nodo inicial es 0

    for (int count = 0; count < N * N; count++) {
        // Encontrar el nodo con la distancia mínima que no ha sido visitado
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
            if (grafo.adj[u][v] != 0 && !visited[v] && dist[u] + grafo.adj[u][v] < dist[v]) {
                dist[v] = dist[u] + grafo.adj[u][v];
            }
        }
    }

    return dist;  // Retornar las distancias desde el nodo inicial
}

// Función para realizar búsqueda en anchura (BFS)
std::vector<int> BFS(const Grafo& grafo, int start) {
    int N = grafo.N;
    std::vector<int> dist(N * N, -1);  // Distancias (inicialmente -1 para nodos no alcanzables)
    std::queue<int> q;  // Cola para BFS
    dist[start] = 0;  // La distancia al nodo inicial es 0
    q.push(start);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        // Visitar los vecinos del nodo actual
        for (int v = 0; v < N * N; v++) {
            if (grafo.adj[u][v] != 0 && dist[v] == -1) {  // Nodo no visitado
                dist[v] = dist[u] + 1;
                q.push(v);
            }
        }
    }

    return dist;  // Retornar las distancias desde el nodo inicial
}
