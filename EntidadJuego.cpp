#include "EntidadJuego.h"
#include <QtMath>

EntidadJuego::EntidadJuego(const QRectF &rect, int vidaInicial,
                           QGraphicsItem *padre)
    : QGraphicsRectItem(rect, padre),
    vida(vidaInicial),
    velocidadRetrocesoX(0)
{
}

void EntidadJuego::recibirDano(int cantidad, qreal impulsoX)
{
    if (cantidad <= 0 || estaDerrotado())
        return;

    vida -= cantidad;

    velocidadRetrocesoX = impulsoX;

    if (vida < 0)
        vida = 0;
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
