#include "Token.h"

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::TABLERO:       return "TABLERO";
        case TokenType::COLUMNA:       return "COLUMNA";
        case TokenType::TAREA:         return "TAREA";
        case TokenType::PRIORIDAD:     return "PRIORIDAD";
        case TokenType::RESPONSABLE:   return "RESPONSABLE";
        case TokenType::FECHA_LIMITE:  return "FECHA_LIMITE";
        case TokenType::ALTA:          return "ALTA";
        case TokenType::MEDIA:         return "MEDIA";
        case TokenType::BAJA:          return "BAJA";
        case TokenType::CADENA:        return "CADENA";
        case TokenType::ENTERO:        return "ENTERO";
        case TokenType::FECHA:         return "FECHA";
        case TokenType::LLAVE_ABR:     return "LLAVE_ABR";
        case TokenType::LLAVE_CIE:     return "LLAVE_CIE";
        case TokenType::CORCHETE_ABR:  return "CORCHETE_ABR";
        case TokenType::CORCHETE_CIE:  return "CORCHETE_CIE";
        case TokenType::DOS_PUNTOS:    return "DOS_PUNTOS";
        case TokenType::COMA:          return "COMA";
        case TokenType::PUNTO_COMA:    return "PUNTO_COMA";
        case TokenType::FIN_ARCHIVO:   return "FIN_ARCHIVO";
        default:                       return "DESCONOCIDO";
    }
}