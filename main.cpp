#define NOMINMAX  // Para evitar el conflicto con las macros max y min de windows.h
#include <windows.h>
#include "Grafo.h"
#include "Tank.h"
#include "Bala.h"
#include <vector>
#include <ctime>  // Para srand y rand
#include <thread>  // Para std::this_thread::sleep_for
#include <chrono>  // Para manejar el retraso de tiempo

const int N = 4;  // Tamaño del grafo
Grafo* grafo;
std::vector<Tank*> tanks;  // Almacenamos los punteros a los tanques
Tank* selectedTank = nullptr;  // Tanque seleccionado para disparar o mover
bool isMoving = false;  // Flag para manejar si se está moviendo un tanque
bool isShooting = false;  // Flag para manejar si se está disparando
int remainingTime = 300;  // Tiempo restante en segundos (5 minutos)

HWND hRouteText;  // Cuadro de texto para mostrar la trayectoria de la bala
HWND hDamageText;  // Cuadro de texto para mostrar el daño de los tanques

void MoveTankWithTrajectory(Tank* tank, int targetX, int targetY, HWND hwnd);  // Nueva función para mostrar la trayectoria
void UpdateTankButton(Tank* tank, HWND hwnd);  // Declaración para actualizar el botón de los tanques
void UpdateTimer(HWND hwnd);  // Actualizar el temporizador

// Prototipos
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

    HWND hwnd = CreateWindow(L"TankAttackWindow", L"Tank Attack Game", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 600, NULL, NULL, hInst, NULL);

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

// Procedimiento de la ventana
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

                // Si es obstáculo, marcar con "X"
                if (grafo->grid[i][j] == 'X') {
                    SetWindowText(hButton, L"X");
                }
                else {
                    SetWindowText(hButton, L"O");
                }
            }
        }

        // Crear botones de movimiento para los tanques
        hMoveButtons[0] = CreateWindow(L"BUTTON", L"Mov Celeste", WS_VISIBLE | WS_CHILD, 400, 50, 120, 50, hwnd, (HMENU)2001, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hMoveButtons[1] = CreateWindow(L"BUTTON", L"Mov Amarillo", WS_VISIBLE | WS_CHILD, 400, 110, 120, 50, hwnd, (HMENU)2002, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hMoveButtons[2] = CreateWindow(L"BUTTON", L"Mov Rojo", WS_VISIBLE | WS_CHILD, 400, 170, 120, 50, hwnd, (HMENU)2003, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hMoveButtons[3] = CreateWindow(L"BUTTON", L"Mov Azul", WS_VISIBLE | WS_CHILD, 400, 230, 120, 50, hwnd, (HMENU)2004, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

        // Crear botones de disparo para los tanques
        hFireButtons[0] = CreateWindow(L"BUTTON", L"Disparar Celeste", WS_VISIBLE | WS_CHILD, 400, 300, 120, 50, hwnd, (HMENU)1001, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hFireButtons[1] = CreateWindow(L"BUTTON", L"Disparar Amarillo", WS_VISIBLE | WS_CHILD, 400, 360, 120, 50, hwnd, (HMENU)1002, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hFireButtons[2] = CreateWindow(L"BUTTON", L"Disparar Rojo", WS_VISIBLE | WS_CHILD, 400, 420, 120, 50, hwnd, (HMENU)1003, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hFireButtons[3] = CreateWindow(L"BUTTON", L"Disparar Azul", WS_VISIBLE | WS_CHILD, 400, 480, 120, 50, hwnd, (HMENU)1004, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

        // Mover los cuadros de texto fuera de la cuadrícula del grafo
        hRouteText = CreateWindow(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_READONLY, 50, 470, 320, 50, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hDamageText = CreateWindow(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_READONLY, 50, 530, 320, 50, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

        // Crear tanques y colocar en el grafo
        tanks.push_back(new CelesteTank(*grafo, hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE)));
        tanks.push_back(new AmarilloTank(*grafo, hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE)));
        tanks.push_back(new RojoTank(*grafo, hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE)));
        tanks.push_back(new AzulTank(*grafo, hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE)));

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
                MoveTankWithTrajectory(selectedTank, targetX, targetY, hwnd);  // Mostrar la trayectoria al moverse
                UpdateTankButton(selectedTank, hwnd);
                isMoving = false;  // Desactivar flag de movimiento
            }

            // Disparar la bala si el flag de disparo está activado
            if (isShooting) {
                Bala bala(selectedTank, targetX, targetY, hRouteText, hDamageText, *grafo, tanks);
                bala.moveBala();
                isShooting = false;  // Desactivar flag de disparo
            }

            selectedTank = nullptr;  // Deseleccionar el tanque después de mover o disparar
        }

        // Manejar la selección de tanque para mover
        if (id >= 2001 && id <= 2004) {
            selectedTank = tanks[id - 2001];  // Selecciona el tanque correspondiente para mover
            isMoving = true;  // Activar flag de movimiento
            MessageBox(hwnd, L"Selecciona una celda para mover el tanque", L"Movimiento", MB_OK);
        }

        // Manejar la selección de tanque para disparar
        if (id >= 1001 && id <= 1004) {
            selectedTank = tanks[id - 1001];  // Selecciona el tanque correspondiente para disparar
            isShooting = true;  // Activar flag de disparo
            MessageBox(hwnd, L"Selecciona una celda para disparar", L"Disparo", MB_OK);
        }

        break;
    }

    case WM_TIMER: {
        // Actualizar el temporizador
        if (remainingTime > 0) {
            remainingTime--;
            UpdateTimer(hwnd);
        }
        else {
            KillTimer(hwnd, 1);  // Detener el temporizador cuando se acabe el tiempo
            MessageBox(hwnd, L"¡El tiempo ha terminado!", L"Fin del juego", MB_OK);
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

// Función para mover un tanque con la trayectoria visible y actualizar el cuadro de texto
void MoveTankWithTrajectory(Tank* tank, int targetX, int targetY, HWND hwnd) {
    int startX = tank->x;
    int startY = tank->y;
    std::wstring trajectoryText = L"Trayectoria: ";

    int stepX = (targetX > startX) ? 1 : -1;
    int stepY = (targetY > startY) ? 1 : -1;

    // Moverse por la trayectoria horizontal y luego vertical
    while (startX != targetX || startY != targetY) {
        // Actualizar la posición en el trayecto
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
            std::this_thread::sleep_for(std::chrono::milliseconds(500));  // Pausa de 0.5 segundos entre cada paso
            SetWindowText(hButton, L"O");  // Restaurar la celda después de pasar
        }
    }

    // Al llegar a la posición final, actualizar las coordenadas del tanque
    tank->x = targetX;
    tank->y = targetY;

    // Actualizar el cuadro de texto con la trayectoria completa
    SetWindowText(hRouteText, trajectoryText.c_str());
}

// Función para actualizar el botón del tanque en la interfaz
void UpdateTankButton(Tank* tank, HWND hwnd) {
    HWND hButton = GetDlgItem(hwnd, tank->x * N + tank->y);
    if (hButton) {
        SetWindowText(hButton, tank->tankText.c_str());
    }
}

// Actualizar el temporizador en la ventana
void UpdateTimer(HWND hwnd) {
    std::wstring timerText = L"Tiempo restante: " + std::to_wstring(remainingTime / 60) + L":" + std::to_wstring(remainingTime % 60);
    SetWindowText(hwnd, timerText.c_str());
}
