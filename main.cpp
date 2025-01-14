#define NOMINMAX  
#include <windows.h>
#include "Grafo.h"
#include "Tank.h"
#include "Bala.h"
#include <vector>
#include <ctime>  
#include <thread>  
#include <chrono>  
#include "Mov.h"
#include "P_Up.h"

const int N = 5;  // Tamaño del grafo
Grafo* grafo;
std::vector<Tank*> tanks;  // Almacena los punteros a los tanques
Tank* selectedTank = nullptr;  // Tanque seleccionado para disparar o mover
bool isMoving = false;  // Flag para manejar si se está moviendo un tanque
bool isShooting = false;  // Flag para manejar si se está disparando
int remainingTime = 300;  // Tiempo 

HWND hRouteText;  // Cuadro de texto para mostrar la trayectoria de la bala
HWND hDamageText;  // Cuadro de texto para mostrar el daño de los tanques
HWND hTankHealth[4];  // Cuadros de texto para mostrar la vida de los 4 tanques
HWND hTimerText;    // Cuadro del temporizador


void MoveTankWithTrajectory(Tank* tank, int targetX, int targetY, HWND hwnd);  // Nueva función para mostrar la trayectoria
void UpdateTankButton(Tank* tank, HWND hwnd);  // Declaración para actualizar el botón de los tanques
void ClearOldTankPosition(Tank* tank, HWND hwnd);  // Declaración para limpiar la posición original del tanque
void UpdateTankHealth();  // Función para actualizar la vida de los tanques en la interfaz
void UpdateTimer(HWND hwnd);  // Actualizar el temporizador
void MoveTank(Tank* tank, int targetX, int targetY, HWND hwnd);  // Declaración de MoveTank


HWND hPowerUpText;  // Cuadro de texto para mostrar el power up activo
HWND hPowerUpButton;  // Botón para aplicar power up



LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

// Crear la ventana
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
    WNDCLASS wc = { 0 };
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"TankAttackWindow";
    wc.lpfnWndProc = WindowProcedure;

    if (!RegisterClass(&wc))
        return -1;

    grafo = new Grafo(N);  // Crear el grafo con obstáculos
    srand((unsigned)time(0));  // Inicializar random

    HWND hwnd = CreateWindow(L"TankAttackWindow", L"Tank Attack Game", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 800, 700, 700, 800, NULL, NULL, hInst, NULL);

    // Iniciar temporizador (1 segundo = 1000ms)
    SetTimer(hwnd, 1, 1000, NULL);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, NULL, NULL)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Limpiar la memoria de los tanques
    for (auto tank : tanks) {
        delete tank;
    }

    return 0;
}

void iniciarTemporizador(HWND hwnd) {
    // Configurar el temporizador para que dispare un mensaje WM_TIMER cada  1 segundo
    SetTimer(hwnd, 1, 1000, NULL);  
}


LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    static HWND hMoveButtons[4];  // Botones de movimiento para cada tanque
    static HWND hFireButtons[4];  // Botones de disparo para cada tanque
    HWND hButton;

    switch (msg) {
    case WM_CREATE: {
        // Crear botones para representar el grafo
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                hButton = CreateWindow(
                    L"BUTTON",
                    NULL,
                    WS_VISIBLE | WS_CHILD,
                    50 + j * 80, 50 + i * 80, 70, 70,
                    hwnd, (HMENU)(intptr_t)(i * N + j),
                    (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                    NULL
                );

                // Si es obstáculo, marcar con X
                if (grafo->grid[i][j] == 'X') {
                    SetWindowText(hButton, L"X");
                }
                else {
                    SetWindowText(hButton, L"O");
                }
            }
        }

        // Crear botones de movimiento para los tanques
        hMoveButtons[0] = CreateWindow(L"BUTTON", L"Mov Celeste", WS_VISIBLE | WS_CHILD, 500, 50, 120, 50, hwnd, (HMENU)2001, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hMoveButtons[1] = CreateWindow(L"BUTTON", L"Mov Amarillo", WS_VISIBLE | WS_CHILD, 500, 110, 120, 50, hwnd, (HMENU)2002, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hMoveButtons[2] = CreateWindow(L"BUTTON", L"Mov Rojo", WS_VISIBLE | WS_CHILD, 500, 170, 120, 50, hwnd, (HMENU)2003, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hMoveButtons[3] = CreateWindow(L"BUTTON", L"Mov Azul", WS_VISIBLE | WS_CHILD, 500, 230, 120, 50, hwnd, (HMENU)2004, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

        // Crear botones de disparo para los tanques
        hFireButtons[0] = CreateWindow(L"BUTTON", L"Disparar Celeste", WS_VISIBLE | WS_CHILD, 500, 300, 120, 50, hwnd, (HMENU)1001, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hFireButtons[1] = CreateWindow(L"BUTTON", L"Disparar Amarillo", WS_VISIBLE | WS_CHILD, 500, 360, 120, 50, hwnd, (HMENU)1002, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hFireButtons[2] = CreateWindow(L"BUTTON", L"Disparar Rojo", WS_VISIBLE | WS_CHILD, 500, 420, 120, 50, hwnd, (HMENU)1003, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hFireButtons[3] = CreateWindow(L"BUTTON", L"Disparar Azul", WS_VISIBLE | WS_CHILD, 500, 480, 120, 50, hwnd, (HMENU)1004, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

        //  cuadros de texto 
        hRouteText = CreateWindow(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_READONLY, 50, 550, 400, 60, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hDamageText = CreateWindow(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_READONLY, 50, 620, 400, 60, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

        // Crear cuadros de texto para la vida de los tanques
        hTankHealth[0] = CreateWindow(L"STATIC", L"Vida Celeste: 100", WS_VISIBLE | WS_CHILD, 500, 550, 150, 30, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hTankHealth[1] = CreateWindow(L"STATIC", L"Vida Amarillo: 100", WS_VISIBLE | WS_CHILD, 500, 580, 150, 30, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hTankHealth[2] = CreateWindow(L"STATIC", L"Vida Rojo: 100", WS_VISIBLE | WS_CHILD, 500, 610, 150, 30, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hTankHealth[3] = CreateWindow(L"STATIC", L"Vida Azul: 100", WS_VISIBLE | WS_CHILD, 500, 640, 150, 30, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

        // Crear tanques y colocar en el grafo
        tanks.push_back(new CelesteTank(*grafo, hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE)));
        tanks.push_back(new AmarilloTank(*grafo, hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE)));
        tanks.push_back(new RojoTank(*grafo, hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE)));
        tanks.push_back(new AzulTank(*grafo, hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE)));

        hTimerText = CreateWindow(L"STATIC", L"Tiempo restante: 05:00", WS_VISIBLE | WS_CHILD, 500, 700, 200, 30, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

        iniciarTemporizador(hwnd);  // Iniciar el temporizador del juego


        // Crear botón para aplicar power up y cuadro de texto para mostrar el power-up aplicado
        hPowerUpButton = CreateWindow(L"BUTTON", L"Usar Power-Up", WS_VISIBLE | WS_CHILD, 300, 700, 120, 50, hwnd, (HMENU)3001, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hPowerUpText = CreateWindow(L"STATIC", L"", WS_VISIBLE | WS_CHILD, 600, 360, 200, 30, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);


        break;
    }

    case WM_COMMAND: {
        int id = LOWORD(wp);

        // Si se selecciona una celda en el grafo para mover el tanque o disparar
        if (id < 1000 && selectedTank != nullptr) {
            int targetX = id / N;
            int targetY = id % N;

            // Mover el tanque si el flag de movimiento está activado
            if (isMoving) {
                // Aquí llamamos a moveTankWithProbabilities 
                moveTankWithProbabilities(selectedTank, *grafo, hwnd, targetX, targetY, hRouteText);
                isMoving = false;
                cambiarTurno();  // Cambiar el turno después del movimiento
            }

            // Disparar la bala si el flag de disparo está activado
            if (isShooting) {
                Bala bala(selectedTank, targetX, targetY, hRouteText, hDamageText, *grafo, tanks);
                bala.moveBala();
                UpdateTankHealth();  // Actualizar la vida de los tanques después del disparo
                isShooting = false;  // Desactivar flag de disparo
                cambiarTurno();  // Cambiar el turno después de disparar
            }

            selectedTank = nullptr;  // Deseleccionar el tanque después de mover o disparar
        }

        // Manejar la selección de tanque para mover
        if (id >= 2001 && id <= 2004) {
            selectedTank = tanks[id - 2001];  // Selecciona el tanque correspondiente para mover
            if (esTurnoDelJugador(selectedTank)) {  // Verificar si es el turno del jugador
                isMoving = true;  // Activar flag de movimiento
            }
            else {
                MessageBox(hwnd, L"No es tu turno", L"Error", MB_OK);
            }
        }
        /*
        // Manejar Power-Up
        if (id == 3001) {
            aplicarPowerUp(hwnd);  // Función para asignar un Power-Up aleatorio al jugador actual
        }
        */

        // Manejar la selección de tanque para disparar
        if (id >= 1001 && id <= 1004) {
            selectedTank = tanks[id - 1001];  // Selecciona el tanque correspondiente para disparar
            if (esTurnoDelJugador(selectedTank)) {  // Verificar si es el turno del jugador
                isShooting = true;  // Activar flag de disparo
                MessageBox(hwnd, L"Selecciona una celda para disparar", L"Disparo", MB_OK);
            }
            else {
                MessageBox(hwnd, L"No es tu turno", L"Error", MB_OK);
            }
        }

        break;
    }



    case WM_TIMER: {
        // Actualizar el temporizador
        if (remainingTime > 0) {
            UpdateTimer(hwnd);
        }
        else {
            KillTimer(hwnd, 1);  // Detener el temporizador cuando se acabe el tiempo
        }
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

// Función para mover el tanque mostrando la trayectoria
void MoveTankWithTrajectory(Tank* tank, int targetX, int targetY, HWND hwnd) {
    int startX = tank->x;
    int startY = tank->y;
    std::wstring trajectoryText = L"Trayectoria: ";

    // Limpiar la posición anterior
    ClearOldTankPosition(tank, hwnd);

    // Calcular los pasos de movimiento
    int stepX = (targetX > startX) ? 1 : ((targetX < startX) ? -1 : 0);
    int stepY = (targetY > startY) ? 1 : ((targetY < startY) ? -1 : 0);

    // Recorrer la trayectoria y actualizar la interfaz paso a paso
    while (startX != targetX || startY != targetY) {
        if (startX != targetX) {
            startX += stepX;
        }
        if (startY != targetY) {
            startY += stepY;
        }

        // Mostrar la "T" en cada paso del trayecto y actualizar la interfaz
        HWND hButton = GetDlgItem(hwnd, startX * N + startY);
        if (hButton) {
            SetWindowText(hButton, L"T");
            trajectoryText += L"(" + std::to_wstring(startX) + L"," + std::to_wstring(startY) + L") ";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));  // Pausa de 0.5 segundos
            SetWindowText(hButton, L"O");  // Restaurar la celda después de pasar
        }
    }

    // Actualizar la nueva posición del tanque
    tank->x = targetX;
    tank->y = targetY;

    // Actualizar el botón en la nueva posición
    UpdateTankButton(tank, hwnd);

    // Mostrar la trayectoria en el cuadro de texto
    SetWindowText(hRouteText, trajectoryText.c_str());
}

// Limpiar la posición anterior del tanque
void ClearOldTankPosition(Tank* tank, HWND hwnd) {
    HWND hOldButton = GetDlgItem(hwnd, tank->x * N + tank->y);
    if (hOldButton) {
        SetWindowText(hOldButton, L"O");  // Restaurar la celda anterior a vacía
        InvalidateRect(hOldButton, NULL, TRUE);  // Forzar el refresco visual
        UpdateWindow(hOldButton);  // Forzar la actualización inmediata
    }
}

// Actualizar el botón del tanque en la nueva posición
void UpdateTankButton(Tank* tank, HWND hwnd) {
    HWND hButton = GetDlgItem(hwnd, tank->x * N + tank->y);
    if (hButton) {
        SetWindowText(hButton, tank->tankText.c_str());  // Mostrar el tanque en la nueva posición
        InvalidateRect(hButton, NULL, TRUE);  // Forzar el refresco visual
        UpdateWindow(hButton);  // Forzar la actualización inmediata
    }
}



// Función para actualizar la vida de los tanques en la interfaz
void UpdateTankHealth() {
    // Asumiendo que la vida de los tanques está almacenada en cada objeto de tanque
    std::wstring healthText;

    // Actualizar la vida del tanque celeste
    healthText = L"Vida Celeste: " + std::to_wstring(tanks[0]->health);
    SetWindowText(hTankHealth[0], healthText.c_str());

    // Actualizar la vida del tanque amarillo
    healthText = L"Vida Amarillo: " + std::to_wstring(tanks[1]->health);
    SetWindowText(hTankHealth[1], healthText.c_str());

    // Actualizar la vida del tanque rojo
    healthText = L"Vida Rojo: " + std::to_wstring(tanks[2]->health);
    SetWindowText(hTankHealth[2], healthText.c_str());

    // Actualizar la vida del tanque azul
    healthText = L"Vida Azul: " + std::to_wstring(tanks[3]->health);
    SetWindowText(hTankHealth[3], healthText.c_str());
}

void UpdateTimer(HWND hwnd) {
    --remainingTime;  // Decrementar tiempo restante

    // Actualizar cuadro de texto del temporizador
    wchar_t buffer[50];
    swprintf(buffer, 50, L"Tiempo restante: %d:%02d", remainingTime / 60, remainingTime % 60);
    SetWindowText(hTimerText, buffer);  // Actualizamos el nuevo cuadro de texto

    if (remainingTime <= 0) {
        // Condición de victoria por tiempo
        int totalHealthP2 = tanks[0]->getHealth() + tanks[1]->getHealth();
        int totalHealthP1 = tanks[2]->getHealth() + tanks[3]->getHealth();

        if (totalHealthP1 > totalHealthP2) {
            MessageBox(hwnd, L"Jugador 1 gana por salud restante", L"Victoria por tiempo", MB_OK);
        }
        else if (totalHealthP2 > totalHealthP1) {
            MessageBox(hwnd, L"Jugador 2 gana por salud restante", L"Victoria por tiempo", MB_OK);
        }
        else {
            MessageBox(hwnd, L"Empate", L"Resultado", MB_OK);
        }

        PostQuitMessage(0);  
    }

    // Verificar victoria por eliminación
    int tanksAliveP2 = (tanks[0]->getHealth() > 0) + (tanks[1]->getHealth() > 0);
    int tanksAliveP1 = (tanks[2]->getHealth() > 0) + (tanks[3]->getHealth() > 0);

    if (tanksAliveP1 == 0) {
        MessageBox(hwnd, L"Jugador 2 gana por eliminacion", L"Victoria por eliminacion", MB_OK);
        PostQuitMessage(0);  
    }
    else if (tanksAliveP2 == 0) {
        MessageBox(hwnd, L"Jugador 1 gana por eliminacion", L"Victoria por eliminacion", MB_OK);
        PostQuitMessage(0);  
    }
}
