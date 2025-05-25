#ifndef CONFIGURATION_FILESYSTEM_H
#define CONFIGURATION_FILESYSTEM_H

#include <ctime>

// Configuración general del sistema
extern const int BLOCK_SIZE;           // Tamaño de bloque en bytes
extern const int TOTAL_BLOCKS;          // Número total de bloques en el "disco"
extern const int DIRECTORY_BLOCKS;      // Bloques reservados para el directorio
extern const int FAT_BLOCKS;            // Bloques reservados para la FAT
extern const int DATA_BLOCK_START;      // Primer bloque de datos
extern const char* DISK_FILENAME;

// Estructura para la entrada del directorio (metadata de archivos)
struct EntradaArchivo {
    char nombre[32];     // Nombre del archivo
    int tamanio;         // Tamaño en bytes
    int bloqueInicial;   // Primer bloque de datos
    time_t fechaCreacion;
    bool activo;         // Indica si el archivo está en uso
};

#endif
