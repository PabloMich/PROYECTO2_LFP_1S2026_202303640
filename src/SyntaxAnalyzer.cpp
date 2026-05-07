#include "SyntaxAnalyzer.h"

SyntaxAnalyzer::SyntaxAnalyzer(const std::vector<Token>& tokens, ErrorManager& errManager)
    : tokens(tokens), pos(0), errManager(errManager) {}

bool SyntaxAnalyzer::esFin() const {
    return pos >= (int)tokens.size() ||
           tokens[pos].type == TokenType::FIN_ARCHIVO;
}

const Token& SyntaxAnalyzer::actual() const { return tokens[pos]; }

bool SyntaxAnalyzer::verificar(TokenType tipo) const {
    return !esFin() && actual().type == tipo;
}

void SyntaxAnalyzer::avanzar() {
    if (!esFin()) pos++;
}

const Token& SyntaxAnalyzer::consumir(TokenType esperado, const std::string& mensaje) {
    if (verificar(esperado)) {
        const Token& t = actual();
        avanzar();
        return t;
    }
    const Token& t = actual();
    errManager.agregarError(t.lexema, ErrorType::SINTACTICO,
        mensaje + " (se encontró '" + t.lexema + "')", t.linea, t.columna);
    return t;
}

// <programa> ::= TABLERO CADENA "{" <columnas> "}" ";"
void SyntaxAnalyzer::parsePrograma() {
    consumir(TokenType::TABLERO,   "Se esperaba 'TABLERO'");
    const Token& nombre = consumir(TokenType::CADENA, "Se esperaba nombre del tablero");
    tablero.nombre = nombre.lexema;
    consumir(TokenType::LLAVE_ABR, "Se esperaba '{'");
    parseColumnas();
    consumir(TokenType::LLAVE_CIE, "Se esperaba '}'");
    consumir(TokenType::PUNTO_COMA,"Se esperaba ';'");
}

// <columnas> ::= <columna> <columnas> | <columna>
void SyntaxAnalyzer::parseColumnas() {
    parseColumna();
    while (verificar(TokenType::COLUMNA)) {
        parseColumna();
    }
}

// <columna> ::= COLUMNA CADENA "{" <tareas> "}" ";"
void SyntaxAnalyzer::parseColumna() {
    consumir(TokenType::COLUMNA,    "Se esperaba 'COLUMNA'");
    const Token& nombre = consumir(TokenType::CADENA, "Se esperaba nombre de columna");
    consumir(TokenType::LLAVE_ABR,  "Se esperaba '{'");

    // Agregar la columna AL TABLERO ANTES de parsear las tareas
    // para que parseTarea() pueda usar tablero.columnas.back()
    Columna col;
    col.nombre = nombre.lexema;
    tablero.columnas.push_back(col);

    if (!verificar(TokenType::LLAVE_CIE)) {
        parseTareas();
    }

    consumir(TokenType::LLAVE_CIE,  "Se esperaba '}'");
    consumir(TokenType::PUNTO_COMA, "Se esperaba ';'");
}

// <tareas> ::= <tarea> "," <tareas> | <tarea>
void SyntaxAnalyzer::parseTareas() {
    parseTarea();
    while (verificar(TokenType::COMA)) {
        avanzar();
        if (verificar(TokenType::LLAVE_CIE)) break;
        parseTarea();
    }
}

// <tarea> ::= tarea ":" CADENA "[" <atributos> "]"
void SyntaxAnalyzer::parseTarea() {
    consumir(TokenType::TAREA,        "Se esperaba 'tarea'");
    consumir(TokenType::DOS_PUNTOS,   "Se esperaba ':' después de 'tarea'");
    const Token& nombre = consumir(TokenType::CADENA, "Se esperaba nombre de la tarea");
    consumir(TokenType::CORCHETE_ABR, "Se esperaba '['");

    Tarea t;
    t.nombre = nombre.lexema;
    parseAtributos(t);

    consumir(TokenType::CORCHETE_CIE, "Se esperaba ']'");

    // Agregar a la última columna
    if (!tablero.columnas.empty()) {
        tablero.columnas.back().tareas.push_back(t);
    }
}

// <atributos> ::= <atributo> "," <atributos> | <atributo>
void SyntaxAnalyzer::parseAtributos(Tarea& tarea) {
    parseAtributo(tarea);
    while (!verificar(TokenType::CORCHETE_CIE) && !esFin()) {
        if (verificar(TokenType::COMA)) {
            avanzar(); // consume ','
            if (verificar(TokenType::CORCHETE_CIE)) break;
            parseAtributo(tarea);
        } else {
            // Falta la coma entre atributos
            const Token& t = actual();
            errManager.agregarError(
                tokens[pos-1].lexema,   // el token anterior (el que necesitaba coma)
                ErrorType::SINTACTICO,
                "Se esperaba COMA después de \"" + tokens[pos-1].lexema +
                "\", se encontró " + t.lexema,
                tokens[pos-1].linea, tokens[pos-1].columna);
            // Intentar continuar parseando el siguiente atributo
            if (verificar(TokenType::PRIORIDAD) ||
                verificar(TokenType::RESPONSABLE) ||
                verificar(TokenType::FECHA_LIMITE)) {
                parseAtributo(tarea);
                } else {
                    break;
                }
        }
    }
}

// <atributo> ::= prioridad ":" ALTA|MEDIA|BAJA
//              | responsable ":" CADENA
//              | fecha_limite ":" FECHA
void SyntaxAnalyzer::parseAtributo(Tarea& tarea) {
    if (verificar(TokenType::PRIORIDAD)) {
        avanzar();
        consumir(TokenType::DOS_PUNTOS, "Se esperaba ':' después de 'prioridad'");
        if (verificar(TokenType::ALTA) || verificar(TokenType::MEDIA) || verificar(TokenType::BAJA)) {
            tarea.prioridad = actual().lexema;
            avanzar();
        } else {
            const Token& t = actual();
            errManager.agregarError(t.lexema, ErrorType::SINTACTICO,
                "Se esperaba ALTA, MEDIA o BAJA", t.linea, t.columna);
        }
    }
    else if (verificar(TokenType::RESPONSABLE)) {
        avanzar();
        consumir(TokenType::DOS_PUNTOS, "Se esperaba ':' después de 'responsable'");
        const Token& t = consumir(TokenType::CADENA, "Se esperaba nombre del responsable");
        tarea.responsable = t.lexema;
    }
    else if (verificar(TokenType::FECHA_LIMITE)) {
        avanzar();
        consumir(TokenType::DOS_PUNTOS, "Se esperaba ':' después de 'fecha_limite'");
        const Token& t = consumir(TokenType::FECHA, "Se esperaba fecha AAAA-MM-DD");
        tarea.fecha_limite = t.lexema;
    }
    else {
        const Token& t = actual();
        errManager.agregarError(t.lexema, ErrorType::SINTACTICO,
            "Atributo no reconocido", t.linea, t.columna);
        avanzar();
    }
}

void SyntaxAnalyzer::parsear() {
    parsePrograma();
    if (!esFin()) {
        const Token& t = actual();
        errManager.agregarError(t.lexema, ErrorType::SINTACTICO,
            "Tokens inesperados después del fin del programa", t.linea, t.columna);
    }
}