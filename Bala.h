#pragma once
#include <windows.h>
#include <vector>
#include <cmath>
#include <thread>  // Para usar std::this_thread::sleep_for
#include <chrono>  // Para controlar el tiempo
#include "Tank.h"
#include "Grafo.h"

class Bala {
public:
    int startX, startY;  // Posición inicial del disparo
    int targetX, targetY;  // Posición objetivo
    double angle;  // Ángulo de la bala
    bool hasHit;  // Indica si la bala ha impactado
    Tank* shooter;  // Tanque que disparó
    HWND hRouteText;  // Cuadro de texto donde se mostrará la trayectoria
    HWND hDamageText; // Cuadro de texto donde se mostrará el daño total
    Grafo& grafo;  // Referencia al grafo
    std::vector<Tank*>& tanks;  // Referencia a los tanques

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


    // Constructor de la bala
    Bala(Tank* t, int tx, int ty, HWND routeText, HWND damageText, Grafo& g, std::vector<Tank*>& ts)
        : shooter(t), targetX(tx), targetY(ty), hRouteText(routeText), hDamageText(damageText), grafo(g), tanks(ts) {
        startX = shooter->x;
        startY = shooter->y;
        angle = calculateAngle();
        hasHit = false;
    }

    // Función para calcular el ángulo de la bala (línea de vista)
    double calculateAngle() {
        double dx = targetX - startX;
        double dy = targetY - startY;
        return atan2(dy, dx);
    }

    // Función para mover la bala y detectar colisiones
    void moveBala() {
        int currentX = startX;
        int currentY = startY;
        double stepX = cos(angle);  // Paso en el eje X basado en el ángulo
        double stepY = sin(angle);  // Paso en el eje Y basado en el ángulo
        bool hasBounced = false;  // Para rastrear si la bala ya rebotó

        std::wstring path = L"Trayectoria de la bala: ";
        HWND hButton = nullptr;

        while (!hasHit) {
            // Mover la bala
            currentX += static_cast<int>(stepX);
            currentY += static_cast<int>(stepY);

            // Verificar si la bala se sale de los límites del grafo (rebote)
            if (currentX < 0 || currentX >= grafo.N || currentY < 0 || currentY >= grafo.N) {
                if (hasBounced) {
                    break;  // Si ya rebotó antes, la bala desaparece
                }
                angle = reflectAngle(angle);  // Rebotar la bala
                stepX = cos(angle);
                stepY = sin(angle);
                hasBounced = true;
            }

            // Verificar si la bala golpea un obstáculo
            if (grafo.grid[currentX][currentY] == 'X') {
                if (hasBounced) {
                    break;  // Si ya rebotó antes, la bala desaparece
                }
                angle = reflectAngle(angle);  // Rebotar en la pared
                stepX = cos(angle);
                stepY = sin(angle);
                hasBounced = true;
            }

            // Agregar la nueva posición al trayecto
            path += L"(" + std::to_wstring(currentX) + L", " + std::to_wstring(currentY) + L") ";

            // Mostrar la "Q" en el botón de la posición actual
            hButton = GetDlgItem(GetParent(hRouteText), currentX * grafo.N + currentY);
            if (hButton) {
                SetWindowText(hButton, L"Q");
                std::this_thread::sleep_for(std::chrono::milliseconds(500));  // Pausa de 0.5 segundos
                SetWindowText(hButton, L"O");  // Restaurar la celda a su estado original después de que la bala pasa
            }

            // Verificar si la bala golpea un tanque
            for (Tank* tank : tanks) {
                if (tank->x == currentX && tank->y == currentY) {
                    applyDamage(tank);  // Aplicar daño
                    hasHit = true;
                    break;
                }
            }
        }

        // Mostrar la trayectoria de la bala en la interfaz
        SetWindowText(hRouteText, path.c_str());
    }

    // Función para aplicar el daño al tanque
    void applyDamage(Tank* tank) {
        if (tank->tankText == L"TCE" || tank->tankText == L"TAZ") {
            tank->damage += 25;  // 25% de daño a tanques celeste y azul
        }
        else if (tank->tankText == L"TAM" || tank->tankText == L"TRO") {
            tank->damage += 50;  // 50% de daño a tanques amarillo y rojo
        }

        // Mostrar el daño actualizado en la interfaz
        std::wstring damageText = L"Daño recibido por " + tank->tankText + L": " + std::to_wstring(tank->damage) + L"%";
        SetWindowText(hDamageText, damageText.c_str());
    }

    // Función para reflejar la bala al chocar con una pared
    double reflectAngle(double angle) {
        return angle + M_PI / 2;  // Rebote de 90 grados
    }
};
