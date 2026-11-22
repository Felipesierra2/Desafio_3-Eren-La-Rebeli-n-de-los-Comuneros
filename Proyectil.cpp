#include "Proyectil.h"

#include <QBrush>
#include <QPen>


Proyectil::Proyectil(qreal direccion)
    : QGraphicsRectItem(),
    direccion(direccion)
{
    setRect(0, 0, 8, 4);

    setBrush(Qt::darkGray);
    setPen(Qt::NoPen);
}

void Proyectil::actualizar(qreal deltaTiempo)
{
    qreal desplazamientoX = direccion * velocidad * deltaTiempo;

    setX(x() + desplazamientoX);
}
