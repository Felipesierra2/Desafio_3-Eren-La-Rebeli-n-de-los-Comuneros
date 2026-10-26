#include "Jugador.h"

#include <QBrush>
#include <QColor>
#include <QPen>
#include <QGraphicsScene>
#include <QtMath>

Jugador::Jugador(QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    setRect(0, 0, 35, 45);
    setBrush(QBrush(QColor(80, 170, 255)));
    setPen(QPen(Qt::white, 2));
}

void Jugador::mover2D(qreal desplazamientoX, qreal desplazamientoY)
{
    if (!scene())
        return;

    const QRectF limites = scene()->sceneRect();

    qreal nuevaX = pos().x() + desplazamientoX;
    qreal nuevaY = pos().y() + desplazamientoY;

    const qreal maxX = limites.right() - rect().width();

    nuevaX = qBound(limites.left(), nuevaX, maxX);

    const qreal sueloY = 560 - rect().height();
    nuevaY = qBound(limites.top(), nuevaY, sueloY);

    setPos(nuevaX, nuevaY);

}

void Jugador::saltar()
{
    if (saltando || !scene())
        return;

    const qreal sueloY = 560 - rect().height();

    if (pos().y() < sueloY - 1)
        return;

    yInicial = pos().y();
    velocidadYInicial = -300;

    tiempo = 0;
    saltando = true;
}

void Jugador::actualizarMovimiento(qreal deltaTiempo)
{
    if (!scene() || !saltando)
        return;

    tiempo += deltaTiempo;

    qreal yFinal = yInicial
                   + velocidadYInicial * tiempo
                   + 0.5 * GRAVEDAD * tiempo * tiempo;

    const qreal sueloY = 560 - rect().height();

    if (yFinal >= sueloY) {
        yFinal = sueloY;
        saltando = false;
        tiempo = 0;
    }

    setY(yFinal);

}
