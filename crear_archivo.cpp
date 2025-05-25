#include "crear_archivo.h"
#include "configuration_filesystem.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <vector>
using namespace std;

// Función auxiliar para cargar la FAT y directorio desde disco
bool cargarFATyDirectorio(int* fat, EntradaArchivo* directorio) {
    ifstream disco(DISK_FILENAME, ios::binary);
    if (!disco) return false;

    disco.read(reinterpret_cast<char*>(fat), sizeof(int) * TOTAL_BLOCKS);
    disco.read(reinterpret_cast<char*>(directorio), sizeof(EntradaArchivo) * ((BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)));

    return disco.good();
}

// Función auxiliar para guardar FAT y directorio en disco
bool guardarFATyDirectorio(int* fat, EntradaArchivo* directorio) {
    fstream disco(DISK_FILENAME, ios::binary | ios::in | ios::out);
    if (!disco) return false;

    disco.write(reinterpret_cast<char*>(fat), sizeof(int) * TOTAL_BLOCKS);
    disco.write(reinterpret_cast<char*>(directorio), sizeof(EntradaArchivo) * ((BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)));

    return disco.good();
}

// Función para asignar bloques libres
vector<int> asignarBloques(int* fat, int bloquesNecesarios) {
    vector<int> bloquesAsignados;
    for (int i = DATA_BLOCK_START; i < TOTAL_BLOCKS && (int)bloquesAsignados.size() < bloquesNecesarios; i++) {
        if (fat[i] == -1) {
            bloquesAsignados.push_back(i);
        }
    }
    if ((int)bloquesAsignados.size() < bloquesNecesarios) {
        bloquesAsignados.clear();
    }
    return bloquesAsignados;
}

bool crearArchivo() {
    string nombreArchivo;
    string contenido;

    cout << "Ingrese el nombre del archivo: ";
    getline(cin, nombreArchivo);

    if (nombreArchivo.length() >= 32) {
        cout << "El nombre debe tener menos de 32 caracteres.\n";
        return false;
    }

    cout << "Ingrese el contenido del archivo: ";
    getline(cin, contenido);

    int fat[TOTAL_BLOCKS];
    EntradaArchivo directorio[(BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)];

    if (!cargarFATyDirectorio(fat, directorio)) {
        cerr << "Error al cargar FAT y directorio.\n";
        return false;
    }

    // Verificar si el archivo ya existe
    for (int i = 0; i < (BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo); i++) {
        if (directorio[i].activo && strcmp(directorio[i].nombre, nombreArchivo.c_str()) == 0) {
            cout << "El archivo ya existe.\n";
            return false;
        }
    }

    // Buscar espacio en directorio
    int indiceLibre = -1;
    for (int i = 0; i < (BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo); i++) {
        if (!directorio[i].activo) {
            indiceLibre = i;
            break;
        }
    }

    if (indiceLibre == -1) {
        cout << "No hay espacio libre en el directorio.\n";
        return false;
    }

    int tamanioContenido = (int)contenido.size();
    int bloquesNecesarios = (tamanioContenido + BLOCK_SIZE - 1) / BLOCK_SIZE;
    
    // Si el contenido está vacío, asignar al menos un bloque
    if (bloquesNecesarios == 0) {
        bloquesNecesarios = 1;
    }

    vector<int> bloquesAsignados = asignarBloques(fat, bloquesNecesarios);
    if (bloquesAsignados.empty()) {
        cout << "No hay espacio suficiente en el disco.\n";
        return false;
    }

    // Enlazar bloques en la FAT
    for (int i = 0; i < bloquesNecesarios - 1; i++) {
        fat[bloquesAsignados[i]] = bloquesAsignados[i + 1];
    }
    fat[bloquesAsignados.back()] = -3; // fin de cadena

    // Escribir contenido en los bloques
    fstream disco(DISK_FILENAME, ios::binary | ios::in | ios::out);
    if (!disco) {
        cerr << "Error al abrir disco.\n";
        return false;
    }

    for (int i = 0; i < bloquesNecesarios; i++) {
        int pos = bloquesAsignados[i] * BLOCK_SIZE;
        disco.seekp(pos, ios::beg);

        int offset = i * BLOCK_SIZE;
        int bytesAEscribir = min(BLOCK_SIZE, tamanioContenido - offset);

        char buffer[BLOCK_SIZE];
        memset(buffer, 0, BLOCK_SIZE); // Inicializar con ceros
        
        if (bytesAEscribir > 0) {
            memcpy(buffer, contenido.data() + offset, bytesAEscribir);
        }
        
        disco.write(buffer, BLOCK_SIZE);
        
        if (!disco.good()) {
            cerr << "Error al escribir bloque " << bloquesAsignados[i] << "\n";
            return false;
        }
    }

    EntradaArchivo& entrada = directorio[indiceLibre];
    memset(&entrada, 0, sizeof(EntradaArchivo)); 
    
    size_t nombreLen = min(nombreArchivo.length(), sizeof(entrada.nombre) - 1);
    memcpy(entrada.nombre, nombreArchivo.c_str(), nombreLen);
    entrada.nombre[nombreLen] = '\0'; 
    
    entrada.tamanio = tamanioContenido;
    entrada.bloqueInicial = bloquesAsignados[0];
    entrada.fechaCreacion = time(nullptr);
    entrada.activo = true;

    if (!guardarFATyDirectorio(fat, directorio)) {
        cerr << "Error al guardar FAT/directorio.\n";
        return false;
    }

    cout << "Archivo '" << nombreArchivo << "' creado correctamente.\n";
    cout << "Tamaño: " << tamanioContenido << " bytes\n";
    cout << "Bloque inicial: " << bloquesAsignados[0] << "\n";
    
    return true;
}