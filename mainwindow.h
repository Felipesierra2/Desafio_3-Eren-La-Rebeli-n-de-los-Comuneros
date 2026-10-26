#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "Juego.h"

#include <QMainWindow>


class QLabel;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QLabel *texto;
    QPushButton *boton;
    QPushButton *botonSalir;
    Juego* juego;
};

#endif
