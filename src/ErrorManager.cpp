#include "ErrorManager.h"

void ErrorManager::agregarError(const std::string& lexema, ErrorType tipo,
                                 const std::string& descripcion, int linea, int columna) {
    errores.push_back({ ++contador, lexema, tipo, descripcion, linea, columna });
}

bool ErrorManager::hayErrores() const {
    return !errores.empty();
}

const std::vector<Error>& ErrorManager::getErrores() const {
    return errores;
}

void ErrorManager::limpiar() {
    errores.clear();
    contador = 0;
}