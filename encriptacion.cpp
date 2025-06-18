#include "encriptacion.h"
#include "crear_archivo.h"  // Para cargarFATyDirectorio
#include "leer_Archivo.h"   // Para buscar archivos
#include "modificar_archivo.h" // Para guardar cambios
#include <iostream>
#include <cctype>
#include <iostream>
#include <fstream>       // Para ifstream
#include <cstring>       // Para strcmp
#include <algorithm>     // Para min
#include <cctype>        // Para isprint

using namespace std;

// Función XOR (mantenemos igual)
string xorEncryptDecrypt(const string& input, const string& key) {
    string output = input;
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = input[i] ^ key[i % key.size()];
    }
    return output;
}

bool manejarOperacionEncriptacion(bool encriptar) {
    string nombreArchivo, clave;
    
    cout << "\nIngrese el nombre del archivo a " 
         << (encriptar ? "encriptar" : "desencriptar") << ": ";
    getline(cin, nombreArchivo);

    cout << "Ingrese la clave: ";
    getline(cin, clave);

    // 1. Cargar FAT y directorio (usando crear_archivo.cpp)
    int fat[TOTAL_BLOCKS];
    EntradaArchivo directorio[(BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)];
    if (!cargarFATyDirectorio(fat, directorio)) {
        cerr << "Error al cargar FAT/directorio\n";
        return false;
    }

    // 2. Buscar archivo (como en leer_archivo.cpp)
    int indiceArchivo = -1;
    for (int i = 0; i < (BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo); i++) {
        if (directorio[i].activo && strcmp(directorio[i].nombre, nombreArchivo.c_str()) == 0) {
            indiceArchivo = i;
            break;
        }
    }

    if (indiceArchivo == -1) {
        cout << "Archivo no encontrado.\n";
        return false;
    }

    // 3. Leer contenido (simplificado)
    string contenido;
    int bloqueActual = directorio[indiceArchivo].bloqueInicial;
    int bytesRestantes = directorio[indiceArchivo].tamanio;
    
    ifstream disco(DISK_FILENAME, ios::binary);
    while (bloqueActual != -3 && bytesRestantes > 0) {
        disco.seekg(bloqueActual * BLOCK_SIZE);
        
        char buffer[BLOCK_SIZE];
        int bytesALeer = min(BLOCK_SIZE, bytesRestantes);
        disco.read(buffer, bytesALeer);
        
        contenido.append(buffer, bytesALeer);
        bytesRestantes -= bytesALeer;
        bloqueActual = fat[bloqueActual];
    }
    disco.close();

    // 4. Aplicar XOR
    string resultado = xorEncryptDecrypt(contenido, clave);

    // 5. Mostrar vista previa
    cout << "\n=== VISTA PREVIA (" << (encriptar ? "ENCRIPTADO" : "DESENCRIPTADO") << ") ===" << endl;
    string preview = resultado.substr(0, min(100, (int)resultado.size()));
    for (char c : preview) {
        if (isprint(c)) cout << c;
        else cout << "[" << (int)(unsigned char)c << "]";
    }
    if (resultado.size() > 100) cout << "...";
    cout << "\n===================================" << endl;

    // 6. Confirmar y guardar
    cout << "\n¿Desea guardar los cambios? (s/n): ";
    char opcion;
    cin >> opcion;
    cin.ignore();

    if (tolower(opcion) == 's') {
        if (!modificarArchivo(nombreArchivo, resultado)) {
            cerr << "Error al guardar cambios\n";
            return false;
        }
        cout << "¡Operación exitosa!\n";
    } else {
        cout << "Cambios descartados.\n";
    }

    return true;
}

// Menú y funciones principales (se mantienen igual)
void mostrarMenuEncriptacion() {
    cout << "\n=== MENÚ ENCRIPTACIÓN ===" << endl;
    cout << "1. Encriptar archivo" << endl;
    cout << "2. Desencriptar archivo" << endl;
    cout << "3. Volver" << endl;
    cout << "Opción: ";
}

void procesarEncriptacion() {
    manejarOperacionEncriptacion(true);
}

void procesarDesencriptacion() {
    manejarOperacionEncriptacion(false);
}