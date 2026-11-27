#ifndef ENTIDADJUEGO_H
#define ENTIDADJUEGO_H

#include <QGraphicsRectItem>

class EntidadJuego : public QGraphicsRectItem
{
private:
    const std::vector<QGraphicsRectItem*>* columnasEscenario = nullptr;

public:
    EntidadJuego(const QRectF &rect, int vidaInicial, QGraphicsItem *padre = nullptr);

    virtual ~EntidadJuego() = default;

    virtual void recibirDano(int cantidad, qreal impulsoX);

    virtual void actualizar(qreal deltaTiempo);

    void establecerColumnasEscenario(const std::vector<QGraphicsRectItem*>& columnas);

    bool estaDerrotado() const;

    int obtenerVida() const;

protected:
    int vida;
    qreal velocidadRetrocesoX;
    static constexpr qreal FRICCION = 900.0;

    int vidaMaxima;

    void moverHorizontalSeguro(qreal desplazamientoX);

    bool estaEnRetroceso() const;

    QGraphicsRectItem* fondoVida;
    QGraphicsRectItem* barraVida;
};

#endif // ENTIDADJUEGO_H
