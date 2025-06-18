#ifndef ENCRIPTACION_H
#define ENCRIPTACION_H

#include "configuration_filesystem.h"
#include <string>

// Función para el menú de encriptación
void mostrarMenuEncriptacion();

// Funciones principales
void procesarEncriptacion();
void procesarDesencriptacion();

// Función XOR para encriptar/desencriptar
std::string xorEncryptDecrypt(const std::string& input, const std::string& key);

// Función para obtener contenido de archivo (compartida)
std::string obtenerContenidoArchivo(int bloqueInicial, int tamanio);

#endif