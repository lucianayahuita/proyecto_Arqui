#ifndef DIRECTORIO_MANAGER_H
#define DIRECTORIO_MANAGER_H

#include <string>

// Variable global para el directorio actual
extern std::string directorioActual;

// Funciones simples que NO requieren modificar las funciones existentes
void inicializarSistemaDirectorios();
void crearDirectorio(const std::string& nombre);
void cambiarDirectorio(const std::string& nombre);
void listarDirectorios();
std::string obtenerDirectorioActual();
void mostrarRutaActual();

// Función para agregar prefijo de directorio a nombres de archivo
std::string agregarPrefijoDirectorio(const std::string& nombreArchivo);

// Función para quitar prefijo de directorio de nombres de archivo
std::string quitarPrefijoDirectorio(const std::string& nombreCompleto);

// Función para verificar si un archivo pertenece al directorio actual
bool perteneceADirectorioActual(const std::string& nombreCompleto);

#endif