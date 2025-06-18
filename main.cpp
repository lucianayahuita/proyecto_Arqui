//PARA LINKEAR: g++ main.cpp crear_archivo.cpp leer_archivo.cpp listar_archivos.cpp eliminar_archivo.cpp modificar_archivo.cpp configuration_filesystem.cpp directorio_manager.cpp register.cpp encriptacion.cpp -o sistema_archivos2
//PARA EJECUTAR: ./sistema_archivos2.exe

#include "register.h"
#include "configuration_filesystem.h"
#include "crear_archivo.h"
#include "leer_archivo.h"
#include "listar_archivos.h"
#include "modificar_archivo.h"
#include "eliminar_archivo.h"
#include "ver_infoArchivo.h"
#include "filtrar_tipo.h"
#include "filtrar_fecha.h"
#include "filtrar_tamanio.h"
#include "directorio_manager.h"  
#include "wrapper_funciones.h"
#include "encriptacion.h"  // Nuevo include para encriptación
#include <string>
#include <iostream>

using namespace std;

void formatearDisco();
void cargarDisco();
void menuEncriptacion();

int main() {
    cout << "=== SISTEMA DE ARCHIVOS CON DIRECTORIOS ===\n";
    
    // PROCESO DE AUTENTICACIÓN
    cout << "Debe autenticarse para usar el sistema de archivos." << endl;
    if (!authenticateUser()) {
        cout << "No se pudo autenticar. Cerrando el sistema." << endl;
        return 1;
    }
    
    // Una vez autenticado, continuar con el sistema de archivos
    cargarDisco();
    inicializarSistemaDirectorios();
    
    // Crear directorio inicial
    string nombreDirectorio;
    cout << "\nIngrese el nombre del directorio de trabajo: ";
    getline(cin, nombreDirectorio);
    
    if (nombreDirectorio.length() >= 20) {
        cout << "Nombre demasiado largo (máximo 19 caracteres)." << endl;
        return 1;
    }
    
    if (!nombreDirectorio.empty()) {
        crearDirectorio(nombreDirectorio);
        cambiarDirectorio(nombreDirectorio);
        cout << "Directorio '" << nombreDirectorio << "' creado y establecido como directorio de trabajo.\n";
    }
    
    int opcion;
    do {
        cout << "\n===============================================\n";
        cout << "  Directorio actual: [" << obtenerDirectorioActual() << "]\n";  
        cout << "===============================================\n";
        cout << "   MENU DEL SISTEMA DE ARCHIVOS\n";
        cout << "1. Crear archivo\n";
        cout << "2. Leer archivo por nombre\n";
        cout << "3. Listar archivos\n";
        cout << "4. Modificar archivo por nombre\n";
        cout << "5. Eliminar archivo por nombre\n";
        cout << "6. Ver información archivo por nombre\n";
        cout << "7. Filtrar por extension de archivo\n";
        cout << "8. Filtrar por fecha de creación\n";
        cout << "9. Filtrar por tamaño de bytes\n";
        // NUEVAS OPCIONES DE ENCRIPTACIÓN
        cout << "10. 🔐 Sistema de Encriptación\n";
        cout << "0. Salir\n";
        cout << "Ingrese una opcion: ";
        cin >> opcion;
        cin.ignore();
        
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
            case 6:
                obtenerInformacion(); 
                break;
            case 7:
                filtroTipo();       
                break;
            case 8:
                filtroFecha();       
                break;
            case 9:
                filtroTamanio();     
                break;
            case 10: {
                menuEncriptacion();  
                break;
            }
            case 0:
                cout << "Saliendo...\n";
                break;
            default:
                cout << "Opción no válida\n";
        }
        
    } while (opcion != 0);
    
    return 0;
}

// Función para manejar el menú de encriptación
void menuEncriptacion() {
    int opcion;
    
    do {
        mostrarMenuEncriptacion();
        cin >> opcion;
        cin.ignore();
        
        switch (opcion) {
            case 1:
                procesarEncriptacion();
                break;
            case 2:
                procesarDesencriptacion();
                break;
            case 3:
                cout << "Volviendo al menú principal...\n";
                break;
            default:
                cout << "Opción no válida\n";
        }
        
    } while (opcion != 3);
}