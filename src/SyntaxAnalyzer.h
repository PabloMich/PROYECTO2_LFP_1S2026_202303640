#pragma once
#include <vector>
#include "Token.h"
#include "ErrorManager.h"
#include "BoardData.h"

class SyntaxAnalyzer {
public:
    SyntaxAnalyzer(const std::vector<Token>& tokens, ErrorManager& errManager);
    void parsear(); // punto de entrada
    const Tablero& getTablero() const {return tablero; }

private:
    std::vector<Token> tokens;
    int pos;
    ErrorManager& errManager;
    Tablero tablero;

    // Token actual y avance
    const Token& actual() const;
    const Token& consumir(TokenType esperado, const std::string& mensaje);
    bool verificar(TokenType tipo) const;
    void avanzar();
    bool esFin() const;

    // Una función por cada regla de la gramática
    void parsePrograma();
    void parseColumnas();
    void parseColumna();
    void parseTareas();
    void parseTarea();
    void parseAtributos();
    void parseAtributo();
    void parseAtributos(Tarea& tarea);
    void parseAtributo(Tarea& tarea);
};