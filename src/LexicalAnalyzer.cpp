#include "LexicalAnalyzer.h"
#include <cctype>

LexicalAnalyzer::LexicalAnalyzer(const std::string& codigo, ErrorManager& errManager)
    : codigo(codigo), pos(0), linea(1), columna(1), errManager(errManager) {}

// ── Helpers básicos ─────────────────────────────────────────

bool LexicalAnalyzer::esFin() const {
    return pos >= (int)codigo.size();
}

char LexicalAnalyzer::actual() const {
    if (esFin()) return '\0';
    return codigo[pos];
}

char LexicalAnalyzer::siguiente() const {
    if (pos + 1 >= (int)codigo.size()) return '\0';
    return codigo[pos + 1];
}

char LexicalAnalyzer::avanzar() {
    char c = actual();
    pos++;
    if (c == '\n') { linea++; columna = 1; }
    else { columna++; }
    return c;
}

void LexicalAnalyzer::saltarEspacios() {
    while (!esFin() && std::isspace((unsigned char)actual())) {
        avanzar();
    }
}

// ── Clasificar si un lexema es palabra reservada ────────────

TokenType LexicalAnalyzer::clasificarPalabra(const std::string& lex) const {
    if (lex == "TABLERO")      return TokenType::TABLERO;
    if (lex == "COLUMNA")      return TokenType::COLUMNA;
    if (lex == "tarea")        return TokenType::TAREA;
    if (lex == "prioridad")    return TokenType::PRIORIDAD;
    if (lex == "responsable")  return TokenType::RESPONSABLE;
    if (lex == "fecha_limite") return TokenType::FECHA_LIMITE;
    if (lex == "ALTA")         return TokenType::ALTA;
    if (lex == "MEDIA")        return TokenType::MEDIA;
    if (lex == "BAJA")         return TokenType::BAJA;
    return TokenType::DESCONOCIDO;
}

// ── Leer palabra, keyword o fecha (2026-05-01) ──────────────

Token LexicalAnalyzer::leerPalabraOFecha() {
    int colInicio = columna;
    int linInicio = linea;
    std::string lex;

    // Lee letras, dígitos, _ y además salta chars inválidos reportándolos
    while (!esFin()) {
        char c = actual();

        if (std::isalnum((unsigned char)c) || c == '_') {
            lex += avanzar();
        }
        else if (c == '"' || c == '{' || c == '}' || c == '[' || c == ']' ||
         c == ':' || c == ',' || c == ';' || c == '-' || std::isspace((unsigned char)c)) {
            break;
         }
        else {
            // Carácter inválido DENTRO de una palabra (ej: # en AL#TA)
            errManager.agregarError(
                std::string(1, c), ErrorType::LEXICO,
                std::string("Carácter no reconocido '") + c +
                "' dentro del lexema " + lex,
                linea, columna);
            avanzar(); // saltarlo y continuar leyendo
        }
    }

    // ¿Es una fecha? 4 dígitos seguidos de guion
    if (lex.size() == 4 && std::isdigit((unsigned char)lex[0]) && actual() == '-') {
        std::string resto;
        int savedPos = pos, savedLin = linea, savedCol = columna;
        if (actual() == '-') {
            resto += avanzar();
            for (int i = 0; i < 2 && !esFin() && std::isdigit((unsigned char)actual()); i++)
                resto += avanzar();
        }
        if (actual() == '-') {
            resto += avanzar();
            for (int i = 0; i < 2 && !esFin() && std::isdigit((unsigned char)actual()); i++)
                resto += avanzar();
        }
        if (resto.size() == 6)
            return Token(TokenType::FECHA, lex + resto, linInicio, colInicio);
        pos = savedPos; linea = savedLin; columna = savedCol;
    }

    if (!lex.empty() && std::isdigit((unsigned char)lex[0]))
        return Token(TokenType::ENTERO, lex, linInicio, colInicio);

    if (lex.empty())
        return Token(TokenType::DESCONOCIDO, lex, linInicio, colInicio);

    TokenType tipo = clasificarPalabra(lex);
    if (tipo == TokenType::DESCONOCIDO) {
        errManager.agregarError(lex, ErrorType::LEXICO,
            "Identificador no reconocido: '" + lex + "'", linInicio, colInicio);
    }
    return Token(tipo, lex, linInicio, colInicio);
}

