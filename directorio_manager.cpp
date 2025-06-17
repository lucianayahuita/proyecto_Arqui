#include "directorio_manager.h"
#include "configuration_filesystem.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>

// Variable global para el directorio actual
std::string directorioActual = "";

// Lista de directorios creados (se mantiene en memoria)
std::vector<std::string> directoriosExistentes;

void inicializarSistemaDirectorios() {
    directorioActual = "";
    directoriosExistentes.clear();
    directoriosExistentes.push_back(""); // Directorio raíz
    std::cout << "Sistema de directorios inicializado.\n";
}

void crearDirectorio(const std::string& nombre) {
    if (nombre.empty() || nombre.length() > 20) {
        std::cout << "Nombre de directorio inválido (vacío o muy largo).\n";
        return;
    }
    
    // Crear la ruta completa del nuevo directorio
    std::string rutaCompleta = directorioActual;
    if (!rutaCompleta.empty()) {
        rutaCompleta += "/";
    }
    rutaCompleta += nombre;
    
    // Verificar si ya existe
    for (const auto& dir : directoriosExistentes) {
        if (dir == rutaCompleta) {
            std::cout << "El directorio '" << nombre << "' ya existe.\n";
            return;
        }
    }
    
    // Agregar a la lista de directorios
    directoriosExistentes.push_back(rutaCompleta);
    std::cout << "Directorio '" << nombre << "' creado exitosamente.\n";
}

void cambiarDirectorio(const std::string& nombre) {
    if (nombre == "..") {
        // Subir un nivel
        if (directorioActual.empty()) {
            std::cout << "Ya estás en el directorio raíz.\n";
            return;
        }
        
        size_t ultimaBarra = directorioActual.find_last_of('/');
        if (ultimaBarra == std::string::npos) {
            directorioActual = "";
        } else {
            directorioActual = directorioActual.substr(0, ultimaBarra);
        }
        
        std::cout << "Directorio cambiado.\n";
        mostrarRutaActual();
        return;
    }
    
    if (nombre == "/" || nombre == "root") {
        directorioActual = "";
        std::cout << "Cambiado al directorio raíz.\n";
        mostrarRutaActual();
        return;
    }
    
    // Crear la ruta del directorio destino
    std::string rutaDestino = directorioActual;
    if (!rutaDestino.empty()) {
        rutaDestino += "/";
    }
    rutaDestino += nombre;
    
    // Verificar si existe
    bool existe = false;
    for (const auto& dir : directoriosExistentes) {
        if (dir == rutaDestino) {
            existe = true;
            break;
        }
    }
    
    if (existe) {
        directorioActual = rutaDestino;
        std::cout << "Directorio cambiado a '" << nombre << "'.\n";
        mostrarRutaActual();
    } else {
        std::cout << "El directorio '" << nombre << "' no existe.\n";
    }
}

void listarDirectorios() {
    std::cout << "\n=== DIRECTORIOS ===\n";
    
    if (!directorioActual.empty()) {
        std::cout << "[DIR] .. (directorio padre)\n";
    }
    
    bool haySubdirectorios = false;
    for (const auto& dir : directoriosExistentes) {
        if (dir.empty()) continue; // Saltar directorio raíz
        
        // Verificar si es un subdirectorio del directorio actual
        std::string prefijo = directorioActual;
        if (!prefijo.empty()) {
            prefijo += "/";
        }
        
        if (dir.find(prefijo) == 0) {
            std::string resto = dir.substr(prefijo.length());
            // Solo mostrar directorios inmediatos (sin barras adicionales)
            if (resto.find('/') == std::string::npos && !resto.empty()) {
                std::cout << "[DIR] " << resto << "\n";
                haySubdirectorios = true;
            }
        }
    }
    
    if (!haySubdirectorios) {
        std::cout << "No hay subdirectorios.\n";
    }
}

std::string obtenerDirectorioActual() {
    return directorioActual.empty() ? "root" : directorioActual;
}

void mostrarRutaActual() {
    std::cout << "Ruta actual: /" << (directorioActual.empty() ? "" : directorioActual) << "\n";
}

// FUNCIONES CLAVE: Estas son las que hacen la magia sin modificar tus funciones existentes
std::string agregarPrefijoDirectorio(const std::string& nombreArchivo) {
    if (directorioActual.empty()) {
        return nombreArchivo;
    }
    return directorioActual + "_" + nombreArchivo;
}

std::string quitarPrefijoDirectorio(const std::string& nombreCompleto) {
    if (directorioActual.empty()) {
        return nombreCompleto;
    }
    
    std::string prefijo = directorioActual + "_";
    if (nombreCompleto.find(prefijo) == 0) {
        return nombreCompleto.substr(prefijo.length());
    }
    return nombreCompleto;
}

bool perteneceADirectorioActual(const std::string& nombreCompleto) {
    if (directorioActual.empty()) {
        // En el directorio raíz, mostrar archivos sin prefijo de directorio
        return nombreCompleto.find('_') == std::string::npos;
    }
    
    std::string prefijo = directorioActual + "_";
    return nombreCompleto.find(prefijo) == 0;
}