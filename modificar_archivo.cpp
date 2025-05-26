#include "modificar_archivo.h"
#include "leer_Archivo.h"
#include "crear_archivo.h"
#include "configuration_filesystem.h"
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;
bool modificarArchivo(){
    if (leerArchivo()){
        cout<<"exito"<<endl;
        return true;
    }else{
        cout<<"fracaso"<<endl;
        return false;
    }
}