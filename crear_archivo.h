// DEFINICIONES PARA CREAR UN ARCHIVO
#ifndef CREAR_ARCHIVO_H
#define CREAR_ARCHIVO_H

#include "configuration_filesystem.h"

bool crearArchivo();
bool cargarFATyDirectorio(int* fat, EntradaArchivo* directorio); 

#endif
