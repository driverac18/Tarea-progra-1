#include <iostream>
#include <vector>
#include <windows.h>
#include <ctime>
#include <cstdlib>

using namespace std;



// Esta función mueve el cursor de la consola a una coordenada específica (x, y)
// para poder dibujar el tablero sin limpiar la pantalla (evita el parpadeo).
void goToXy(int pos_x, int pos_y) {
    HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos;
    pos.X = pos_x;
    pos.Y = pos_y;
    SetConsoleCursorPosition(h_console, pos);
}

// CLASE COORDENADA
class Coordenada {
public:
    int pos_x, pos_y;

    // Constructor que inicializa las coordenadas (por defecto en 0)
    Coordenada(int pos_x = 0, int pos_y = 0) {
        this->pos_x = pos_x;
        this->pos_y = pos_y;
    }
};

// =======================
// CLASE TABLERO
// =======================
class Tablero {
private:
    int num_filas, num_columnas;
    vector<vector<int>> matriz_grid;

public:
    // Constructor: redimensiona la matriz al tamaño especificado y la llena de 0s (células muertas)
    Tablero(int cant_filas, int cant_columnas) : num_filas(cant_filas), num_columnas(cant_columnas) {
        matriz_grid.resize(num_filas, vector<int>(num_columnas, 0));
    }
    // Llena el tablero de forma aleatoria con 0s (muerta) y 1s (viva)
    void inicializarRandom() {
        srand(time(0));
        for (int i = 0; i < num_filas; i++) {
            for (int j = 0; j < num_columnas; j++) {
                matriz_grid[i][j] = rand() % 2;
            }
        }
    }

    // Obtiene el valor de una celda aplicando la lógica de "Tablero Toroidal".
    // Esto significa que si la coordenada sale del límite derecho, aparece por el izquierdo (como en Pac-Man).
    int getCelda(int pos_x, int pos_y) {
        pos_x = (pos_x + num_filas) % num_filas;
        pos_y = (pos_y + num_columnas) % num_columnas;
        return matriz_grid[pos_x][pos_y];
    }

    // Actualiza el estado de una celda específica
    void setCelda(int pos_x, int pos_y, int valor_celda) {
        matriz_grid[pos_x][pos_y] = valor_celda;
    }

    int getFilas() { return num_filas; }
    int getColumnas() { return num_columnas; }

    // Dibuja el estado actual del tablero en la consola
    void dibujar() {
        for (int i = 0; i < num_filas; i++) {
            for (int j = 0; j < num_columnas; j++) {
                goToXy(j, i); // Usamos (j, i) porque 'j' son las columnas (eje X) e 'i' las filas (eje Y)
                
                if (matriz_grid[i][j] == 1)
                    cout << (char)219; // Carácter ASCII para un bloque sólido (célula viva)
                else
                    cout << " ";       // Espacio en blanco (célula muerta)
            }
        }
    }
};

// =======================
// CLASE JUEGO DE LA VIDA
// =======================
class JuegoDeLaVida {
private:
    Tablero tablero_juego; 

public:
    // Constructor que inicializa el tablero con las dimensiones dadas
    JuegoDeLaVida(int cant_filas, int cant_columnas) : tablero_juego(cant_filas, cant_columnas) {}

    void inicializar() {
        tablero_juego.inicializarRandom();
    }

    // Cuenta cuántas células vivas rodean a una coordenada específica (pos_x, pos_y)
    int contarVecinos(int pos_x, int pos_y) {
        int suma_vecinos = 0; // snake_case

        // Se recorre una cuadrícula de 3x3 alrededor de la célula central
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                // Omitimos la propia célula central para no contarla como su propio vecino
                if (i == 0 && j == 0) continue; 
                
                suma_vecinos += tablero_juego.getCelda(pos_x + i, pos_y + j);
            }
        }
        return suma_vecinos;
    }

    // Aplica las reglas oficiales del Juego de la Vida de Conway para pasar a la siguiente generación
    void actualizar() {
        // Creamos una copia temporal del tablero para no afectar los cálculos de las celdas vecinas
        Tablero nuevo_tablero = tablero_juego; 

        for (int i = 0; i < tablero_juego.getFilas(); i++) {
            for (int j = 0; j < tablero_juego.getColumnas(); j++) {

                int total_vecinos = contarVecinos(i, j);
                int estado_actual = tablero_juego.getCelda(i, j);

                // REGLAS DEL JUEGO DE LA VIDA
                if (estado_actual == 1) { // Si la célula está VIVA
                    // Regla 1 y 2: Muere por soledad (< 2) o por sobrepoblación (> 3)
                    if (total_vecinos < 2 || total_vecinos > 3)
                        nuevo_tablero.setCelda(i, j, 0); 
                    // Si tiene 2 o 3, sobrevive (no se cambia nada)
                } else { // Si la célula está MUERTA
                    // Regla 3: Nace una nueva célula si tiene exactamente 3 vecinas vivas
                    if (total_vecinos == 3)
                        nuevo_tablero.setCelda(i, j, 1);
                }
            }
        }
        // Sobrescribimos el tablero antiguo con la nueva generación
        tablero_juego = nuevo_tablero;
    }

    // Bucle principal del juego
    void ejecutar() {
        while (true) {
            tablero_juego.dibujar(); // Renderiza la matriz
            actualizar();            // Calcula la siguiente generación
            Sleep(300);              // Pausa de 300 milisegundos para poder ver la animación
        }
    }
};
// =======================
// MAIN
// =======================
int main() {
    // Dimensiones de la pantalla/tablero
    int total_filas = 25;      
    int total_columnas = 40;   
    
    system("cls"); // Limpia la consola antes de iniciar
    
    // Instanciamos el juego
    JuegoDeLaVida juego_principal(total_filas, total_columnas);
    juego_principal.inicializar();
    juego_principal.ejecutar();

    return 0;
}
