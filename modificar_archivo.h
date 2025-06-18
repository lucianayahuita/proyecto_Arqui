#ifndef MODIFICAR_ARCHIVO_H
#define MODIFICAR_ARCHIVO_H

#include "configuration_filesystem.h"
#include <string>

// Para modificación directa desde encriptación
bool modificarArchivo(const std::string& nombreArchivo, const std::string& nuevoContenido);

// Para modificación interactiva
bool modificarArchivo();

#endif