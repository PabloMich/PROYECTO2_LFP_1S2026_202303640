#pragma once
#include <QMainWindow>
#include <QTextEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QTabWidget>
#include "ErrorManager.h"
#include "Token.h"
#include "ReportGenerator.h"
#include "BoardData.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void cargarArchivo();
    void analizarCodigo();
    void abrirReporte1();
    void abrirReporte2();
    void abrirReporte3();

private:
    // Panel izquierdo
    QTextEdit*    editorCodigo;
    QPushButton*  btnCargar;
    QPushButton*  btnAnalizar;
    QLabel*       lblArchivo;

    // Tabs del panel derecho
    QTabWidget*   tabs;
    QTableWidget* tablaTokens;
    QTableWidget* tablaErrores;

    // Datos del análisis
    std::vector<Token> tokens;
    ErrorManager errManager;

    Tablero tableroActual;

    // Helpers UI
    void setupUI();
    void mostrarTokens();
    void mostrarErrores();
    void limpiarTablas();

    void abrirGraphviz();
};