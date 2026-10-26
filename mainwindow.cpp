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

    // Crear un texto
    texto = new QLabel("¡Hola! Estoy aprendiendo Qt.", central);

    // Crear botones
    boton = new QPushButton("Cambiar texto", central);
    botonSalir = new QPushButton("Salir", central);

    // Organizar los componentes verticalmente
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->addWidget(texto);
    layout->addWidget(boton);
    layout->addWidget(botonSalir);
}

MainWindow::~MainWindow()
{
}
