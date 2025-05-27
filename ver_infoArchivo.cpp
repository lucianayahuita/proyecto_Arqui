#include "ver_infoArchivo.h"
#include "leer_Archivo.h"
#include "crear_archivo.h"
#include "configuration_filesystem.h"
#include <iomanip>
#include <ctime>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

using namespace std;

// Declaraciones externas
extern bool cargarFATyDirectorio(int* fat, EntradaArchivo* directorio);

vector<int> obtenerCadenaBloques(int* fat, int bloqueInicial) {
    vector<int> bloques;
    int bloqueActual = bloqueInicial;
    
    while (bloqueActual >= DATA_BLOCK_START && bloqueActual < TOTAL_BLOCKS) {
        bloques.push_back(bloqueActual);
        
        if (fat[bloqueActual] == -3) { 
            break;
        }
        
        if (fat[bloqueActual] < DATA_BLOCK_START || fat[bloqueActual] >= TOTAL_BLOCKS) {
            break; 
        }
        
        bloqueActual = fat[bloqueActual];
    }
    
    return bloques;
}

// Función auxiliar para calcular fragmentación
double calcularFragmentacion(const vector<int>& bloques) {
    if (bloques.size() <= 1) return 0.0;
    
    int bloquesConsecutivos = 0;
    for (size_t i = 1; i < bloques.size(); i++) {
        if (bloques[i] == bloques[i-1] + 1) {
            bloquesConsecutivos++;
        }
    }
    
    return (1.0 - ((double)bloquesConsecutivos / (bloques.size() - 1))) * 100.0;
}

bool obtenerInformacion() {
    string nombreArchivo;
    cout << "Ingrese el nombre del archivo para ver su información: ";
    getline(cin, nombreArchivo);

    if (nombreArchivo.length() >= 32) {
        cout << "Nombre demasiado largo (máximo 31 caracteres).\n";
        return false;
    }

    int fat[TOTAL_BLOCKS];
    EntradaArchivo directorio[(BLOCK_SIZE * DIRECTORY_BLOCKS) / sizeof(EntradaArchivo)];

    if (!cargarFATyDirectorio(fat, directorio)) {
        cerr << "Error al cargar FAT y directorio.\n";
        return false;
    }

    // Buscar el archivo
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
    
    // Obtener información adicional
    vector<int> bloques = obtenerCadenaBloques(fat, archivo.bloqueInicial);
    int bloquesUsados = (int)bloques.size();
    int bloquesNecesarios = (archivo.tamanio + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (bloquesNecesarios == 0) bloquesNecesarios = 1;
    
    double fragmentacion = calcularFragmentacion(bloques);
    int espacioUsado = bloquesUsados * BLOCK_SIZE;
    int espacioLibre = espacioUsado - archivo.tamanio;
    double eficiencia = (archivo.tamanio > 0) ? ((double)archivo.tamanio / espacioUsado) * 100.0 : 0.0;

    // Formatear fecha
    time_t fecha = archivo.fechaCreacion;
    tm* tiempo = localtime(&fecha);
    char fechaStr[100];
    strftime(fechaStr, sizeof(fechaStr), "%d/%m/%Y %H:%M:%S", tiempo);

    // Mostrar información completa del archivo
    cout << "\n" << string(60, '=') << "\n";
    cout << "         INFORMACIÓN DETALLADA DEL ARCHIVO\n";
    cout << string(60, '=') << "\n";
    
    cout << left << setw(25) << "Nombre del archivo:" << "'" << archivo.nombre << "'\n";
    cout << left << setw(25) << "Tamaño del contenido:" << archivo.tamanio << " bytes\n";
    cout << left << setw(25) << "Fecha de creación:" << fechaStr << "\n";
    cout << left << setw(25) << "Bloque inicial:" << archivo.bloqueInicial << "\n";
    
    cout << "\n" << string(40, '-') << " ALMACENAMIENTO " << string(5, '-') << "\n";
    cout << left << setw(25) << "Bloques utilizados:" << bloquesUsados << "\n";
    cout << left << setw(25) << "Bloques necesarios:" << bloquesNecesarios << "\n";
    cout << left << setw(25) << "Espacio total usado:" << espacioUsado << " bytes\n";
    cout << left << setw(25) << "Espacio libre interno:" << espacioLibre << " bytes\n";
    cout << left << setw(25) << "Eficiencia:" << fixed << setprecision(1) << eficiencia << "%\n";
    cout << left << setw(25) << "Fragmentación:" << fixed << setprecision(1) << fragmentacion << "%\n";
    
    cout << "\n" << string(40, '-') << " UBICACIÓN " << string(9, '-') << "\n";
    cout << "Bloques ocupados: ";
    for (size_t i = 0; i < bloques.size(); i++) {
        cout << bloques[i];
        if (i < bloques.size() - 1) cout << " -> ";
    }
    cout << "\n";
    
    // Mostrar mapa de bloques
    cout << "\nMapa de bloques (cada bloque = " << BLOCK_SIZE << " bytes):\n";
    for (size_t i = 0; i < bloques.size(); i++) {
        int bytesEnBloque = min(BLOCK_SIZE, archivo.tamanio - (int)(i * BLOCK_SIZE));
        if (bytesEnBloque <= 0) bytesEnBloque = 0;
        
        cout << "  Bloque " << setw(4) << bloques[i] 
             << ": " << setw(4) << bytesEnBloque << " bytes usados";
        
        if (i == 0) cout << " (inicio)";
        if (i == bloques.size() - 1) cout << " (final)";
        cout << "\n";
    }
    
    cout << "\n" << string(40, '-') << " ESTADO " << string(12, '-') << "\n";
    cout << left << setw(25) << "Estado del archivo:" << "Activo\n";
    cout << left << setw(25) << "Integridad:" << ((bloquesUsados == bloquesNecesarios) ? "OK" : "Posible problema") << "\n";
    
    if (fragmentacion > 0) {
        cout << left << setw(25) << "Nota:" << "Archivo fragmentado (" 
             << fixed << setprecision(1) << fragmentacion << "% fragmentación)\n";
    } else {
        cout << left << setw(25) << "Nota:" << "Archivo almacenado de forma contigua\n";
    }
    
    cout << string(60, '=') << "\n";
    
    // Preguntar si quiere ver el contenido
    char respuesta;
    cout << "\n¿Desea ver el contenido del archivo? (s/n): ";
    cin >> respuesta;
    cin.ignore();
    
    if (respuesta == 's' || respuesta == 'S') {
        // Leer y mostrar contenido
        ifstream disco(DISK_FILENAME, ios::binary);
        if (!disco) {
            cerr << "Error al abrir el disco para leer contenido.\n";
            return true; // La información ya se mostró correctamente
        }
        
        cout << "\n" << string(30, '-') << " CONTENIDO " << string(19, '-') << "\n";
        
        string contenido;
        for (int bloque : bloques) {
            disco.seekg(bloque * BLOCK_SIZE, ios::beg);
            char buffer[BLOCK_SIZE];
            disco.read(buffer, BLOCK_SIZE);
            
            int bytesALeer = min(BLOCK_SIZE, archivo.tamanio - (int)contenido.size());
            if (bytesALeer > 0) {
                contenido.append(buffer, bytesALeer);
            }
        }
        
        if (contenido.empty()) {
            cout << "(Archivo vacío)\n";
        } else {
            cout << "\"" << contenido << "\"\n";
        }
        
        cout << string(60, '-') << "\n";
        disco.close();
    }

    return true;
}