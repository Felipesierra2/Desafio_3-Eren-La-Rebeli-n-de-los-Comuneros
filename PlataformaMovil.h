#ifndef PLATAFORMAMOVIL_H
#define PLATAFORMAMOVIL_H

#include <qgraphicsitem.h>

class PlataformaMovil: public QGraphicsRectItem
{
private:
    qreal posicionEquilibrioY;
    qreal amplitud;
    qreal frecuenciaAngular;
    qreal tiempo;
    qreal desplazamientoY;
public:
    PlataformaMovil(qreal x, qreal y,
                    qreal ancho, qreal alto,
                    qreal amplitud, qreal periodo);

    void actualizar(qreal deltaTiempo);

    qreal obtenerDesplazamientoY() const;

};

#endif // PLATAFORMAMOVIL_H
