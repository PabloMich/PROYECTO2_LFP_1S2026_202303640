# TaskScript Analyzer
**Curso:** Lenguajes Formales y de Programación  
**Universidad San Carlos de Guatemala**

## Descripción
Analizador léxico y sintáctico para el lenguaje TaskScript,
orientado a gestión de tareas tipo Kanban.

## Dependencias
- C++17 o superior
- Qt 6.x (o Qt 5.15)
- CMake 3.16+
- MinGW (Windows)

## Compilación
```bash
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.7.0/mingw_64"
cmake --build .
```

## Uso
1. Ejecutar `TaskScript.exe`
2. Click en **Cargar archivo .task**
3. Click en **Analizar**
4. Ver tokens y errores en las tablas
5. Generar reportes HTML con los botones superiores

## Estructura
```
Proyecto2/
├── src/       # Código fuente C++
├── docs/      # Manuales técnico y de usuario
├── tests/     # Casos de prueba .task
├── examples/  # Ejemplos válidos
└── README.md
```