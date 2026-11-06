#ifndef ENTIDADJUEGO_H
#define ENTIDADJUEGO_H

#include <QGraphicsRectItem>

class EntidadJuego : public QGraphicsRectItem
{
public:
    EntidadJuego(const QRectF &rect, int vidaInicial, QGraphicsItem *padre = nullptr);

    virtual ~EntidadJuego() = default;

    virtual void recibirDano(int cantidad, qreal impulsoX);

    virtual void actualizar(qreal deltaTiempo);

    bool estaDerrotado() const;

    int obtenerVida() const;

protected:
    int vida;
    qreal velocidadRetrocesoX;
    static constexpr qreal FRICCION = 900.0;
};

#endif // ENTIDADJUEGO_H
