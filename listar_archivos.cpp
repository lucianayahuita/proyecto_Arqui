#include "listar_archivos.h"
#include "leer_Archivo.h"
#include "crear_archivo.h"
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;
bool listarArchivos(){
    int fat[TOTAL_BLOCKS];
    EntradaArchivo directorio[(BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)];

    if (!cargarFATyDirectorio(fat, directorio)) {
        cerr << "Error al cargar FAT y directorio.\n";
        return false;
    }

    cout << "Archivos en directorio:\n";
    for (int i = 0; i < (BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo); i++) {
        if (directorio[i].activo) {
            time_t timestamp = directorio[i].fechaCreacion;
            struct tm* fecha = localtime(&timestamp);
            char bufferFecha[20];
            strftime(bufferFecha, sizeof(bufferFecha), "%d-%m-%Y", fecha);

            cout << "Archivo " << i << ": '" << directorio[i].nombre 
                 << "' (tamaño: " << directorio[i].tamanio 
                 << ", bloque: " << directorio[i].bloqueInicial 
                 << ", fecha de creacion: " << bufferFecha << ")\n";
        }
    }

    return true;
}
