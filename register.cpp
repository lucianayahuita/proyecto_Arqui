#include "register.h"
#include <fstream>
#include <string>
#include <iostream>
using namespace std;

const string archivo = "users.txt";

bool registerUser() {
    string userName, password;

    cout << "Ingrese su nombre de usuario: ";
    getline(cin, userName);

    cout << "Ingrese su contraseña: ";
    getline(cin, password);

    ofstream file(archivo, ios::app);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo para guardar el usuario." << endl;
        return false;
    }

    file << userName << ":" << password << endl;

    file.close();
    cout << "Usuario registrado correctamente." << endl;
    return true;
}

