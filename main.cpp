#define NOMINMAX  // Para evitar el conflicto con las macros max y min de windows.h
#include <windows.h>
#include "Grafo.h"
#include "Tank.h"
#include "Mov.h"
#include <vector>
#include <ctime>  // Para srand y rand

const int N = 4;  // Tamaño del grafo
Grafo* grafo;
std::vector<Tank*> tanks;  // Almacenamos los punteros a los tanques
Tank* selectedTank = nullptr;  // Tanque seleccionado para algoritmo
bool isPlayer1Turn = true;  // Para manejar el turno
int remainingTime = 300;  // Tiempo restante en segundos (5 minutos)

void MoveTankRandom(Tank* tank);  // Declaración de función para movimiento aleatorio

// Prototipos
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void UpdateTimer(HWND hwnd);
void MoveTank(Tank* tank, const std::vector<int>& distances, int targetCell);
void UpdateButton(Tank* tank, HWND hwnd, int newX, int newY);  // Actualizar el botón del tanque
void ClearOldPosition(Tank* tank, HWND hwnd);  // Liberar la celda anterior del tanque

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
    static HWND hMoveButtons[4];  // Botones de movimiento para cada tanque
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
        hMoveButtons[0] = CreateWindow(L"BUTTON", L"Mov Celeste", WS_VISIBLE | WS_CHILD, 400, 50, 120, 50, hwnd, (HMENU)1001, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hMoveButtons[1] = CreateWindow(L"BUTTON", L"Mov Amarillo", WS_VISIBLE | WS_CHILD, 400, 110, 120, 50, hwnd, (HMENU)1002, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hMoveButtons[2] = CreateWindow(L"BUTTON", L"Mov Rojo", WS_VISIBLE | WS_CHILD, 400, 170, 120, 50, hwnd, (HMENU)1003, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        hMoveButtons[3] = CreateWindow(L"BUTTON", L"Mov Azul", WS_VISIBLE | WS_CHILD, 400, 230, 120, 50, hwnd, (HMENU)1004, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

        // Crear tanques y colocar en el grafo
        tanks.push_back(new CelesteTank(*grafo, hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE)));
        tanks.push_back(new AmarilloTank(*grafo, hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE)));
        tanks.push_back(new RojoTank(*grafo, hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE)));
        tanks.push_back(new AzulTank(*grafo, hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE)));

        break;
    }

    case WM_COMMAND: {
        int id = LOWORD(wp);

        // Manejar los botones de movimiento
        if (id >= 1001 && id <= 1004) {
            Tank* tankToMove = tanks[id - 1001];  // Selecciona el tanque correspondiente al botón

            // Verificar el turno del jugador
            if ((isPlayer1Turn && (id == 1003 || id == 1004)) || (!isPlayer1Turn && (id == 1001 || id == 1002))) {
                int randomNum = rand() % 100;

                if (id == 1001 || id == 1004) {
                    // Celeste/Azul: 50% BFS, 50% movimiento aleatorio
                    if (randomNum < 50) {
                        selectedTank = tankToMove;
                        MessageBox(hwnd, L"Selecciona una celda para mover el tanque usando BFS", L"BFS", MB_OK);
                    }
                    else {
                        ClearOldPosition(tankToMove, hwnd);  // Limpiar posición actual
                        MoveTankRandom(tankToMove);
                        UpdateButton(tankToMove, hwnd, tankToMove->x, tankToMove->y);  // Actualizar el botón después del movimiento
                        isPlayer1Turn = !isPlayer1Turn;  // Cambiar turno después del movimiento
                    }
                }
                else if (id == 1002 || id == 1003) {
                    // Amarillo/Rojo: 80% Dijkstra, 20% movimiento aleatorio
                    if (randomNum < 80) {
                        selectedTank = tankToMove;
                        MessageBox(hwnd, L"Selecciona una celda para mover el tanque usando Dijkstra", L"Dijkstra", MB_OK);
                    }
                    else {
                        ClearOldPosition(tankToMove, hwnd);  // Limpiar posición actual
                        MoveTankRandom(tankToMove);
                        UpdateButton(tankToMove, hwnd, tankToMove->x, tankToMove->y);  // Actualizar el botón después del movimiento
                        isPlayer1Turn = !isPlayer1Turn;  // Cambiar turno después del movimiento
                    }
                }
            }
            else {
                MessageBox(hwnd, L"No es tu turno", L"Error", MB_OK);
            }
        }

        // Seleccionar una celda del grafo para mover el tanque con un algoritmo
        if (selectedTank != nullptr && id < 1000) {
            std::vector<int> distances;

            if (selectedTank->tankText == L"TCE" || selectedTank->tankText == L"TAZ") {
                distances = BFS(*grafo, selectedTank->x * N + selectedTank->y);
            }
            else if (selectedTank->tankText == L"TAM" || selectedTank->tankText == L"TRO") {
                distances = Dijkstra(*grafo, selectedTank->x * N + selectedTank->y);
            }

            if (!distances.empty()) {
                ClearOldPosition(selectedTank, hwnd);  // Liberar posición actual
                MoveTank(selectedTank, distances, id);
                UpdateButton(selectedTank, hwnd, selectedTank->x, selectedTank->y);  // Actualizar el botón después del movimiento
                selectedTank = nullptr;
                isPlayer1Turn = !isPlayer1Turn;  // Cambiar turno después del movimiento
            }
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

// Función para mover el tanque de forma aleatoria
void MoveTankRandom(Tank* tank) {
    int N = grafo->N;
    std::vector<int> moves = { -1, 1, -N, N };  // Movimientos: izquierda, derecha, arriba, abajo
    int currentPos = tank->x * N + tank->y;

    // Mantener el movimiento hasta que se encuentre una celda vacía
    bool moved = false;
    while (!moved) {
        int randomMove = moves[rand() % moves.size()];
        int newPosition = currentPos + randomMove;

        // Verificar que el nuevo movimiento sea válido
        int newX = newPosition / N;
        int newY = newPosition % N;

        // Verificar límites y que la celda esté vacía
        if (newX >= 0 && newX < N && newY >= 0 && newY < N && grafo->grid[newX][newY] == 'O') {
            // Mover el tanque a la nueva posición
            tank->x = newX;
            tank->y = newY;
            moved = true;  // Movimiento válido realizado
        }
    }
}

// Función para mover el tanque basado en Dijkstra o BFS
void MoveTank(Tank* tank, const std::vector<int>& distances, int targetCell) {
    int N = grafo->N;
    if (distances[targetCell] != std::numeric_limits<int>::max()) {
        int newX = targetCell / N;
        int newY = targetCell % N;

        // Actualizar la posición del tanque en la interfaz
        tank->x = newX;
        tank->y = newY;
    }
}

// Función para actualizar el botón del tanque después del movimiento
void UpdateButton(Tank* tank, HWND hwnd, int newX, int newY) {
    HWND hButton = GetDlgItem(hwnd, newX * grafo->N + newY);  // Obtener el botón según la nueva posición
    SetWindowText(hButton, tank->tankText.c_str());  // Actualizar el texto del botón con el tanque
}

// Función para limpiar la celda anterior del tanque
void ClearOldPosition(Tank* tank, HWND hwnd) {
    HWND hButton = GetDlgItem(hwnd, tank->x * grafo->N + tank->y);  // Obtener el botón de la celda anterior
    SetWindowText(hButton, L"O");  // Dejar la celda vacía
}

// Actualizar el temporizador en la ventana
void UpdateTimer(HWND hwnd) {
    std::wstring timerText = L"Tiempo restante: " + std::to_wstring(remainingTime / 60) + L":" + std::to_wstring(remainingTime % 60);
    SetWindowText(hwnd, timerText.c_str());
}
