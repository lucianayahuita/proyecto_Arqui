// DEFINICIONES PARA MODIFICAR UN ARCHIVO
#ifndef MODIFICAR_ARCHIVO_H
#define MODIFICAR_ARCHIVO_H

#include "configuration_filesystem.h"
#include "crear_archivo.h"

bool modificarArchivo();
bool cargarFATyDirectorio(int* fat, EntradaArchivo* directorio); 

#endif