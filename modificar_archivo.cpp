#include "modificar_archivo.h"
#include "configuration_filesystem.h"
#include "crear_archivo.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>
#include <vector>
#include <algorithm>

using namespace std;

// Declaración externa para usar funciones de crear_archivo.cpp
extern bool cargarFATyDirectorio(int* fat, EntradaArchivo* directorio);
extern bool guardarFATyDirectorio(int* fat, EntradaArchivo* directorio);
extern vector<int> asignarBloques(int* fat, int bloquesNecesarios);

// Función auxiliar para liberar bloques de un archivo
void liberarBloques(int* fat, int bloqueInicial) {
    int bloqueActual = bloqueInicial;
    
    while (bloqueActual >= DATA_BLOCK_START && bloqueActual < TOTAL_BLOCKS) {
        int siguienteBloque = fat[bloqueActual];
        fat[bloqueActual] = -1; // Marcar como libre
        
        if (siguienteBloque == -3) { // Fin de cadena
            break;
        }
        
        if (siguienteBloque < DATA_BLOCK_START || siguienteBloque >= TOTAL_BLOCKS) {
            break; // Evitar corrupción
        }
        
        bloqueActual = siguienteBloque;
    }
}

// Función auxiliar para obtener el contenido actual de un archivo
string obtenerContenidoArchivo(int bloqueInicial, int tamanio) {
    ifstream disco(DISK_FILENAME, ios::binary);
    if (!disco) return "";
    
    string contenido;
    contenido.reserve(tamanio);
    
    int bloqueActual = bloqueInicial;
    int bytesLeidos = 0;
    
    while (bloqueActual >= DATA_BLOCK_START && bloqueActual < TOTAL_BLOCKS && bytesLeidos < tamanio) {
        disco.seekg(bloqueActual * BLOCK_SIZE, ios::beg);
        
        char buffer[BLOCK_SIZE];
        disco.read(buffer, BLOCK_SIZE);
        
        int bytesALeer = min(BLOCK_SIZE, tamanio - bytesLeidos);
        contenido.append(buffer, bytesALeer);
        bytesLeidos += bytesALeer;
        
        // Obtener siguiente bloque de la FAT
        int fat[TOTAL_BLOCKS];
        ifstream discoFAT(DISK_FILENAME, ios::binary);
        discoFAT.read(reinterpret_cast<char*>(fat), sizeof(int) * TOTAL_BLOCKS);
        discoFAT.close();
        
        if (fat[bloqueActual] == -3) break; // Fin de cadena
        bloqueActual = fat[bloqueActual];
    }
    
    disco.close();
    return contenido;
}

bool modificarArchivo() {
    string nombreArchivo;
    cout << "Ingrese el nombre del archivo a modificar: ";
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
    
    // Mostrar información del archivo actual
    cout << "\n--- Información del archivo ---\n";
    cout << "Nombre: " << directorio[index].nombre << "\n";
    cout << "Tamaño actual: " << directorio[index].tamanio << " bytes\n";
    cout << "Bloque inicial: " << directorio[index].bloqueInicial << "\n";
    
    // Obtener y mostrar contenido actual
    string contenidoActual = obtenerContenidoArchivo(directorio[index].bloqueInicial, directorio[index].tamanio);
    cout << "\nContenido actual:\n";
    cout << "\"" << contenidoActual << "\"\n\n";
    
    // Preguntar qué tipo de modificación hacer
    cout << "Seleccione el tipo de modificación:\n";
    cout << "1. Reemplazar todo el contenido\n";
    cout << "2. Agregar al final del archivo\n";
    cout << "3. Insertar al inicio del archivo\n";
    cout << "Opción: ";
    
    int opcion;
    cin >> opcion;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    string nuevoContenido;
    cout << "Ingrese el nuevo contenido: ";
    getline(cin, nuevoContenido);
    
    string contenidoFinal;
    
    switch (opcion) {
        case 1: // Reemplazar
            contenidoFinal = nuevoContenido;
            break;
        case 2: // Agregar al final
            contenidoFinal = contenidoActual + nuevoContenido;
            break;
        case 3: // Insertar al inicio
            contenidoFinal = nuevoContenido + contenidoActual;
            break;
        default:
            cout << "Opción inválida.\n";
            return false;
    }
    
    // Calcular bloques necesarios para el nuevo contenido
    int nuevoTamanio = (int)contenidoFinal.size();
    int bloquesNecesarios = (nuevoTamanio + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (bloquesNecesarios == 0) {
        bloquesNecesarios = 1; // Al menos un bloque
    }
    
    // Liberar bloques actuales del archivo
    liberarBloques(fat, directorio[index].bloqueInicial);
    
    // Asignar nuevos bloques
    vector<int> bloquesAsignados = asignarBloques(fat, bloquesNecesarios);
    if (bloquesAsignados.empty()) {
        cout << "No hay espacio suficiente en el disco para la modificación.\n";
        return false;
    }
    
    // Enlazar bloques en la FAT
    for (int i = 0; i < bloquesNecesarios - 1; i++) {
        fat[bloquesAsignados[i]] = bloquesAsignados[i + 1];
    }
    fat[bloquesAsignados.back()] = -3; // Fin de cadena
    
    // Escribir nuevo contenido en los bloques
    fstream disco(DISK_FILENAME, ios::binary | ios::in | ios::out);
    if (!disco) {
        cerr << "Error al abrir disco para escritura.\n";
        return false;
    }
    
    for (int i = 0; i < bloquesNecesarios; i++) {
        int pos = bloquesAsignados[i] * BLOCK_SIZE;
        disco.seekp(pos, ios::beg);
        
        int offset = i * BLOCK_SIZE;
        int bytesAEscribir = min(BLOCK_SIZE, nuevoTamanio - offset);
        
        char buffer[BLOCK_SIZE];
        memset(buffer, 0, BLOCK_SIZE); // Inicializar con ceros
        
        if (bytesAEscribir > 0) {
            memcpy(buffer, contenidoFinal.data() + offset, bytesAEscribir);
        }
        
        disco.write(buffer, BLOCK_SIZE);
        
        if (!disco.good()) {
            cerr << "Error al escribir bloque " << bloquesAsignados[i] << "\n";
            disco.close();
            return false;
        }
    }
    
    disco.close();
    
    // Actualizar entrada del directorio
    directorio[index].tamanio = nuevoTamanio;
    directorio[index].bloqueInicial = bloquesAsignados[0];
    directorio[index].fechaCreacion = time(nullptr); // Actualizar fecha de modificación
    
    // Guardar cambios en FAT y directorio
    if (!guardarFATyDirectorio(fat, directorio)) {
        cerr << "Error al guardar FAT/directorio.\n";
        return false;
    }
    
    cout << "\n--- Archivo modificado correctamente ---\n";
    cout << "Nombre: " << directorio[index].nombre << "\n";
    cout << "Nuevo tamaño: " << nuevoTamanio << " bytes\n";
    cout << "Nuevo bloque inicial: " << bloquesAsignados[0] << "\n";
    cout << "Bloques utilizados: " << bloquesNecesarios << "\n";
    
    return true;
}