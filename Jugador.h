#ifndef JUGADOR_H
#define JUGADOR_H

#include <QGraphicsRectItem>

class Jugador : public QGraphicsRectItem
{
private:
    qreal velocidadYInicial = -300;
    const qreal GRAVEDAD = 500;
    qreal yInicial = 0;
    qreal tiempo = 0;
    bool saltando = false;

    QGraphicsRectItem **plataformas = nullptr;
    QGraphicsRectItem *plataformaActual = nullptr;
    int cantidadPlataformas = 0;
    bool enSuelo = true;
public:
    explicit Jugador(QGraphicsItem *parent = nullptr);

    void saltar();
    void actualizarMovimiento(qreal deltaTiempo);
    void mover2D(qreal desplazamientoX, qreal desplazamientoY);

    void establecerPlataformas(QGraphicsRectItem **plataformas,int cantidadPlataformas);

protected:
};


#endif // JUGADOR_H
