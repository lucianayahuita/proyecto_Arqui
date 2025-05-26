
#include "configuration_filesystem.h"
#include "crear_archivo.h" 
#include "leer_archivo.h" 
#include "listar_archivos.h"
#include "modificar_archivo.h"
#include "eliminar_archivo.h"
#include <iostream>
using namespace std;
void formatearDisco();
void cargarDisco();

int main() {
    std::cout << "Iniciando sistema de archivos...\n";
    cargarDisco();
    
    int opcion;
    do {
        cout << "\n  MENU DEL SISTEMA DE ARCHIVOS\n";
        cout << "1. Crear archivo\n";
        cout << "2. Leer archivo por nombre\n";
        cout << "3. Listar archivos\n";
        cout << "4. Modificar archivo por nombre\n";
        cout << "5. Eliminar archivo por nombre\n";
        cout << "0. Salir\n";
        cout << "Ingrese una opción: ";
        cin >> opcion;
        cin.ignore();  // limpiar el buffer de entrada

        switch (opcion) {
            case 1:
                crearArchivo(); 
                break;
            case 2:
                leerArchivo();
                break;
            case 3:
                listarArchivos();
                break;
            case 4:
                modificarArchivo();
                break;
            case 5:
                eliminarArchivo();
                break;

            case 0:
                std::cout << " Saliendo...\n";
                break;
            default:
                std::cout << "Opción no válida\n";
        }

    } while (opcion != 0);
    return 0;
}
