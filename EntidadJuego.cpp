#include "EntidadJuego.h"
#include <QtMath>
#include <QDebug>
#include <QBrush>
#include <QPen>

EntidadJuego::EntidadJuego(const QRectF &rect, int vidaInicial,
                           QGraphicsItem *padre)
    : QGraphicsRectItem(rect, padre),
    vida(vidaInicial),
    vidaMaxima(vidaInicial),
    velocidadRetrocesoX(0),
    fondoVida(nullptr),
    barraVida(nullptr)
{
    fondoVida = new QGraphicsRectItem(0, -10, 35, 5, this);
    fondoVida->setBrush(Qt::black);
    fondoVida->setPen(Qt::NoPen);

    barraVida = new QGraphicsRectItem(0, -10, 35, 5, this);
    barraVida->setBrush(Qt::green);
    barraVida->setPen(Qt::NoPen);
}

void EntidadJuego::recibirDano(int cantidad, qreal impulsoX)
{
    if (cantidad <= 0 || estaDerrotado())
        return;

    qDebug() << "Vida antes:" << vida;

    vida -= cantidad;

    velocidadRetrocesoX = impulsoX;

    if (vida < 0)
        vida = 0;

    qreal porcentajeVida = static_cast<qreal>(vida) / vidaMaxima;

    barraVida->setRect(
        0, -10,
        35 * porcentajeVida,
        5
        );

    if (estaDerrotado()) {
        setVisible(false);
    }

    if (porcentajeVida > 0.7) {
        barraVida->setBrush(Qt::green);
    }
    else if (porcentajeVida > 0.4) {
        barraVida->setBrush(Qt::yellow);
    }
    else {
        barraVida->setBrush(Qt::red);
    }

    qDebug() << "Vida despues:" << vida;
}

bool EntidadJuego::estaDerrotado() const
{
    return vida <= 0;
}

int EntidadJuego::obtenerVida() const
{
    return vida;
}

void EntidadJuego::actualizar(qreal deltaTiempo)
{
    if (qAbs(velocidadRetrocesoX) < 0.1)
    {
        velocidadRetrocesoX = 0;
        return;
    }

    qreal desplazamientoX = velocidadRetrocesoX * deltaTiempo;

    moveBy(desplazamientoX, 0);

    if (velocidadRetrocesoX > 0)
        velocidadRetrocesoX -= FRICCION * deltaTiempo;
    else
        velocidadRetrocesoX += FRICCION * deltaTiempo;

    if ((desplazamientoX > 0 && velocidadRetrocesoX < 0) ||
        (desplazamientoX < 0 && velocidadRetrocesoX > 0))
    {
        velocidadRetrocesoX = 0;
    }
}
