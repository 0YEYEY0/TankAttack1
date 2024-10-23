#pragma once
#include <windows.h>
#include <string>
#include "Tank.h"
#include "Mov.h"
#include "Bala.h"

/*

// Variables para almacenar los power-ups actuales de cada jugador
extern PowerUp* currentPowerUpPlayer1;
extern PowerUp* currentPowerUpPlayer2;
extern HWND hPowerUpText;  // Usado para mostrar el Power-Up aplicado
extern int turnoJugador;   // Variable global que ya existe en Tank.h

// Enum para identificar los tipos de power-ups
enum PowerUpType { DOUBLE_TURN, PRECISION_MOVEMENT, PRECISION_ATTACK, POWER_ATTACK };

// Clase PowerUp para manejar cada uno
class PowerUp {
public:
    PowerUpType type;
    std::wstring description;

    PowerUp(PowerUpType type) : type(type) {
        switch (type) {
        case DOUBLE_TURN:
            description = L"Doble Turno";
            break;
        case PRECISION_MOVEMENT:
            description = L"Precisión de Movimiento";
            break;
        case PRECISION_ATTACK:
            description = L"Precisión de Ataque";
            break;
        case POWER_ATTACK:
            description = L"Poder de Ataque";
            break;
        }
    }
};

// Función para asignar aleatoriamente un power-up al jugador actual
void aplicarPowerUp(HWND hwnd) {
    int randomPowerUp = rand() % 4;  // Generar un power-up aleatorio
    PowerUp* powerUp = new PowerUp(static_cast<PowerUpType>(randomPowerUp));

    if (turnoJugador == 1) {
        currentPowerUpPlayer1 = powerUp;  // Asignar el power-up al jugador 1
        SetWindowText(hPowerUpText, (L"Player 1 obtuvo: " + powerUp->description).c_str());
    }
    else {
        currentPowerUpPlayer2 = powerUp;  // Asignar el power-up al jugador 2
        SetWindowText(hPowerUpText, (L"Player 2 obtuvo: " + powerUp->description).c_str());
    }

    cambiarTurno();  // Cambiar turno después de aplicar power-up
}

// Función para aplicar el power-up de ataque (A*)
void aplicarPowerUpAtaque(Grafo& grafo, int startX, int startY, int goalX, int goalY) {
    if (turnoJugador == 1 && currentPowerUpPlayer1 && currentPowerUpPlayer1->type == PRECISION_ATTACK) {
        MessageBox(NULL, L"Precisión de Ataque (A*) aplicada", L"Power-Up", MB_OK);
        std::vector<int> ruta = AStar(grafo, startX, startY, goalX, goalY);
        // Usar la ruta para mover la bala
        currentPowerUpPlayer1 = nullptr;  // Consumir el power-up
    }
    else if (turnoJugador == 2 && currentPowerUpPlayer2 && currentPowerUpPlayer2->type == PRECISION_ATTACK) {
        MessageBox(NULL, L"Precisión de Ataque (A*) aplicada", L"Power-Up", MB_OK);
        std::vector<int> ruta = AStar(grafo, startX, startY, goalX, goalY);
        currentPowerUpPlayer2 = nullptr;  // Consumir el power-up
    }
}

// Función para aplicar el power-up de movimiento (Dijkstra o BFS)
void aplicarPowerUpMovimiento(Grafo& grafo, int start, int target) {
    if (turnoJugador == 1 && currentPowerUpPlayer1 && currentPowerUpPlayer1->type == PRECISION_MOVEMENT) {
        if (rand() % 100 < 90) {  // 90% de precisión
            MessageBox(NULL, L"Precisión de Movimiento aplicada", L"Power-Up", MB_OK);
            std::vector<int> ruta = BFS(grafo, start, target);  // Usar BFS para movimiento
        }
        currentPowerUpPlayer1 = nullptr;  // Consumir el power-up
    }
    else if (turnoJugador == 2 && currentPowerUpPlayer2 && currentPowerUpPlayer2->type == PRECISION_MOVEMENT) {
        if (rand() % 100 < 90) {
            MessageBox(NULL, L"Precisión de Movimiento aplicada", L"Power-Up", MB_OK);
            std::vector<int> ruta = Dijkstra(grafo, start, target);  // Usar Dijkstra para movimiento
        }
        currentPowerUpPlayer2 = nullptr;  // Consumir el power-up
    }
}

// Función para aplicar el power-up de Doble Turno
void aplicarPowerUpDobleTurno() {
    if (turnoJugador == 1 && currentPowerUpPlayer1 && currentPowerUpPlayer1->type == DOUBLE_TURN) {
        MessageBox(NULL, L"Doble Turno aplicado", L"Power-Up", MB_OK);
        // No cambiar el turno para permitir un segundo movimiento
        currentPowerUpPlayer1 = nullptr;  // Consumir el power-up
    }
    else if (turnoJugador == 2 && currentPowerUpPlayer2 && currentPowerUpPlayer2->type == DOUBLE_TURN) {
        MessageBox(NULL, L"Doble Turno aplicado", L"Power-Up", MB_OK);
        // No cambiar el turno para permitir un segundo movimiento
        currentPowerUpPlayer2 = nullptr;  // Consumir el power-up
    }
}

// Función para aplicar el power-up de Poder de Ataque (100% de daño)
void aplicarPowerUpPowerAttack(Tank* selectedTank, Tank* targetTank) {
    if (turnoJugador == 1 && currentPowerUpPlayer1 && currentPowerUpPlayer1->type == POWER_ATTACK) {
        MessageBox(NULL, L"Poder de Ataque aplicado: Daño 100%", L"Power-Up", MB_OK);
        targetTank->health -= 100;  // Daño de 100% al tanque enemigo
        if (targetTank->health < 0) targetTank->health = 0;  // Evitar que la salud sea negativa
        currentPowerUpPlayer1 = nullptr;  // Consumir el power up
    }
    else if (turnoJugador == 2 && currentPowerUpPlayer2 && currentPowerUpPlayer2->type == POWER_ATTACK) {
        MessageBox(NULL, L"Poder de Ataque aplicado: Daño 100%", L"Power-Up", MB_OK);
        targetTank->health -= 100;  // Daño de 100% al tanque enemigo
        if (targetTank->health < 0) targetTank->health = 0;  // Evitar que la salud sea negativa
        currentPowerUpPlayer2 = nullptr;  // Consumir el power up
    }
}
*/