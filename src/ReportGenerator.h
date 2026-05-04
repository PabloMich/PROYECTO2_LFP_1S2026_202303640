#pragma once
#include <string>
#include "BoardData.h"

class ReportGenerator {
public:
    // Recibe el tablero ya parseado y la ruta donde guardar
    static void generarKanban(const Tablero& tablero, const std::string& rutaSalida);
    static void generarCargaResponsable(const Tablero& tablero, const std::string& rutaSalida);
    static void generarResumenPrioridades(const Tablero& tablero, const std::string& rutaSalida);
    static void generarGraphviz(const Tablero& tablero, const std::string& rutaSalida);
};