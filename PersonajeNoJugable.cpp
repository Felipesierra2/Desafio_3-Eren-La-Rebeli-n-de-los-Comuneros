#include "PersonajeNoJugable.h"
#include <QBrush>
#include <QPen>

PersonajeNoJugable::PersonajeNoJugable()
    : QGraphicsRectItem(0, 0, 35, 45)
{
    setBrush(Qt::darkGreen);
    setPen(Qt::NoPen);
}
