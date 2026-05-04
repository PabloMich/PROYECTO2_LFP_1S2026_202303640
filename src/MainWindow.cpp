#include "MainWindow.h"
#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>

#include <QDesktopServices>
#include <QUrl>
#include <QDir>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("TaskScript Analyzer");
    setMinimumSize(1100, 650);
    setupUI();
}

void MainWindow::setupUI() {
    // ── Widget central con splitter ──────────────────────────
    QWidget*   central  = new QWidget(this);
    QSplitter* splitter = new QSplitter(Qt::Horizontal, central);

    // ── Panel IZQUIERDO: editor ──────────────────────────────
    QWidget*     panelIzq = new QWidget();
    QVBoxLayout* layIzq   = new QVBoxLayout(panelIzq);

    lblArchivo   = new QLabel("Sin archivo cargado");
    btnCargar    = new QPushButton("Cargar archivo .task");
    btnAnalizar  = new QPushButton("Analizar");
    editorCodigo = new QTextEdit();
    editorCodigo->setPlaceholderText("Pega o carga tu código TaskScript aquí...");
    editorCodigo->setFont(QFont("Courier New", 10));

    // Estilo botones
    btnCargar->setStyleSheet(
        "QPushButton { background:#2E75B6; color:white; padding:6px; border-radius:4px; }"
        "QPushButton:hover { background:#1a5a9a; }");
    btnAnalizar->setStyleSheet(
        "QPushButton { background:#217346; color:white; padding:6px; border-radius:4px; }"
        "QPushButton:hover { background:#145230; }");

    QHBoxLayout* layBtns = new QHBoxLayout();
    layBtns->addWidget(btnCargar);
    layBtns->addWidget(btnAnalizar);

    layIzq->addWidget(lblArchivo);
    layIzq->addLayout(layBtns);
    layIzq->addWidget(editorCodigo);
    panelIzq->setMinimumWidth(420);

    // ── Panel DERECHO: tabs ──────────────────────────────────
    tabs = new QTabWidget();

    // Tab 1: Tokens
    tablaTokens = new QTableWidget(0, 5);
    tablaTokens->setHorizontalHeaderLabels({"#", "Lexema", "Tipo", "Línea", "Columna"});
    tablaTokens->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tablaTokens->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaTokens->setAlternatingRowColors(true);

    // Tab 2: Errores
    tablaErrores = new QTableWidget(0, 6);
    tablaErrores->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tablaErrores->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch); // Descripción se estira
    tablaTokens->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tablaTokens->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch); // Tipo se estira
    tablaErrores->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaErrores->setAlternatingRowColors(true);

    tabs->addTab(tablaTokens,  "Tokens");
    tabs->addTab(tablaErrores, "Errores");

    // ── Splitter une ambos paneles ───────────────────────────
    splitter->addWidget(panelIzq);
    splitter->addWidget(tabs);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 3);

    QHBoxLayout* layMain = new QHBoxLayout(central);
    layMain->addWidget(splitter);
    setCentralWidget(central);

    // Botones de reportes
    QPushButton* btnR1 = new QPushButton("Reporte Kanban");
    QPushButton* btnR2 = new QPushButton("Carga Responsable");
    QPushButton* btnR3 = new QPushButton("Prioridades");

    QString estiloReporte =
        "QPushButton { background:#5D6D7E; color:white; padding:5px; border-radius:4px; }"
        "QPushButton:hover { background:#3d4e5e; }";
    btnR1->setStyleSheet(estiloReporte);
    btnR2->setStyleSheet(estiloReporte);
    btnR3->setStyleSheet(estiloReporte);

    QHBoxLayout* layReportes = new QHBoxLayout();
    layReportes->addWidget(btnR1);
    layReportes->addWidget(btnR2);
    layReportes->addWidget(btnR3);

    // Agrega layReportes al layout del panel derecho
    QWidget* panelDer = new QWidget();
    QVBoxLayout* layDer = new QVBoxLayout(panelDer);
    layDer->addLayout(layReportes);
    layDer->addWidget(tabs);

    splitter->addWidget(panelIzq);
    splitter->addWidget(panelDer);   // <-- panelDer en vez de tabs directamente

    // ── Conexiones ───────────────────────────────────────────
    connect(btnCargar,   &QPushButton::clicked, this, &MainWindow::cargarArchivo);
    connect(btnAnalizar, &QPushButton::clicked, this, &MainWindow::analizarCodigo);

    connect(btnR1, &QPushButton::clicked, this, &MainWindow::abrirReporte1);
    connect(btnR2, &QPushButton::clicked, this, &MainWindow::abrirReporte2);
    connect(btnR3, &QPushButton::clicked, this, &MainWindow::abrirReporte3);
}

