#include "filtrar_tamanio.h"
#include "configuration_filesystem.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <climits>

using namespace std;

// Estructura para almacenar información de archivo filtrado por tamaño
struct ArchivoInfoTamanio {
    string nombre;
    int tamanio;
    int bloqueInicial;
    time_t fechaCreacion;
};

// Función estática para cargar el directorio desde disco (solo para este archivo)
static bool cargarDirectorioTamanio(EntradaArchivo* directorio) {
    ifstream disco(DISK_FILENAME, ios::binary);
    if (!disco) return false;
    
    // Saltar la FAT
    disco.seekg(sizeof(int) * TOTAL_BLOCKS, ios::beg);
    
    // Leer el directorio
    disco.read(reinterpret_cast<char*>(directorio), 
               sizeof(EntradaArchivo) * ((BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)));
    
    return disco.good();
}

// Función estática para convertir time_t a string legible (solo para este archivo)
static string formatearFechaTamanio(time_t tiempo) {
    char buffer[100];
    struct tm* timeinfo = localtime(&tiempo);
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", timeinfo);
    return string(buffer);
}

// Función para formatear el tamaño en bytes de manera legible
static string formatearTamanio(int bytes) {
    if (bytes < 1024) {
        return to_string(bytes) + " B";
    } else if (bytes < 1024 * 1024) {
        double kb = bytes / 1024.0;
        return to_string((int)kb) + " KB (" + to_string(bytes) + " bytes)";
    } else {
        double mb = bytes / (1024.0 * 1024.0);
        return to_string((int)mb) + " MB (" + to_string(bytes) + " bytes)";
    }
}

// Función para validar que el input sea un número positivo
static bool validarNumero(const string& str, int& numero) {
    if (str.empty()) return false;
    
    for (char c : str) {
        if (!isdigit(c)) return false;
    }
    
    try {
        numero = stoi(str);
        return numero >= 0;
    } catch (...) {
        return false;
    }
}

// Función estática auxiliar para mostrar la información de un archivo (solo para este archivo)
static void mostrarInfoArchivoTamanio(const ArchivoInfoTamanio& archivo, int indice) {
    cout << "[" << (indice + 1) << "] " << archivo.nombre << endl;
    cout << "    Tamaño: " << formatearTamanio(archivo.tamanio) << endl;
    cout << "    Bloque inicial: " << archivo.bloqueInicial << endl;
    cout << "    Fecha de creación: " << formatearFechaTamanio(archivo.fechaCreacion) << endl;
    cout << "    ----------------------------------------" << endl;
}

// Función para comparar archivos por tamaño (para ordenamiento)
static bool compararPorTamanio(const ArchivoInfoTamanio& a, const ArchivoInfoTamanio& b) {
    return a.tamanio > b.tamanio; // Orden descendente (mayor a menor)
}

// Función para comparar archivos por nombre (para ordenamiento)
static bool compararPorNombre(const ArchivoInfoTamanio& a, const ArchivoInfoTamanio& b) {
    return a.nombre < b.nombre; // Orden alfabético
}

// Función para listar archivos por tamaño específico
static bool listarArchivosPorTamanioEspecifico(int tamanioEspecifico) {
    EntradaArchivo directorio[(BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)];
    
    if (!cargarDirectorioTamanio(directorio)) {
        cerr << "Error al cargar el directorio desde el disco." << endl;
        return false;
    }
    
    vector<ArchivoInfoTamanio> archivosEncontrados;
    int totalEntradas = (BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo);
    
    // Buscar archivos con el tamaño especificado
    for (int i = 0; i < totalEntradas; i++) {
        if (directorio[i].activo && directorio[i].tamanio == tamanioEspecifico) {
            ArchivoInfoTamanio info;
            info.nombre = directorio[i].nombre;
            info.tamanio = directorio[i].tamanio;
            info.bloqueInicial = directorio[i].bloqueInicial;
            info.fechaCreacion = directorio[i].fechaCreacion;
            
            archivosEncontrados.push_back(info);
        }
    }
    
    // Mostrar resultados
    cout << "\n========== ARCHIVOS CON TAMAÑO DE " << formatearTamanio(tamanioEspecifico) << " ==========" << endl;
    
    if (archivosEncontrados.empty()) {
        cout << "No se encontraron archivos con el tamaño especificado." << endl;
        return true;
    }
    
    // Ordenar por nombre
    sort(archivosEncontrados.begin(), archivosEncontrados.end(), compararPorNombre);
    
    cout << "Se encontraron " << archivosEncontrados.size() << " archivo(s):" << endl;
    cout << "================================================" << endl;
    
    for (size_t i = 0; i < archivosEncontrados.size(); i++) {
        mostrarInfoArchivoTamanio(archivosEncontrados[i], i);
    }
    
    cout << "\nESTADÍSTICAS:" << endl;
    cout << "Total de archivos: " << archivosEncontrados.size() << endl;
    cout << "Espacio total usado: " << formatearTamanio(tamanioEspecifico * archivosEncontrados.size()) << endl;
    cout << "================================================" << endl;
    
    return true;
}

