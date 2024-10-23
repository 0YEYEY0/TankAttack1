#pragma once
#include <windows.h>
#include <string>
#include "Tank.h"


// Declaración globales
extern HWND hPowerUpText;  // Cuadro de texto para mostrar el Power-Up activo
extern int turnoJugador;   // Variable global que ya existe en Tank.h
extern PowerUp* currentPowerUpPlayer1;  // Power-Up actual de Player 1
extern PowerUp* currentPowerUpPlayer2;  // Power-Up actual de Player 2

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
inline void aplicarPowerUp(HWND hwnd) {
    int randomPowerUp = rand() % 4;  // Generar un power-up aleatorio
    PowerUp* powerUp = new PowerUp(static_cast<PowerUpType>(randomPowerUp));

    std::wstring resultText;
    if (turnoJugador == 1) {
        currentPowerUpPlayer1 = powerUp;  // Asignar el power-up al jugador 1
        resultText = L"Player 1 obtuvo: " + powerUp->description;
    }
    else {
        currentPowerUpPlayer2 = powerUp;  // Asignar el power-up al jugador 2
        resultText = L"Player 2 obtuvo: " + powerUp->description;
    }

    // Mostrar el power-up aplicado en la interfaz
    SetWindowText(hPowerUpText, resultText.c_str());

    // Cambiar el turno después de usar el power-up
    cambiarTurno();
}

// Función para aplicar el power-up de movimiento
inline void aplicarPowerUpMovimiento() {
    if (turnoJugador == 1 && currentPowerUpPlayer1 && currentPowerUpPlayer1->type == PRECISION_MOVEMENT) {
        if (rand() % 100 < 90) {  // 90% de precisión
            MessageBox(NULL, L"Precisión de Movimiento aplicada", L"Power-Up", MB_OK);
            // Aquí aplicas BFS o Dijkstra basado en el color del tanque
        }
        currentPowerUpPlayer1 = nullptr;  // Consumir el power-up
    }
    else if (turnoJugador == 2 && currentPowerUpPlayer2 && currentPowerUpPlayer2->type == PRECISION_MOVEMENT) {
        if (rand() %
