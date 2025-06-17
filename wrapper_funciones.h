#ifndef WRAPPER_FUNCIONES_H
#define WRAPPER_FUNCIONES_H
#include "directorio_manager.h"
#include <iostream>
#include <string>

// Función que intercepta la entrada del usuario para agregar prefijo de directorio
inline std::string leerNombreArchivoConDirectorio(const std::string& prompt) {
    std::string nombre;
    std::cout << prompt;
    std::getline(std::cin, nombre);
    return agregarPrefijoDirectorio(nombre);
}

// Función para mostrar nombre sin prefijo al usuario
inline void mostrarNombreSinDirectorio(const std::string& nombreCompleto) {
    std::cout << quitarPrefijoDirectorio(nombreCompleto);
}

#endif