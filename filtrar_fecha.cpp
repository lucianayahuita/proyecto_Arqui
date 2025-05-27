#include "filtrar_fecha.h"
#include "configuration_filesystem.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <ctime>
#include <sstream>

using namespace std;

// Estructura para almacenar información de archivo filtrado
struct ArchivoInfoFecha {
    string nombre;
    int tamanio;
    int bloqueInicial;
    time_t fechaCreacion;
};

// Función estática para cargar el directorio desde disco 
static bool cargarDirectorioFecha(EntradaArchivo* directorio) {
    ifstream disco(DISK_FILENAME, ios::binary);
    if (!disco) return false;
    
    // Saltar la FAT
    disco.seekg(sizeof(int) * TOTAL_BLOCKS, ios::beg);
    
    // Leer el directorio
    disco.read(reinterpret_cast<char*>(directorio), 
               sizeof(EntradaArchivo) * ((BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)));
    
    return disco.good();
}

// Función estática para convertir time_t a string legible 
static string formatearFechaFiltro(time_t tiempo) {
    char buffer[100];
    struct tm* timeinfo = localtime(&tiempo);
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", timeinfo);
    return string(buffer);
}

// Función para obtener solo la fecha (sin hora) de un time_t
static string obtenerSoloFecha(time_t tiempo) {
    char buffer[20];
    struct tm* timeinfo = localtime(&tiempo);
    strftime(buffer, sizeof(buffer), "%d/%m/%Y", timeinfo);
    return string(buffer);
}

// Función para convertir string fecha a time_t (solo fecha, hora a 00:00:00)
static time_t convertirFechaString(const string& fechaStr) {
    struct tm tm = {};
    stringstream ss(fechaStr);
    string item;
    vector<int> fecha;
    
    // Parsear dd/mm/yyyy
    while (getline(ss, item, '/')) {
        fecha.push_back(stoi(item));
    }
    
    if (fecha.size() != 3) return -1;
    
    tm.tm_mday = fecha[0];    // día
    tm.tm_mon = fecha[1] - 1; // mes (0-11)
    tm.tm_year = fecha[2] - 1900; // año desde 1900
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    tm.tm_isdst = -1;
    
    return mktime(&tm);
}

// Función para validar formato de fecha dd/mm/yyyy
static bool validarFormatoFecha(const string& fecha) {
    if (fecha.length() != 10) return false;
    if (fecha[2] != '/' || fecha[5] != '/') return false;
    
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!isdigit(fecha[i])) return false;
    }
    
    return true;
}

// Función estática auxiliar para mostrar la información de un archivo 
static void mostrarInfoArchivoFecha(const ArchivoInfoFecha& archivo, int indice) {
    cout << "[" << (indice + 1) << "] " << archivo.nombre << endl;
    cout << "    Tamaño: " << archivo.tamanio << " bytes" << endl;
    cout << "    Bloque inicial: " << archivo.bloqueInicial << endl;
    cout << "    Fecha de creación: " << formatearFechaFiltro(archivo.fechaCreacion) << endl;
    cout << "    ----------------------------------------" << endl;
}

