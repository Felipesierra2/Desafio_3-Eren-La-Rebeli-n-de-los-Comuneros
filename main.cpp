#include "Juego.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.setMinimumSize(800, 600);
    mainWindow.showMaximized();

    return app.exec();
}
