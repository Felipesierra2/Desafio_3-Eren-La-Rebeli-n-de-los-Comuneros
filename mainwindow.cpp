#include "mainwindow.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Crear un contenedor central
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    juego = new Juego(central);
    juego->setMinimumSize(600,400);

    // Organizar los componentes verticalmente
    QVBoxLayout *layout = new QVBoxLayout(central);
}

MainWindow::~MainWindow()
{
}