// Función para listar archivos por fecha específica
static bool listarArchivosPorFecha(const string& fechaBuscar) {
    if (!validarFormatoFecha(fechaBuscar)) {
        cout << "Formato de fecha inválido. Use dd/mm/yyyy (ej: 15/03/2024)" << endl;
        return false;
    }
    
    EntradaArchivo directorio[(BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)];
    
    if (!cargarDirectorioFecha(directorio)) {
        cerr << "Error al cargar el directorio desde el disco." << endl;
        return false;
    }
    
    vector<ArchivoInfoFecha> archivosEncontrados;
    int totalEntradas = (BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo);
    
    // Buscar archivos con la fecha especificada
    for (int i = 0; i < totalEntradas; i++) {
        if (directorio[i].activo) {
            string fechaArchivo = obtenerSoloFecha(directorio[i].fechaCreacion);
            
            if (fechaArchivo == fechaBuscar) {
                ArchivoInfoFecha info;
                info.nombre = directorio[i].nombre;
                info.tamanio = directorio[i].tamanio;
                info.bloqueInicial = directorio[i].bloqueInicial;
                info.fechaCreacion = directorio[i].fechaCreacion;
                
                archivosEncontrados.push_back(info);
            }
        }
    }
    
    // Mostrar resultados
    cout << "\n========== ARCHIVOS CREADOS EL " << fechaBuscar << " ==========" << endl;
    
    if (archivosEncontrados.empty()) {
        cout << "No se encontraron archivos creados en la fecha '" << fechaBuscar << "'." << endl;
        return true;
    }
    
    cout << "Se encontraron " << archivosEncontrados.size() << " archivo(s):" << endl;
    cout << "================================================" << endl;
    
    for (size_t i = 0; i < archivosEncontrados.size(); i++) {
        mostrarInfoArchivoFecha(archivosEncontrados[i], i);
    }
    
    // Mostrar estadísticas
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

// Función para listar archivos en un rango de fechas
static bool listarArchivosPorRangoFecha(const string& fechaInicio, const string& fechaFin) {
    if (!validarFormatoFecha(fechaInicio) || !validarFormatoFecha(fechaFin)) {
        cout << "Formato de fecha inválido. Use dd/mm/yyyy (ej: 15/03/2024)" << endl;
        return false;
    }
    
    time_t tiempoInicio = convertirFechaString(fechaInicio);
    time_t tiempoFin = convertirFechaString(fechaFin);
    
    if (tiempoInicio == -1 || tiempoFin == -1) {
        cout << "Error al convertir las fechas." << endl;
        return false;
    }
    
    if (tiempoInicio > tiempoFin) {
        cout << "La fecha de inicio debe ser anterior o igual a la fecha de fin." << endl;
        return false;
    }
    
    // Ajustar tiempo de fin al final del día (23:59:59)
    tiempoFin += 86399; // 24*60*60 - 1 segundos
    
    EntradaArchivo directorio[(BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)];
    
    if (!cargarDirectorioFecha(directorio)) {
        cerr << "Error al cargar el directorio desde el disco." << endl;
        return false;
    }
    
    vector<ArchivoInfoFecha> archivosEncontrados;
    int totalEntradas = (BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo);
    
    // Buscar archivos en el rango de fechas
    for (int i = 0; i < totalEntradas; i++) {
        if (directorio[i].activo) {
            time_t fechaArchivo = directorio[i].fechaCreacion;
            
            if (fechaArchivo >= tiempoInicio && fechaArchivo <= tiempoFin) {
                ArchivoInfoFecha info;
                info.nombre = directorio[i].nombre;
                info.tamanio = directorio[i].tamanio;
                info.bloqueInicial = directorio[i].bloqueInicial;
                info.fechaCreacion = directorio[i].fechaCreacion;
                
                archivosEncontrados.push_back(info);
            }
        }
    }
    
    // Mostrar resultados
    cout << "\n========== ARCHIVOS CREADOS ENTRE " << fechaInicio << " Y " << fechaFin << " ==========" << endl;
    
    if (archivosEncontrados.empty()) {
        cout << "No se encontraron archivos creados en el rango de fechas especificado." << endl;
        return true;
    }
    
    cout << "Se encontraron " << archivosEncontrados.size() << " archivo(s):" << endl;
    cout << "================================================" << endl;
    
    for (size_t i = 0; i < archivosEncontrados.size(); i++) {
        mostrarInfoArchivoFecha(archivosEncontrados[i], i);
    }
    
    // Mostrar estadísticas
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
void filtroFecha() {
    int opcion;
    string fecha, fechaInicio, fechaFin;
    
    cout << "\n=== FILTRAR ARCHIVOS POR FECHA DE CREACIÓN ===" << endl;
    cout << "1. Buscar por fecha específica" << endl;
    cout << "2. Buscar por rango de fechas" << endl;
    cout << "Seleccione una opción (1-2): ";
    
    cin >> opcion;
    cin.ignore(); // Limpiar buffer
    
    switch (opcion) {
        case 1:
            cout << "Ingrese la fecha a buscar (dd/mm/yyyy): ";
            getline(cin, fecha);
            
            if (fecha.empty()) {
                cout << "Error: Debe ingresar una fecha." << endl;
                return;
            }
            
            listarArchivosPorFecha(fecha);
            break;
            
        case 2:
            cout << "Ingrese la fecha de inicio (dd/mm/yyyy): ";
            getline(cin, fechaInicio);
            
            if (fechaInicio.empty()) {
                cout << "Error: Debe ingresar la fecha de inicio." << endl;
                return;
            }
            
            cout << "Ingrese la fecha de fin (dd/mm/yyyy): ";
            getline(cin, fechaFin);
            
            if (fechaFin.empty()) {
                cout << "Error: Debe ingresar la fecha de fin." << endl;
                return;
            }
            
            listarArchivosPorRangoFecha(fechaInicio, fechaFin);
            break;
            
        default:
            cout << "Opción no válida." << endl;
            break;
    }
}