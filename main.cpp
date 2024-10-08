#include <windows.h>
#include "Grafo.h"
#include "Tank.h"
#include <vector>
#include <string>

const int N = 4;  // Tamaño del grafo
Grafo* grafo;
std::vector<Tank*> tanks;  // Almacenamos los punteros a los tanques
bool isPlayer1Turn = true;  // Para manejar el turno
int remainingTime = 300;  // Tiempo restante en segundos (5 minutos)

// Prototipos
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void UpdateTimer(HWND hwnd);

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

    HWND hwnd = CreateWindow(L"TankAttackWindow", L"Tank Attack Game", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 500, NULL, NULL, hInst, NULL);

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
    switch (msg) {
    case WM_CREATE: {
        // Crear botones para representar el grafo
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                HWND hButton = CreateWindow(
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

        // Crear tanques y colocar en el grafo
        tanks.push_back(new CelesteTank(*grafo, hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE)));
        tanks.push_back(new AmarilloTank(*grafo, hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE)));
        tanks.push_back(new RojoTank(*grafo, hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE)));
        tanks.push_back(new AzulTank(*grafo, hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE)));

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

    case WM_COMMAND: {
        // Manejar el clic en los botones del grafo (para mover tanques)
        int buttonId = LOWORD(wp);
        HWND hButton = (HWND)lp;

        // Identificar el turno y los tanques que puede mover cada jugador
        if (isPlayer1Turn) {
            MessageBox(hwnd, L"Es el turno del Jugador 1 (tanques rojo y azul)", L"Turno", MB_OK);
        }
        else {
            MessageBox(hwnd, L"Es el turno del Jugador 2 (tanques celeste y amarillo)", L"Turno", MB_OK);
        }

        // Alternar turnos
        isPlayer1Turn = !isPlayer1Turn;
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

// Actualizar el temporizador en la ventana
void UpdateTimer(HWND hwnd) {
    std::wstring timerText = L"Tiempo restante: " + std::to_wstring(remainingTime / 60) + L":" + std::to_wstring(remainingTime % 60);
    SetWindowText(hwnd, timerText.c_str());
}
