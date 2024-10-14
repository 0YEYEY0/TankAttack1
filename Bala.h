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
    int startX, startY;  // Posici�n inicial del disparo
    int targetX, targetY;  // Posici�n objetivo
    double angle;  // �ngulo de la bala
    bool hasHit;  // Indica si la bala ha impactado
    Tank* shooter;  // Tanque que dispar�
    HWND hRouteText;  // Cuadro de texto donde se mostrar� la trayectoria
    HWND hDamageText; // Cuadro de texto donde se mostrar� el da�o total
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

    // Funci�n para calcular el �ngulo de la bala (l�nea de vista)
    double calculateAngle() {
        double dx = targetX - startX;
        double dy = targetY - startY;
        return atan2(dy, dx);
    }

    // Funci�n para mover la bala y detectar colisiones
    void moveBala() {
        int currentX = startX;
        int currentY = startY;
        double stepX = cos(angle);  // Paso en el eje X basado en el �ngulo
        double stepY = sin(angle);  // Paso en el eje Y basado en el �ngulo
        bool hasBounced = false;  // Para rastrear si la bala ya rebot�

        std::wstring path = L"Trayectoria de la bala: ";
        HWND hButton = nullptr;

        while (!hasHit) {
            // Mover la bala
            currentX += static_cast<int>(stepX);
            currentY += static_cast<int>(stepY);

            // Verificar si la bala se sale de los l�mites del grafo (rebote)
            if (currentX < 0 || currentX >= grafo.N || currentY < 0 || currentY >= grafo.N) {
                if (hasBounced) {
                    break;  // Si ya rebot� antes, la bala desaparece
                }
                angle = reflectAngle(angle);  // Rebotar la bala
                stepX = cos(angle);
                stepY = sin(angle);
                hasBounced = true;
            }

            // Verificar si la bala golpea un obst�culo
            if (grafo.grid[currentX][currentY] == 'X') {
                if (hasBounced) {
                    break;  // Si ya rebot� antes, la bala desaparece
                }
                angle = reflectAngle(angle);  // Rebotar en la pared
                stepX = cos(angle);
                stepY = sin(angle);
                hasBounced = true;
            }

            // Agregar la nueva posici�n al trayecto
            path += L"(" + std::to_wstring(currentX) + L", " + std::to_wstring(currentY) + L") ";

            // Mostrar la "Q" en el bot�n de la posici�n actual
            hButton = GetDlgItem(GetParent(hRouteText), currentX * grafo.N + currentY);
            if (hButton) {
                SetWindowText(hButton, L"Q");
                std::this_thread::sleep_for(std::chrono::milliseconds(500));  // Pausa de 0.5 segundos
                SetWindowText(hButton, L"O");  // Restaurar la celda a su estado original despu�s de que la bala pasa
            }

            // Verificar si la bala golpea un tanque
            for (Tank* tank : tanks) {
                if (tank->x == currentX && tank->y == currentY) {
                    applyDamage(tank);  // Aplicar da�o
                    hasHit = true;
                    break;
                }
            }
        }

        // Mostrar la trayectoria de la bala en la interfaz
        SetWindowText(hRouteText, path.c_str());
    }

    // Funci�n para aplicar el da�o al tanque
    void applyDamage(Tank* tank) {
        if (tank->tankText == L"TCE" || tank->tankText == L"TAZ") {
            tank->damage += 25;  // 25% de da�o a tanques celeste y azul
        }
        else if (tank->tankText == L"TAM" || tank->tankText == L"TRO") {
            tank->damage += 50;  // 50% de da�o a tanques amarillo y rojo
        }

        // Mostrar el da�o actualizado en la interfaz
        std::wstring damageText = L"Da�o recibido por " + tank->tankText + L": " + std::to_wstring(tank->damage) + L"%";
        SetWindowText(hDamageText, damageText.c_str());
    }

    // Funci�n para reflejar la bala al chocar con una pared
    double reflectAngle(double angle) {
        return angle + M_PI / 2;  // Rebote de 90 grados
    }
};
