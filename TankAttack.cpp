#include <windows.h>
#include <cstdlib>  // Para srand y rand
#include <ctime>    // Para time()

// Tamaño del grafo (4x4)
const int N = 4;

// Definir la matriz del grafo ('O' para celdas vacías, 'X' para obstáculos)
char graph[N][N];

// Prototipos
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

// Generar obstáculos aleatorios en el grafo
void GenerateGraph() {
    srand(time(0));  // Inicializar generador de números aleatorios
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (rand() % 5 == 0) {
                graph[i][j] = 'X';  // Obstáculo
            }
            else {
                graph[i][j] = 'O';  // Celda vacía
            }
        }
    }
}

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

    GenerateGraph();  // Generar el grafo

    CreateWindow(L"TankAttackWindow", L"Tank Attack Game", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 500, NULL, NULL, hInst, NULL);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, NULL, NULL)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
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
                    WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                    50 + j * 80, 50 + i * 80, 70, 70,
                    hwnd, (HMENU)(i * N + j),
                    (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                    NULL
                );

                // Cambiar el texto del botón dependiendo de si es obstáculo o celda vacía
                if (graph[i][j] == 'X') {
                    SetWindowText(hButton, L"X");
                }
                else {
                    SetWindowText(hButton, L"O");
                }
            }
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
