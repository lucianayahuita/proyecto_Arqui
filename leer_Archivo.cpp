#include "leer_Archivo.h"
#include "crear_archivo.h"
#include "configuration_filesystem.h"
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

bool leerArchivo() {
    string nombreArchivo;
    cout << "Ingrese el nombre del archivo a leer: ";
    getline(cin, nombreArchivo);

    if (nombreArchivo.length() >= 32) {
        cout << "Nombre demasiado largo.\n";
        return false;
    }

    int fat[TOTAL_BLOCKS];
    EntradaArchivo directorio[(BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)];

    if (!cargarFATyDirectorio(fat, directorio)) {
        cerr << "Error al cargar FAT y directorio.\n";
        return false;
    }

    // Debug: Mostrar archivos en el directorio
    cout << "DEBUG - Archivos en directorio:\n";
    for (int i = 0; i < (BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo); i++) {
        if (directorio[i].activo) {
            cout << "Archivo " << i << ": '" << directorio[i].nombre 
                 << "' (tamaño: " << directorio[i].tamanio 
                 << ", bloque: " << directorio[i].bloqueInicial << ")\n";
        }
    }

    // Buscar archivo - CORREGIDO: usar strcmp en lugar de ==
    int indice = -1;
    for (int i = 0; i < (BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo); i++) {
        if (directorio[i].activo && strcmp(directorio[i].nombre, nombreArchivo.c_str()) == 0) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        cout << "Archivo '" << nombreArchivo << "' no encontrado.\n";
        return false;
    }

    EntradaArchivo& archivo = directorio[indice];
    cout << "DEBUG - Archivo encontrado: '" << archivo.nombre 
         << "', tamaño: " << archivo.tamanio 
         << ", bloque inicial: " << archivo.bloqueInicial << "\n";

    int bloqueActual = archivo.bloqueInicial;
    int bytesRestantes = archivo.tamanio;

    ifstream disco(DISK_FILENAME, ios::binary);
    if (!disco) {
        cerr << "Error al abrir el disco.\n";
        return false;
    }

    string contenido;
    contenido.reserve(bytesRestantes);

    cout << "DEBUG - Leyendo cadena de bloques...\n";
    
    while (bloqueActual != -3 && bloqueActual >= DATA_BLOCK_START && bytesRestantes > 0) {
        cout << "DEBUG - Leyendo bloque " << bloqueActual << ", bytes restantes: " << bytesRestantes << "\n";
        
        int pos = bloqueActual * BLOCK_SIZE;
        disco.seekg(pos, ios::beg);

        if (!disco.good()) {
            cerr << "Error al posicionarse en bloque " << bloqueActual << "\n";
            return false;
        }

        int bytesALeer = min(BLOCK_SIZE, bytesRestantes);
        char buffer[BLOCK_SIZE];
        memset(buffer, 0, BLOCK_SIZE);
        
        disco.read(buffer, bytesALeer);
        
        if (!disco.good() && !disco.eof()) {
            cerr << "Error al leer bloque " << bloqueActual << "\n";
            return false;
        }

        int bytesLeidos = disco.gcount();
        cout << "DEBUG - Bytes leídos del bloque: " << bytesLeidos << "\n";
        cout << "DEBUG - Contenido raw del bloque: '";
        for (int j = 0; j < min(bytesLeidos, 50); j++) {
            if (buffer[j] >= 32 && buffer[j] <= 126) {
                cout << buffer[j];
            } else {
                cout << "[" << (int)(unsigned char)buffer[j] << "]";
            }
        }
        cout << "'\n";
        
        if (bytesLeidos > 0) {
            int bytesAUsar = min(bytesLeidos, bytesRestantes);
            contenido.append(buffer, bytesAUsar);
            bytesRestantes -= bytesAUsar;
        }
        
        // Obtener siguiente bloque de la FAT
        int siguienteBloque = fat[bloqueActual];
        cout << "DEBUG - Siguiente bloque en FAT[" << bloqueActual << "] = " << siguienteBloque << "\n";
        
        bloqueActual = siguienteBloque;
    }

    cout << "\n=== CONTENIDO DEL ARCHIVO ===\n";
    if (contenido.empty()) {
        cout << "(El archivo está vacío)\n";
    } else {
        cout << contenido << "\n";
    }
    cout << "=== FIN DEL CONTENIDO ===\n";
    cout << "Total de caracteres leídos: " << contenido.length() << "\n";
    
    return true;
}