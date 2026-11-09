#include "Juego.h"
#include "Jugador.h"
#include "Soldado.h"

#include <QGraphicsScene>
#include <QKeyEvent>
#include <QTimer>
#include <QtMath>

Juego::Juego(QWidget *parent)
    : QGraphicsView(parent),
    escena(new QGraphicsScene(this)),
    jugador(new Jugador()),
    temporizador(new QTimer(this))
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
    plataformas.push_back(escena->addRect(300, 500, 180, 20,
                                          QPen(Qt::NoPen),
                                          QBrush(QColor(180, 130, 80))));

    escena->addItem(jugador);
    jugador->setPos(250,560-45);

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    connect(temporizador, &QTimer::timeout,this,&Juego::actualizarJuego);
    reloj.start();
    temporizador->start(16);

    Soldado* soldado = new Soldado();

    soldado->setPos(100, 515);

    scene()->addItem(soldado);

    soldados.append(soldado);
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
        jugador->saltar();
        break;

    case Qt::Key_Space:
        if (!event->isAutoRepeat())
            jugador->atacar();
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

    jugador->establecerColumnas(columnas);
    jugador->establecerPlataformas(plataformas);
    jugador->movimientoHorizontal(direccionHorizontal * velocidadHorizontal * deltaTiempo);
    jugador->actualizarMovimiento(deltaTiempo);

    for (Soldado* soldado : std::as_const(soldados))
    {
        QRectF limitesEren = jugador->sceneBoundingRect();
        QRectF eren = jugador->sceneBoundingRect();
        QRectF enemigo = soldado->sceneBoundingRect();

        limitesEren.adjust(-4, -0, 4, 0);

        qreal distanciaHorizontal = 0;

        if (eren.right() < enemigo.left())
            distanciaHorizontal = enemigo.left() - eren.right();
        else if (enemigo.right() < eren.left())
            distanciaHorizontal = eren.left() - enemigo.right();

        bool superposicionVertical =
            eren.bottom() > enemigo.top() &&
            eren.top() < enemigo.bottom();
    }

    for (auto it = soldados.begin(); it != soldados.end(); ){
        Soldado* soldado = *it;

        if (soldado->estaDerrotado())
        {
            jugador->desaparecerSoldado(soldado);

            scene()->removeItem(soldado);
            delete soldado;

            it = soldados.erase(it);
        }
        else
        {
            soldado->actualizar(deltaTiempo);
            ++it;
        }
    }
}
