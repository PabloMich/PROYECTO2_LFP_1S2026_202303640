#pragma once
#include <string>
#include <vector>
#include "Token.h"
#include "ErrorManager.h"

class LexicalAnalyzer {
public:
    LexicalAnalyzer(const std::string& codigo, ErrorManager& errManager);

    // Retorna todos los tokens del código fuente
    std::vector<Token> tokenizar();

private:
    std::string codigo;
    int pos;        // posición actual en el string
    int linea;
    int columna;
    ErrorManager& errManager;

    // Avanza un carácter
    char avanzar();

    // Mira el carácter actual sin avanzar
    char actual() const;

    // Mira el siguiente sin avanzar
    char siguiente() const;

    // Salta espacios, tabs y saltos de línea
    void saltarEspacios();

    // Funciones para reconocer cada tipo de token
    Token leerPalabraOFecha();   // identifiers, keywords, fechas tipo 2026-05-01
    Token leerCadena();          // "texto"
    Token leerEntero();          // 123
    Token leerDelimitador();     // { } [ ] : , ;

    // Dado un lexema, retorna si es palabra reservada o identificador
    TokenType clasificarPalabra(const std::string& lex) const;

    bool esFin() const;
};