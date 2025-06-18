#ifndef REGISTER_H
#define REGISTER_H

#include <string>
using namespace std;

struct User {
    char userName[32];          
    char password[40];         
};

// Funciones principales
bool registerUser();
bool loginUser();
bool authenticateUser();  // Función principal que maneja todo el proceso

// Funciones auxiliares
bool userExists(const string& userName);
bool validateCredentials(const string& userName, const string& password);
void displayMenu();

#endif