// Función para listar archivos en un rango de tamaños
static bool listarArchivosPorRangoTamanio(int tamanioMin, int tamanioMax) {
    if (tamanioMin > tamanioMax) {
        cout << "El tamaño mínimo debe ser menor o igual al tamaño máximo." << endl;
        return false;
    }
    
    EntradaArchivo directorio[(BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)];
    
    if (!cargarDirectorioTamanio(directorio)) {
        cerr << "Error al cargar el directorio desde el disco." << endl;
        return false;
    }
    
    vector<ArchivoInfoTamanio> archivosEncontrados;
    int totalEntradas = (BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo);
    
    // Buscar archivos en el rango de tamaños
    for (int i = 0; i < totalEntradas; i++) {
        if (directorio[i].activo && 
            directorio[i].tamanio >= tamanioMin && 
            directorio[i].tamanio <= tamanioMax) {
            
            ArchivoInfoTamanio info;
            info.nombre = directorio[i].nombre;
            info.tamanio = directorio[i].tamanio;
            info.bloqueInicial = directorio[i].bloqueInicial;
            info.fechaCreacion = directorio[i].fechaCreacion;
            
            archivosEncontrados.push_back(info);
        }
    }
    
    // Mostrar resultados
    cout << "\n========== ARCHIVOS CON TAMAÑO ENTRE " << formatearTamanio(tamanioMin) 
         << " Y " << formatearTamanio(tamanioMax) << " ==========" << endl;
    
    if (archivosEncontrados.empty()) {
        cout << "No se encontraron archivos en el rango de tamaños especificado." << endl;
        return true;
    }
    
    // Ordenar por tamaño (mayor a menor)
    sort(archivosEncontrados.begin(), archivosEncontrados.end(), compararPorTamanio);
    
    cout << "Se encontraron " << archivosEncontrados.size() << " archivo(s):" << endl;
    cout << "================================================" << endl;
    
    for (size_t i = 0; i < archivosEncontrados.size(); i++) {
        mostrarInfoArchivoTamanio(archivosEncontrados[i], i);
    }
    
    // Mostrar estadísticas
    int tamanioTotal = 0;
    int tamanioMinEncontrado = INT_MAX;
    int tamanioMaxEncontrado = 0;
    
    for (const auto& archivo : archivosEncontrados) {
        tamanioTotal += archivo.tamanio;
        if (archivo.tamanio < tamanioMinEncontrado) tamanioMinEncontrado = archivo.tamanio;
        if (archivo.tamanio > tamanioMaxEncontrado) tamanioMaxEncontrado = archivo.tamanio;
    }
    
    cout << "\nESTADÍSTICAS:" << endl;
    cout << "Total de archivos: " << archivosEncontrados.size() << endl;
    cout << "Espacio total usado: " << formatearTamanio(tamanioTotal) << endl;
    cout << "Tamaño promedio: " << formatearTamanio(tamanioTotal / archivosEncontrados.size()) << endl;
    cout << "Archivo más pequeño: " << formatearTamanio(tamanioMinEncontrado) << endl;
    cout << "Archivo más grande: " << formatearTamanio(tamanioMaxEncontrado) << endl;
    cout << "================================================" << endl;
    
    return true;
}

