#ifndef JUEGO_H
#define JUEGO_H

#include <QGraphicsView>
#include <QElapsedTimer>


class QGraphicsRectItem;
class QGraphicsScene;
class QKeyEvent;
class QTimer;
class Jugador;

class Juego : public QGraphicsView
{
public:
    explicit Juego(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void actualizarJuego();

    bool izquierdaPresionada = false;
    bool derechaPresionada = false;

    QElapsedTimer reloj;
    QGraphicsScene *escena;
    QGraphicsRectItem **plataformas;
    Jugador *jugador;
    QTimer *temporizador;

    int cantidadPlataformas = 1;
};

#endif // JUEGO_H