// ── Cargar archivo .task ─────────────────────────────────────

void MainWindow::cargarArchivo() {
    QString ruta = QFileDialog::getOpenFileName(
        this, "Abrir archivo TaskScript", "", "TaskScript (*.task);;Todos (*.*)");

    if (ruta.isEmpty()) return;

    QFile archivo(ruta);
    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "No se pudo abrir el archivo.");
        return;
    }

    QTextStream stream(&archivo);
    editorCodigo->setPlainText(stream.readAll());
    lblArchivo->setText("" + ruta);
    archivo.close();
}


// ── Analizar el código ───────────────────────────────────────

void MainWindow::analizarCodigo() {
    limpiarTablas();

    std::string codigo = editorCodigo->toPlainText().toStdString();
    if (codigo.empty()) {
        QMessageBox::information(this, "Aviso", "El editor está vacío.");
        return;
    }

    // Análisis léxico
    errManager.limpiar();
    LexicalAnalyzer lexer(codigo, errManager);
    tokens = lexer.tokenizar();

    // Análisis sintáctico
    SyntaxAnalyzer parser(tokens, errManager);
    parser.parsear();

    tableroActual = parser.getTablero();

    // Mostrar resultados
    mostrarTokens();
    mostrarErrores();

    // Ir a la tab de errores si hay alguno
    if (errManager.hayErrores()) {
        tabs->setCurrentIndex(1);
        setWindowTitle("TaskScript Analyzer" +
            QString::number(errManager.getErrores().size()) + " error(es)");
    } else {
        tabs->setCurrentIndex(0);
        setWindowTitle("TaskScript Analyzer — ✓ Sin errores");
    }
}

// ── Llenar tabla de tokens ───────────────────────────────────

void MainWindow::mostrarTokens() {
    tablaTokens->setRowCount(0);
    int fila = 0;
    for (const auto& t : tokens) {
        if (t.type == TokenType::FIN_ARCHIVO) continue;
        tablaTokens->insertRow(fila);
        tablaTokens->setItem(fila, 0, new QTableWidgetItem(QString::number(fila + 1)));
        tablaTokens->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(t.lexema)));
        tablaTokens->setItem(fila, 2, new QTableWidgetItem(QString::fromStdString(tokenTypeToString(t.type))));
        tablaTokens->setItem(fila, 3, new QTableWidgetItem(QString::number(t.linea)));
        tablaTokens->setItem(fila, 4, new QTableWidgetItem(QString::number(t.columna)));
        fila++;
    }
}

// ── Llenar tabla de errores ──────────────────────────────────

void MainWindow::mostrarErrores() {
    tablaErrores->setRowCount(0);
    int fila = 0;
    for (const auto& e : errManager.getErrores()) {
        tablaErrores->insertRow(fila);
        tablaErrores->setItem(fila, 0, new QTableWidgetItem(QString::number(e.numero)));
        tablaErrores->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(e.lexema)));

        QString tipo = (e.tipo == ErrorType::LEXICO) ? "Léxico" : "Sintáctico";
        auto* itemTipo = new QTableWidgetItem(tipo);
        itemTipo->setForeground(e.tipo == ErrorType::LEXICO ?
            QColor("#c0392b") : QColor("#e67e22"));
        tablaErrores->setItem(fila, 2, itemTipo);

        tablaErrores->setItem(fila, 3, new QTableWidgetItem(QString::fromStdString(e.descripcion)));
        tablaErrores->setItem(fila, 4, new QTableWidgetItem(QString::number(e.linea)));
        tablaErrores->setItem(fila, 5, new QTableWidgetItem(QString::number(e.columna)));
        fila++;
    }
}

void MainWindow::abrirReporte1() {
    ReportGenerator::generarKanban(tableroActual, "reporte_kanban.html");
    QDesktopServices::openUrl(QUrl::fromLocalFile(
        QDir::currentPath() + "/reporte_kanban.html"));
}
void MainWindow::abrirReporte2() {
    ReportGenerator::generarCargaResponsable(tableroActual, "reporte_responsable.html");
    QDesktopServices::openUrl(QUrl::fromLocalFile(
        QDir::currentPath() + "/reporte_responsable.html"));
}
void MainWindow::abrirReporte3() {
    ReportGenerator::generarResumenPrioridades(tableroActual, "reporte_prioridades.html");
    QDesktopServices::openUrl(QUrl::fromLocalFile(
        QDir::currentPath() + "/reporte_prioridades.html"));
}

// ── Limpiar ──────────────────────────────────────────────────

void MainWindow::limpiarTablas() {
    tablaTokens->setRowCount(0);
    tablaErrores->setRowCount(0);
}