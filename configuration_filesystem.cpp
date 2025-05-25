#include "configuration_filesystem.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <ctime>
// Definición de las constantes globales
const char* DISK_FILENAME = "filesystem.bin";
const int TOTAL_BLOCKS = 1024;
const int BLOCK_SIZE = 512;
const int FAT_BLOCKS = 1;
const int DIRECTORY_BLOCKS = 2;
const int DATA_BLOCK_START = FAT_BLOCKS + DIRECTORY_BLOCKS;

void formatearDisco() {
    std::ofstream disco(DISK_FILENAME, std::ios::binary | std::ios::trunc);
    if (!disco) {
        std::cerr << " Error al crear el archivo del sistema de archivos.\n";
        return;
    }

    // --------- 1. Inicializar la FAT ----------
    int fat[TOTAL_BLOCKS];
    for (int i = 0; i < TOTAL_BLOCKS; ++i) {
        fat[i] = -1; // -1 = libre
    }

    // Reservar bloques para FAT y Directorio
    for (int i = 0; i < FAT_BLOCKS + DIRECTORY_BLOCKS; ++i) {
        fat[i] = -2; // -2 = reservado
    }

    disco.write(reinterpret_cast<char*>(fat), sizeof(fat));

    // --------- 2. Inicializar el directorio ----------
    EntradaArchivo directorio[(BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)];
    for (auto& entrada : directorio) {
        entrada.activo = false;
    }

    disco.write(reinterpret_cast<char*>(directorio), sizeof(directorio));

    // --------- 3. Inicializar bloques de datos ----------
    char bloqueVacio[BLOCK_SIZE];
    memset(bloqueVacio, 0, BLOCK_SIZE);
    for (int i = DATA_BLOCK_START; i < TOTAL_BLOCKS; ++i) {
        disco.write(bloqueVacio, BLOCK_SIZE);
    }

    disco.close();
    std::cout << "Disco creado y formateado correctamente.\n";
}

void cargarDisco() {
    std::ifstream disco(DISK_FILENAME, std::ios::binary);
    if (!disco.good()) {
        std::cout << "Disco no existe. Se formateará uno nuevo...\n";
        formatearDisco();
    } else {
        std::cout << "Disco ya existe. Se cargará sin formatear.\n";
    }
}
