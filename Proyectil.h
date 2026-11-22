#ifndef PROYECTIL_H
#define PROYECTIL_H

#include <qgraphicsitem.h>

class Proyectil : public QGraphicsRectItem
{
private:
    qreal direccion;
    qreal velocidad = 400;

public:
    Proyectil(qreal direccion);

    void actualizar(qreal deltaTiempo);
};

#endif // PROYECTIL_H
