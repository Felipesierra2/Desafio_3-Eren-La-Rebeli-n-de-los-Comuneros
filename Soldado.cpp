#include "Soldado.h"
#include <QBrush>

Soldado::Soldado()
    : EntidadJuego(QRectF(0, 0, 35, 45), VIDA_INICIAL)
{
    setBrush(Qt::red);
}
