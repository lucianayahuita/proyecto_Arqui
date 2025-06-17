#ifndef REGISTER_H
#define REGISTER_H

struct User {
    char userName[32];     
    char password[40];        
};

bool registerUser();

#endif
