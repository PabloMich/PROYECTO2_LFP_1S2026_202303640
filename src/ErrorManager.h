#pragma once
#include <string>
#include <vector>

enum class ErrorType { LEXICO, SINTACTICO };

struct Error {
    int numero;
    std::string lexema;
    ErrorType tipo;
    std::string descripcion;
    int linea;
    int columna;
};

class ErrorManager {
public:
    void agregarError(const std::string& lexema, ErrorType tipo,
                      const std::string& descripcion, int linea, int columna);
    bool hayErrores() const;
    const std::vector<Error>& getErrores() const;
    void limpiar();

private:
    std::vector<Error> errores;
    int contador = 0;
};