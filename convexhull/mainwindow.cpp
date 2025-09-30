#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Beri judul pada window
    setWindowTitle("Visualisasi Convex Hull");

    // Sambungkan signal clicked() dari tombol runButton ke slot computeConvexHull() di canvasWidget
    connect(ui->runButton, &QPushButton::clicked, ui->canvasWidget, &CanvasWidget::computeConvexHull);

    // Sambungkan signal clicked() dari tombol clearButton ke slot clearCanvas() di canvasWidget
    connect(ui->clearButton, &QPushButton::clicked, ui->canvasWidget, &CanvasWidget::clearCanvas);
}

MainWindow::~MainWindow()
{
    delete ui;
}