// Función para mostrar archivos ordenados por tamaño
static bool mostrarArchivosPorTamanio(bool ordenAscendente = false) {
    EntradaArchivo directorio[(BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)];
    
    if (!cargarDirectorioTamanio(directorio)) {
        cerr << "Error al cargar el directorio desde el disco." << endl;
        return false;
    }
    
    vector<ArchivoInfoTamanio> todosLosArchivos;
    int totalEntradas = (BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo);
    
    // Recopilar todos los archivos activos
    for (int i = 0; i < totalEntradas; i++) {
        if (directorio[i].activo) {
            ArchivoInfoTamanio info;
            info.nombre = directorio[i].nombre;
            info.tamanio = directorio[i].tamanio;
            info.bloqueInicial = directorio[i].bloqueInicial;
            info.fechaCreacion = directorio[i].fechaCreacion;
            
            todosLosArchivos.push_back(info);
        }
    }
    
    if (todosLosArchivos.empty()) {
        cout << "No hay archivos en el sistema." << endl;
        return true;
    }
    
    // Ordenar según la preferencia
    if (ordenAscendente) {
        sort(todosLosArchivos.begin(), todosLosArchivos.end(), 
             [](const ArchivoInfoTamanio& a, const ArchivoInfoTamanio& b) {
                 return a.tamanio < b.tamanio; // Menor a mayor
             });
    } else {
        sort(todosLosArchivos.begin(), todosLosArchivos.end(), compararPorTamanio);
    }
    
    // Mostrar resultados
    cout << "\n========== TODOS LOS ARCHIVOS ORDENADOS POR TAMAÑO ";
    cout << (ordenAscendente ? "(MENOR A MAYOR)" : "(MAYOR A MENOR)") << " ==========" << endl;
    
    cout << "Total de archivos encontrados: " << todosLosArchivos.size() << endl;
    cout << "================================================" << endl;
    
    for (size_t i = 0; i < todosLosArchivos.size(); i++) {
        mostrarInfoArchivoTamanio(todosLosArchivos[i], i);
    }
    
    // Mostrar estadísticas generales
    int tamanioTotal = 0;
    for (const auto& archivo : todosLosArchivos) {
        tamanioTotal += archivo.tamanio;
    }
    
    cout << "\nESTADÍSTICAS GENERALES:" << endl;
    cout << "Total de archivos: " << todosLosArchivos.size() << endl;
    cout << "Espacio total usado: " << formatearTamanio(tamanioTotal) << endl;
    cout << "Tamaño promedio: " << formatearTamanio(tamanioTotal / todosLosArchivos.size()) << endl;
    cout << "Archivo más pequeño: " << formatearTamanio(todosLosArchivos.back().tamanio) << endl;
    cout << "Archivo más grande: " << formatearTamanio(todosLosArchivos.front().tamanio) << endl;
    cout << "================================================" << endl;
    
    return true;
}

// Función principal 
void filtroTamanio() {
    int opcion;
    string input;
    int tamanio, tamanioMin, tamanioMax;
    
    cout << "\n=== FILTRAR ARCHIVOS POR TAMAÑO ===" << endl;
    cout << "1. Buscar por tamaño específico (en bytes)" << endl;
    cout << "2. Buscar por rango de tamaños" << endl;
    cout << "3. Mostrar todos los archivos ordenados por tamaño (mayor a menor)" << endl;
    cout << "4. Mostrar todos los archivos ordenados por tamaño (menor a mayor)" << endl;
    cout << "Seleccione una opción (1-4): ";
    
    cin >> opcion;
    cin.ignore(); // Limpiar buffer
    
    switch (opcion) {
        case 1:
            cout << "Ingrese el tamaño exacto en bytes: ";
            getline(cin, input);
            
            if (input.empty() || !validarNumero(input, tamanio)) {
                cout << "Error: Debe ingresar un número válido." << endl;
                return;
            }
            
            listarArchivosPorTamanioEspecifico(tamanio);
            break;
            
        case 2:
            cout << "Ingrese el tamaño mínimo en bytes: ";
            getline(cin, input);
            
            if (input.empty() || !validarNumero(input, tamanioMin)) {
                cout << "Error: Debe ingresar un número válido para el tamaño mínimo." << endl;
                return;
            }
            
            cout << "Ingrese el tamaño máximo en bytes: ";
            getline(cin, input);
            
            if (input.empty() || !validarNumero(input, tamanioMax)) {
                cout << "Error: Debe ingresar un número válido para el tamaño máximo." << endl;
                return;
            }
            
            listarArchivosPorRangoTamanio(tamanioMin, tamanioMax);
            break;
            
        case 3:
            mostrarArchivosPorTamanio(false);
            break;
            
        case 4:
            mostrarArchivosPorTamanio(true); 
            break;
            
        default:
            cout << "Opción no válida." << endl;
            break;
    }
}