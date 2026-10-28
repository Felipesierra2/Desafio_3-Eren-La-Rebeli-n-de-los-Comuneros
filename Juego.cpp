#include "Juego.h"
#include "Jugador.h"

#include <QGraphicsScene>
#include <QKeyEvent>
#include <QTimer>

Juego::Juego(QWidget *parent)
    : QGraphicsView(parent),
    escena(new QGraphicsScene(this)),
    jugador(new Jugador()),
    temporizador(new QTimer(this)),
    plataformas(new QGraphicsRectItem*[1])
{
    setWindowTitle("Mi juego 2D");
    setFixedSize(816, 639);

    escena->setSceneRect(0, 0, 800, 600);
    setScene(escena);

    setRenderHint(QPainter::Antialiasing);
    setBackgroundBrush(QBrush(QColor(28, 32, 45)));

    escena->addRect(0, 560, 800, 40,
                    QPen(Qt::NoPen),
                    QBrush(QColor(65, 145, 90)));
    plataformas[0] = escena->addRect(
        300, 500, 180, 20,
        QPen(Qt::NoPen),
        QBrush(QColor(180, 130, 80))
        );

    escena->addItem(jugador);
    jugador->setPos(100,560-45);

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    connect(temporizador, &QTimer::timeout,this,&Juego::actualizarJuego);
    reloj.start();
    temporizador->start(16);
}

void Juego::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        event->accept();
        return;
    }

    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_A:
        izquierdaPresionada = true;
        break;

    case Qt::Key_Right:
    case Qt::Key_D:
        derechaPresionada = true;
        break;

    case Qt::Key_Up:
    case Qt::Key_W:
    case Qt::Key_Space:
        jugador->saltar();
        break;

    default:
        QGraphicsView::keyPressEvent(event);
        return;
    }

    event->accept();
}

void Juego::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        event->accept();
        return;
    }

    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_A:
        izquierdaPresionada = false;
        break;

    case Qt::Key_Right:
    case Qt::Key_D:
        derechaPresionada = false;
        break;

    default:
        QGraphicsView::keyReleaseEvent(event);
        return;
    }

    event->accept();
}

void Juego::actualizarJuego()
{
    qreal deltaTiempo = reloj.restart() / 1000.0;

    deltaTiempo = qMin(deltaTiempo, 0.05);

    const qreal velocidadHorizontal = 250;

    qreal direccionHorizontal = 0;

    if (izquierdaPresionada)
        direccionHorizontal -= 1;

    if (derechaPresionada)
        direccionHorizontal += 1;

    jugador->establecerPlataformas(plataformas, cantidadPlataformas);
    jugador->mover2D(
        direccionHorizontal * velocidadHorizontal * deltaTiempo,
        0
        );
    jugador->actualizarMovimiento(deltaTiempo);
}
