#pragma once
#include <vector>
#include <cstdlib>  
#include <ctime>    
#include <iostream>  

class Grafo {
public:
    int N;  // Tama�o del grafo (NxN)
    std::vector<std::vector<int>> adjMatrix;  // Matriz de adyacencia
    std::vector<std::vector<char>> grid;  // Representaci�n de la cuadr�cula 'O' = vac�o, 'X' = obst�culo

    // Constructor
    Grafo(int size) : N(size) {
        adjMatrix.resize(N * N, std::vector<int>(N * N, 0));  // Inicializar la matriz de adyacencia
        grid.resize(N, std::vector<char>(N, 'O'));  // Inicializar la cuadr�cula vac�a
        generateObstacles();  // Generar obst�culos
        generateAdjacencyMatrix();  // Generar la matriz de adyacencia
    }

    // Generar obst�culos aleatorios
    void generateObstacles() {
        srand(static_cast<unsigned int>(time(0)));  // Inicializar generador de n�meros aleatorios
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (rand() % 4 == 0) {  // Probabilidad de ser obst�culo
                    grid[i][j] = 'X';  // Colocar un obst�culo
                }
            }
        }
    }

    // Verificar si una celda est� vac�a
    bool isEmpty(int x, int y) const {
        return grid[x][y] == 'O';
    }

    // Obtener el �ndice de la celda en la matriz de adyacencia
    int getIndex(int x, int y) const {
        return x * N + y;
    }

    // Conectar dos nodos en la matriz de adyacencia
    void addEdge(int u, int v) {
        adjMatrix[u][v] = 1;
        adjMatrix[v][u] = 1;  // El grafo es no dirigido
    }

    // Generar la matriz de adyacencia seg�n la cuadr�cula y los obst�culos
    void generateAdjacencyMatrix() {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (isEmpty(i, j)) {
                    int index = getIndex(i, j);
                    // Conectar con las celdas vecinas si est�n vac�as
                    if (i > 0 && isEmpty(i - 1, j)) addEdge(index, getIndex(i - 1, j));  // Arriba
                    if (i < N - 1 && isEmpty(i + 1, j)) addEdge(index, getIndex(i + 1, j));  // Abajo
                    if (j > 0 && isEmpty(i, j - 1)) addEdge(index, getIndex(i, j - 1));  // Izquierda
                    if (j < N - 1 && isEmpty(i, j + 1)) addEdge(index, getIndex(i, j + 1));  // Derecha
                }
            }
        }
    }

    void printAdjMatrix() const {
        for (int i = 0; i < N * N; ++i) {
            for (int j = 0; j < N * N; ++j) {
                std::cout << adjMatrix[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
};
