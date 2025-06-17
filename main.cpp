//PARA LINKEAR: g++ main.cpp crear_archivo.cpp leer_archivo.cpp listar_archivos.cpp eliminar_archivo.cpp modificar_archivo.cpp configuration_filesystem.cpp -o sistema_archivos2
//PARA COMENTAR: ./sistema_archivos2.exe
//PARA LINKEAR: g++ main.cpp crear_archivo.cpp leer_archivo.cpp listar_archivos.cpp eliminar_archivo.cpp modificar_archivo.cpp configuration_filesystem.cpp directorio_manager.cpp -o sistema_archivos2
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
#include <string>
#include <iostream>

using namespace std;

void formatearDisco();
void cargarDisco();

int main() {
    cout << "=== SISTEMA DE ARCHIVOS CON DIRECTORIOS ===\n";
    cargarDisco();
    inicializarSistemaDirectorios();
    cout<<"Inicie sesion"<<endl;
    registerUser();
    //Crear directorio inicial
    string nombreDirectorio;
    cout << "Ingrese el nombre del directorio de trabajo: ";
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
        // OPCIONES DE DIRECTORIO
        cout << "10. Crear directorio\n";
        cout << "11. Cambiar directorio\n";
        cout << "12. Listar directorios\n";
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
                string nombreDirectorio;
                cout << "Ingrese el nombre del nuevo directorio: ";
                getline(cin, nombreDirectorio);
                crearDirectorio(nombreDirectorio);
                break;
            }
            case 11: {
                cout << "Directorios disponibles:\n";
                listarDirectorios();
                cout << "\nIngrese el nombre del directorio (o '..' para subir, '/' para raíz): ";
                string nombreDirectorio;
                getline(cin, nombreDirectorio);
                cambiarDirectorio(nombreDirectorio);
                break;
            }
            case 12:
                listarDirectorios();
                break;
            case 0:
                cout << "Saliendo...\n";
                break;
            default:
                cout << "Opción no válida\n";
        }
        
    } while (opcion != 0);
    
    return 0;
}