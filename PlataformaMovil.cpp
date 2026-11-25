#include "PlataformaMovil.h"
#include <qbrush.h>
#include <QtMath>
#include <QPen>



PlataformaMovil::PlataformaMovil(qreal x, qreal y,
                                 qreal ancho, qreal alto,
                                 qreal amplitud, qreal periodo)
    : QGraphicsRectItem(0, 0, ancho, alto),
    posicionEquilibrioY(y),
    amplitud(amplitud),
    frecuenciaAngular(2.0 * M_PI / periodo),
    tiempo(0),
    desplazamientoY(0)
{
    setPos(x, y);
    setBrush(QColor(180, 130, 70));
    setPen(Qt::NoPen);
}

void PlataformaMovil::actualizar(qreal deltaTiempo)
{
    qreal posicionAnteriorY = pos().y();

    tiempo += deltaTiempo;

    qreal nuevaY = posicionEquilibrioY
                   + amplitud * qSin(frecuenciaAngular * tiempo);

    desplazamientoY = nuevaY - posicionAnteriorY;

    setY(nuevaY);
}

qreal PlataformaMovil::obtenerDesplazamientoY() const
{
    return desplazamientoY;
}
