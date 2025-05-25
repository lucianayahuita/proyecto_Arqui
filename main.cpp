
#include "configuration_filesystem.h"
#include <iostream>

// Declarar las funciones que están definidas en configuration_filesystem.cpp
void formatearDisco();
void cargarDisco();

int main() {
    std::cout << "Iniciando sistema de archivos...\n";
    cargarDisco();

    // Aquí seguirías con el resto de tu programa (crear archivos, leer, etc.)

    return 0;
}