// ── Leer cadena "texto" ──────────────────────────────────────

Token LexicalAnalyzer::leerCadena() {
    int colInicio = columna;
    int linInicio = linea;
    avanzar(); // consume "
    std::string lex;

    while (!esFin() && actual() != '"' && actual() != '\n') {
        char c = actual();
        // Reportar chars inválidos dentro de la cadena
        if (c == '@' || c == '#' || c == '$' || c == '!' || c == '%' ||
            c == '^' || c == '&' || c == '*' || c == '?' || c == '~') {
            errManager.agregarError(
                std::string(1, c), ErrorType::LEXICO,
                std::string("Carácter no reconocido '") + c +
                "' dentro de la cadena \"" + lex + "\"",
                linea, columna);
            }
        lex += avanzar();
    }

    if (esFin() || actual() == '\n') {
        errManager.agregarError("\"" + lex, ErrorType::LEXICO,
            "Cadena no cerrada antes del fin de línea", linInicio, colInicio);
        return Token(TokenType::DESCONOCIDO, lex, linInicio, colInicio);
    }

    avanzar(); // consume "
    return Token(TokenType::CADENA, lex, linInicio, colInicio);
}

// ── Leer entero 123 ─────────────────────────────────────────

Token LexicalAnalyzer::leerEntero() {
    int colInicio = columna;
    int linInicio = linea;
    std::string lex;
    while (!esFin() && std::isdigit((unsigned char)actual())) {
        lex += avanzar();
    }
    return Token(TokenType::ENTERO, lex, linInicio, colInicio);
}

// ── Leer delimitadores ───────────────────────────────────────

Token LexicalAnalyzer::leerDelimitador() {
    int col = columna, lin = linea;
    char c = avanzar();
    switch (c) {
        case '{': return Token(TokenType::LLAVE_ABR,    "{", lin, col);
        case '}': return Token(TokenType::LLAVE_CIE,    "}", lin, col);
        case '[': return Token(TokenType::CORCHETE_ABR, "[", lin, col);
        case ']': return Token(TokenType::CORCHETE_CIE, "]", lin, col);
        case ':': return Token(TokenType::DOS_PUNTOS,   ":", lin, col);
        case ',': return Token(TokenType::COMA,         ",", lin, col);
        case ';': return Token(TokenType::PUNTO_COMA,   ";", lin, col);
        default:
            errManager.agregarError(std::string(1,c), ErrorType::LEXICO,
                std::string("Carácter no reconocido '") + c + "'", lin, col);
            return Token(TokenType::DESCONOCIDO, std::string(1,c), lin, col);
    }
}

// ── Función principal: tokenizartodo el código ─────────────

std::vector<Token> LexicalAnalyzer::tokenizar() {
    std::vector<Token> tokens;

    while (true) {
        saltarEspacios();
        if (esFin()) break;

        char c = actual();

        if (std::isalpha((unsigned char)c) || c == '_') {
            tokens.push_back(leerPalabraOFecha());
        }
        else if (std::isdigit((unsigned char)c)) {
            // Puede ser entero o fecha que empieza con dígito
            tokens.push_back(leerPalabraOFecha());
        }
        else if (c == '"') {
            tokens.push_back(leerCadena());
        }
        else {
            tokens.push_back(leerDelimitador());
        }
    }

    tokens.push_back(Token(TokenType::FIN_ARCHIVO, "EOF", linea, columna));
    return tokens;
}