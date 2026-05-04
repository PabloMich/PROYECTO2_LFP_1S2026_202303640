#include "ReportGenerator.h"
#include <fstream>
#include <map>
#include <sstream>

// ── CSS compartido ───────────────────────────────────────────
static std::string cssBase() {
    return R"(
    <style>
      body { font-family: Arial, sans-serif; background:#f0f2f5; margin:0; padding:20px; }
      h1   { color:#2E75B6; }
      h2   { color:#333; }
    </style>)";
}

// ── Reporte 1: Tablero Kanban ────────────────────────────────
void ReportGenerator::generarKanban(const Tablero& tablero, const std::string& ruta) {
    std::ofstream f(ruta);

    auto colorPrioridad = [](const std::string& p) -> std::string {
        if (p == "ALTA")  return "#e74c3c";
        if (p == "MEDIA") return "#f39c12";
        return "#27ae60";
    };

    f << "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
      << "<title>Tablero Kanban</title>" << cssBase() << R"(
    <style>
      .board   { display:flex; gap:16px; flex-wrap:wrap; }
      .column  { background:#fff; border-radius:8px; padding:12px;
                 min-width:220px; flex:1; box-shadow:0 2px 6px rgba(0,0,0,.15); }
      .col-hdr { font-weight:bold; color:#2E75B6; border-bottom:2px solid #2E75B6;
                 padding-bottom:6px; margin-bottom:10px; }
      .card    { background:#f9f9f9; border-radius:6px; padding:10px;
                 margin-bottom:8px; border-left:4px solid #ccc; }
      .badge   { display:inline-block; color:#fff; padding:2px 8px;
                 border-radius:4px; font-size:12px; margin-bottom:4px; }
      .resp    { font-size:12px; color:#555; margin-top:4px; }
      .fecha   { font-size:12px; color:#777; }
    </style></head><body>)";

    f << "<h1>Tablero: " << tablero.nombre << "</h1><div class='board'>";

    for (const auto& col : tablero.columnas) {
        f << "<div class='column'>"
          << "<div class='col-hdr'>" << col.nombre
          << " (" << col.tareas.size() << ")</div>";

        for (const auto& t : col.tareas) {
            std::string color = colorPrioridad(t.prioridad);
            f << "<div class='card' style='border-left-color:" << color << ";'>"
              << "<strong>" << t.nombre << "</strong><br>"
              << "<span class='badge' style='background:" << color << ";'>"
              << t.prioridad << "</span>"
              << "<div class='fecha'>Fecha: " << t.fecha_limite << "</div>"
              << "<div class='resp'>👤 " << t.responsable << "</div>"
              << "</div>";
        }
        f << "</div>";
    }

    f << "</div></body></html>";
}

// ── Reporte 2: Carga por Responsable ────────────────────────
void ReportGenerator::generarCargaResponsable(const Tablero& tablero, const std::string& ruta) {
    // Contar tareas por responsable
    struct Carga { int total=0, alta=0, media=0, baja=0; };
    std::map<std::string, Carga> cargas;
    int totalGlobal = 0;

    for (const auto& col : tablero.columnas)
        for (const auto& t : col.tareas) {
            cargas[t.responsable].total++;
            if (t.prioridad == "ALTA")       cargas[t.responsable].alta++;
            else if (t.prioridad == "MEDIA") cargas[t.responsable].media++;
            else                              cargas[t.responsable].baja++;
            totalGlobal++;
        }

    std::ofstream f(ruta);
    f << "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
      << "<title>Carga por Responsable</title>" << cssBase() << R"(
    <style>
      table { width:100%; border-collapse:collapse; background:#fff;
              border-radius:8px; overflow:hidden; box-shadow:0 2px 6px rgba(0,0,0,.15); }
      th    { background:#2E75B6; color:#fff; padding:10px; text-align:left; }
      td    { padding:9px 10px; border-bottom:1px solid #eee; }
      tr:hover td { background:#f0f7ff; }
      .bar-bg  { background:#ddd; border-radius:4px; height:16px; width:100%; }
      .bar-fill{ background:#2E75B6; border-radius:4px; height:16px; }
    </style></head><body>)";

    f << "<h1>Carga por Responsable</h1>"
      << "<table><tr><th>Responsable</th><th>Total</th>"
      << "<th>Alta</th><th>Media</th><th>Baja</th><th>Distribución</th></tr>";

    for (const auto& [nombre, c] : cargas) {
        int pct = totalGlobal > 0 ? (c.total * 100 / totalGlobal) : 0;
        f << "<tr><td>" << nombre << "</td>"
          << "<td>" << c.total << "</td>"
          << "<td>" << c.alta  << "</td>"
          << "<td>" << c.media << "</td>"
          << "<td>" << c.baja  << "</td>"
          << "<td><div class='bar-bg'><div class='bar-fill' style='width:"
          << pct << "%;'></div></div>" << pct << "%</td></tr>";
    }

    f << "</table></body></html>";
}

// ── Reporte 3: Resumen de Prioridades ───────────────────────
void ReportGenerator::generarResumenPrioridades(const Tablero& tablero, const std::string& ruta) {
    int alta=0, media=0, baja=0;
    for (const auto& col : tablero.columnas)
        for (const auto& t : col.tareas) {
            if (t.prioridad == "ALTA")       alta++;
            else if (t.prioridad == "MEDIA") media++;
            else                              baja++;
        }
    int total = alta + media + baja;

    auto pct = [&](int n) { return total > 0 ? n * 100 / total : 0; };

    std::ofstream f(ruta);
    f << "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
      << "<title>Resumen de Prioridades</title>" << cssBase() << R"(
    <style>
      .cards { display:flex; gap:20px; flex-wrap:wrap; margin-top:16px; }
      .card  { flex:1; min-width:160px; border-radius:10px; padding:20px;
               color:#fff; text-align:center; box-shadow:0 3px 8px rgba(0,0,0,.2); }
      .num   { font-size:48px; font-weight:bold; }
      .lbl   { font-size:16px; margin-top:4px; }
      .pct   { font-size:13px; opacity:.85; }
    </style></head><body>)";

    f << "<h1>Resumen de Prioridades — " << tablero.nombre << "</h1>"
      << "<p>Total de tareas: <strong>" << total << "</strong></p>"
      << "<div class='cards'>"
      << "<div class='card' style='background:#e74c3c;'>"
      << "<div class='num'>" << alta  << "</div><div class='lbl'>ALTA</div>"
      << "<div class='pct'>" << pct(alta)  << "% del total</div></div>"
      << "<div class='card' style='background:#f39c12;'>"
      << "<div class='num'>" << media << "</div><div class='lbl'>MEDIA</div>"
      << "<div class='pct'>" << pct(media) << "% del total</div></div>"
      << "<div class='card' style='background:#27ae60;'>"
      << "<div class='num'>" << baja  << "</div><div class='lbl'>BAJA</div>"
      << "<div class='pct'>" << pct(baja)  << "% del total</div></div>"
      << "</div></body></html>";
}

// ── Graphviz DOT ─────────────────────────────────────────────
void ReportGenerator::generarGraphviz(const Tablero& tablero, const std::string& ruta) {
    std::ofstream f(ruta);
    int id = 0;

    f << "digraph ArbolDerivacion {\n"
      << "  rankdir=TB;\n"
      << "  node [shape=box, style=filled, fontname=\"Arial\"];\n\n";

    // Nodo raíz
    f << "  n" << id << " [label=\"<programa>\", fillcolor=\"#2E75B6\", fontcolor=white];\n";
    int raiz = id++;

    // TABLERO
    f << "  n" << id << " [label=\"TABLERO\", fillcolor=\"#D6EAF8\"];\n";
    f << "  n" << raiz << " -> n" << id++ << ";\n";
    f << "  n" << id << " [label=\"\\\"" << tablero.nombre << "\\\"\", fillcolor=\"#D6EAF8\"];\n";
    f << "  n" << raiz << " -> n" << id++ << ";\n";

    // Nodo <columnas>
    f << "  n" << id << " [label=\"<columnas>\", fillcolor=\"#AED6F1\"];\n";
    int nColumnas = id++;
    f << "  n" << raiz << " -> n" << nColumnas << ";\n";

    for (const auto& col : tablero.columnas) {
        f << "  n" << id << " [label=\"<columna>\", fillcolor=\"#AED6F1\"];\n";
        int nCol = id++;
        f << "  n" << nColumnas << " -> n" << nCol << ";\n";

        f << "  n" << id << " [label=\"COLUMNA\", fillcolor=\"#D6EAF8\"];\n";
        f << "  n" << nCol << " -> n" << id++ << ";\n";
        f << "  n" << id << " [label=\"\\\"" << col.nombre << "\\\"\", fillcolor=\"#D6EAF8\"];\n";
        f << "  n" << nCol << " -> n" << id++ << ";\n";

        f << "  n" << id << " [label=\"<tareas>\", fillcolor=\"#AED6F1\"];\n";
        int nTareas = id++;
        f << "  n" << nCol << " -> n" << nTareas << ";\n";

        for (const auto& t : col.tareas) {
            f << "  n" << id << " [label=\"<tarea>\", fillcolor=\"#AED6F1\"];\n";
            int nTarea = id++;
            f << "  n" << nTareas << " -> n" << nTarea << ";\n";

            f << "  n" << id << " [label=\"\\\"" << t.nombre << "\\\"\", fillcolor=\"#FDFEFE\"];\n";
            f << "  n" << nTarea << " -> n" << id++ << ";\n";
            f << "  n" << id << " [label=\"" << t.prioridad << "\", fillcolor=\"#FDFEFE\"];\n";
            f << "  n" << nTarea << " -> n" << id++ << ";\n";
            f << "  n" << id << " [label=\"" << t.responsable << "\", fillcolor=\"#FDFEFE\"];\n";
            f << "  n" << nTarea << " -> n" << id++ << ";\n";
        }
    }

    f << "}\n";
}