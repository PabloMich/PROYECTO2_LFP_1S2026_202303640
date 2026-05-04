#pragma once
#include <string>
#include <vector>

struct Tarea {
    std::string nombre;
    std::string prioridad;   // "ALTA", "MEDIA", "BAJA"
    std::string responsable;
    std::string fecha_limite;
};

struct Columna {
    std::string nombre;
    std::vector<Tarea> tareas;
};

struct Tablero {
    std::string nombre;
    std::vector<Columna> columnas;
};