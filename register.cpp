#include "register.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

const string USERS_FILE = "users.txt";

// Función principal que maneja todo el proceso de autenticación
bool authenticateUser() {
    int opcion;
    
    while (true) {
        displayMenu();
        cin >> opcion;
        cin.ignore(); // Limpiar el buffer
        
        switch (opcion) {
            case 1:
                if (registerUser()) {
                    return true; // Usuario registrado y autenticado
                }
                break;
            case 2:
                if (loginUser()) {
                    return true; // Usuario logueado exitosamente
                }
                break;
            case 3:
                cout << "Saliendo del sistema..." << endl;
                return false;
            default:
                cout << "Opción no válida. Intente nuevamente." << endl;
        }
    }
}

void displayMenu() {
    cout << "\n=== SISTEMA DE AUTENTICACIÓN ===" << endl;
    cout << "1. Registrar nuevo usuario" << endl;
    cout << "2. Iniciar sesión" << endl;
    cout << "3. Salir" << endl;
    cout << "Seleccione una opción: ";
}

bool registerUser() {
    string userName, password;
    
    cout << "\n--- REGISTRO DE USUARIO ---" << endl;
    cout << "Ingrese su nombre de usuario: ";
    getline(cin, userName);
    
    // Validar que el nombre de usuario no esté vacío
    if (userName.empty()) {
        cout << "Error: El nombre de usuario no puede estar vacío." << endl;
        return false;
    }
    
    // Validar longitud del nombre de usuario
    if (userName.length() >= 32) {
        cout << "Error: El nombre de usuario es demasiado largo (máximo 31 caracteres)." << endl;
        return false;
    }
    
    // Verificar si el usuario ya existe
    if (userExists(userName)) {
        cout << "Error: El usuario '" << userName << "' ya existe." << endl;
        cout << "¿Desea iniciar sesión? (s/n): ";
        char respuesta;
        cin >> respuesta;
        cin.ignore();
        
        if (respuesta == 's' || respuesta == 'S') {
            return loginUser();
        }
        return false;
    }
    
    cout << "Ingrese su contraseña: ";
    getline(cin, password);
    
    // Validar que la contraseña no esté vacía
    if (password.empty()) {
        cout << "Error: La contraseña no puede estar vacía." << endl;
        return false;
    }
    
    // Validar longitud de la contraseña
    if (password.length() >= 40) {
        cout << "Error: La contraseña es demasiado larga (máximo 39 caracteres)." << endl;
        return false;
    }
    
    // Guardar el nuevo usuario
    ofstream file(USERS_FILE, ios::app);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo para guardar el usuario." << endl;
        return false;
    }
    
    file << userName << ":" << password << endl;
    file.close();
    
    cout << "Usuario '" << userName << "' registrado correctamente." << endl;
    cout << "Sesión iniciada automáticamente." << endl;
    return true;
}

bool loginUser() {
    string userName, password;
    
    cout << "\n--- INICIAR SESIÓN ---" << endl;
    cout << "Nombre de usuario: ";
    getline(cin, userName);
    
    cout << "Contraseña: ";
    getline(cin, password);
    
    if (validateCredentials(userName, password)) {
        cout << "¡Bienvenido, " << userName << "!" << endl;
        return true;
    } else {
        cout << "Error: Credenciales incorrectas." << endl;
        return false;
    }
}

bool userExists(const string& userName) {
    ifstream file(USERS_FILE);
    if (!file.is_open()) {
        // Si el archivo no existe, no hay usuarios registrados
        return false;
    }
    
    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            size_t pos = line.find(':');
            if (pos != string::npos) {
                string existingUser = line.substr(0, pos);
                if (existingUser == userName) {
                    file.close();
                    return true;
                }
            }
        }
    }
    
    file.close();
    return false;
}

bool validateCredentials(const string& userName, const string& password) {
    ifstream file(USERS_FILE);
    if (!file.is_open()) {
        return false;
    }
    
    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            size_t pos = line.find(':');
            if (pos != string::npos) {
                string existingUser = line.substr(0, pos);
                string existingPassword = line.substr(pos + 1);
                
                if (existingUser == userName && existingPassword == password) {
                    file.close();
                    return true;
                }
            }
        }
    }
    
    file.close();
    return false;
}