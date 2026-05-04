#pragma once
#include <string>

// Todos los tipos de token del lenguaje TaskScript
enum class TokenType {
    // Palabras reservadas
    TABLERO, COLUMNA, TAREA, PRIORIDAD, RESPONSABLE, FECHA_LIMITE,
    // Enumeraciones de prioridad
    ALTA, MEDIA, BAJA,
    // Literales
    CADENA,   // "texto"
    ENTERO,   // 123
    FECHA,    // 2026-05-01
    // Delimitadores
    LLAVE_ABR,    // {
    LLAVE_CIE,    // }
    CORCHETE_ABR, // [
    CORCHETE_CIE, // ]
    DOS_PUNTOS,   // :
    COMA,         // ,
    PUNTO_COMA,   // ;
    // Especiales
    FIN_ARCHIVO,
    DESCONOCIDO
};

// Convierte un TokenType a string para los reportes
std::string tokenTypeToString(TokenType type);

struct Token {
    TokenType type;
    std::string lexema;
    int linea;
    int columna;

    Token(TokenType t, std::string lex, int lin, int col)
        : type(t), lexema(std::move(lex)), linea(lin), columna(col) {}
};