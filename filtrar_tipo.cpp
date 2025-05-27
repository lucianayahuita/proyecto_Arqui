#include "filtrar_tipo.h"
#include "configuration_filesystem.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <ctime>

using namespace std;

struct ArchivoInfo {
    string nombre;
    int tamanio;
    int bloqueInicial;
    time_t fechaCreacion;
};

string obtenerExtension(const string& nombreArchivo) {
    size_t posUltimoPunto = nombreArchivo.find_last_of('.');
    if (posUltimoPunto == string::npos || posUltimoPunto == nombreArchivo.length() - 1) {
        return ""; 
    }
    return nombreArchivo.substr(posUltimoPunto + 1);
}

// Función para validar si la extensión es válida
bool extensionValida(const string& extension) {
    if (extension.empty()) return false;
    
    // Verificar que no contenga caracteres especiales
    for (char c : extension) {
        if (!isalnum(c)) return false;
    }
    return true;
}

// Función para cargar el directorio desde disco
bool cargarDirectorio(EntradaArchivo* directorio) {
    ifstream disco(DISK_FILENAME, ios::binary);
    if (!disco) return false;
    // Saltar la FAT
    disco.seekg(sizeof(int) * TOTAL_BLOCKS, ios::beg);
    // Leer el directorio
    disco.read(reinterpret_cast<char*>(directorio), 
               sizeof(EntradaArchivo) * ((BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)));
    
    return disco.good();
}

// Función para convertir time_t a string legible
string formatearFecha(time_t tiempo) {
    char buffer[100];
    struct tm* timeinfo = localtime(&tiempo);
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", timeinfo);
    return string(buffer);
}

// Función auxiliar para mostrar la información de un archivo
void mostrarInfoArchivo(const ArchivoInfo& archivo, int indice) {
    cout << "[" << (indice + 1) << "] " << archivo.nombre << endl;
    cout << "    Tamaño: " << archivo.tamanio << " bytes" << endl;
    cout << "    Bloque inicial: " << archivo.bloqueInicial << endl;
    cout << "    Fecha de creación: " << formatearFecha(archivo.fechaCreacion) << endl;
    cout << "    ----------------------------------------" << endl;
}

// Función para listar archivos por extensión específica
bool listarArchivosPorExtension(const string& extension) {
    // Validar entrada
    if (!extensionValida(extension)) {
        cout << "Extensión no válida. Use solo caracteres alfanuméricos." << endl;
        return false;
    }
    
    EntradaArchivo directorio[(BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)];
    
    if (!cargarDirectorio(directorio)) {
        cerr << "Error al cargar el directorio desde el disco." << endl;
        return false;
    }
    
    vector<ArchivoInfo> archivosEncontrados;
    int totalEntradas = (BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo);
    
    // Convertir extensión a minúsculas para comparación case-insensitive
    string extensionLower = extension;
    for (char& c : extensionLower) {
        c = tolower(c);
    }
    
    // Buscar archivos con la extensión especificada
    for (int i = 0; i < totalEntradas; i++) {
        if (directorio[i].activo) {
            string nombreArchivo = directorio[i].nombre;
            string extArchivo = obtenerExtension(nombreArchivo);
            
            // Convertir a minúsculas para comparación
            for (char& c : extArchivo) {
                c = tolower(c);
            }
            
            if (extArchivo == extensionLower) {
                ArchivoInfo info;
                info.nombre = nombreArchivo;
                info.tamanio = directorio[i].tamanio;
                info.bloqueInicial = directorio[i].bloqueInicial;
                info.fechaCreacion = directorio[i].fechaCreacion;
                
                archivosEncontrados.push_back(info);
            }
        }
    }
    cout << "\n========== ARCHIVOS CON EXTENSIÓN ." << extension << " ==========" << endl;
    if (archivosEncontrados.empty()) {
        cout << "No se encontraron archivos con la extensión '." << extension << "'." << endl;
        return true;
    }
    cout << "Se encontraron " << archivosEncontrados.size() << " archivo(s):" << endl;
    cout << "================================================" << endl;
    for (size_t i = 0; i < archivosEncontrados.size(); i++) {
        mostrarInfoArchivo(archivosEncontrados[i], i);
    }
    int tamanioTotal = 0;
    for (const auto& archivo : archivosEncontrados) {
        tamanioTotal += archivo.tamanio;
    }
    cout << "\nESTADÍSTICAS:" << endl;
    cout << "Total de archivos: " << archivosEncontrados.size() << endl;
    cout << "Espacio total usado: " << tamanioTotal << " bytes" << endl;
    cout << "================================================" << endl;
    
    return true;
}
// Función principal que será llamada desde el menú principal
void filtroTipo() {
    string extension;
    
    cout << "\n=== FILTRAR ARCHIVOS POR TIPO (EXTENSIÓN) ===" << endl;
    cout << "Ingrese la extensión a buscar (sin el punto): ";
    getline(cin, extension);
    
    if (extension.empty()) {
        cout << "Error: Debe ingresar una extensión." << endl;
        return;
    }
    
    listarArchivosPorExtension(extension);
}