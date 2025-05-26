#include "eliminar_archivo.h"
#include "configuration_filesystem.h"
#include "crear_archivo.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>

using namespace std;

extern bool cargarFATyDirectorio(int* fat, EntradaArchivo* directorio);
extern bool guardarFATyDirectorio(int* fat, EntradaArchivo* directorio);

bool eliminarArchivo() {
    string nombreArchivo;
    cout << "Ingrese el nombre del archivo a eliminar: ";
    getline(cin, nombreArchivo);
    
    int fat[TOTAL_BLOCKS];
    EntradaArchivo directorio[(BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)];
    
    if (!cargarFATyDirectorio(fat, directorio)) {
        cerr << "Error al cargar FAT y directorio.\n";
        return false;
    }
    
    // Buscar el archivo en el directorio
    int index = -1;
    for (int i = 0; i < (BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo); i++) {
        if (directorio[i].activo && strcmp(directorio[i].nombre, nombreArchivo.c_str()) == 0) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        cout << "Archivo no encontrado.\n";
        return false;
    }
    
    cout << "Eliminando archivo: " << directorio[index].nombre << "\n";
    cout << "Tamaño: " << directorio[index].tamanio << " bytes\n";
    cout << "Bloque inicial: " << directorio[index].bloqueInicial << "\n";
    

    int bloqueActual = directorio[index].bloqueInicial;
    int bloquesLiberados = 0;
    
    while (bloqueActual >= DATA_BLOCK_START && bloqueActual < TOTAL_BLOCKS) {
        int siguienteBloque = fat[bloqueActual];
       
        fat[bloqueActual] = -1;
        bloquesLiberados++;
        
        if (siguienteBloque == -3) {
            break;
        }
        
        if (siguienteBloque < DATA_BLOCK_START || siguienteBloque >= TOTAL_BLOCKS) {
            cerr << "Advertencia: Cadena de bloques corrupta en bloque " << bloqueActual << "\n";
            break;
        }
        
        bloqueActual = siguienteBloque;
    }
    
    cout << "Bloques liberados: " << bloquesLiberados << "\n";
    
    memset(&directorio[index], 0, sizeof(EntradaArchivo));
    directorio[index].activo = false;
    
    if (!guardarFATyDirectorio(fat, directorio)) {
        cerr << "Error al guardar FAT/directorio.\n";
        return false;
    }
    
    cout << "Archivo '" << nombreArchivo << "' eliminado correctamente.\n";
    return true;
}