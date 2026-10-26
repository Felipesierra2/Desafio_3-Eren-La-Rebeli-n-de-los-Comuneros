#include "Juego.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.setMinimumSize(700,500);

    mainWindow.show();

    return app.exec();
}
