#include "EntidadJuego.h"
#include <QtMath>
#include <QDebug>
#include <QBrush>
#include <QPen>
#include <qgraphicsscene.h>

EntidadJuego::EntidadJuego(const QRectF &rect, int vidaInicial,
                           QGraphicsItem *padre)
    : QGraphicsRectItem(rect, padre),
    vida(vidaInicial),
    vidaMaxima(vidaInicial),
    velocidadRetrocesoX(0),
    fondoVida(nullptr),
    barraVida(nullptr)
{
    fondoVida = new QGraphicsRectItem(0, -10, 35, 5, this);
    fondoVida->setBrush(Qt::black);
    fondoVida->setPen(Qt::NoPen);

    barraVida = new QGraphicsRectItem(0, -10, 35, 5, this);
    barraVida->setBrush(Qt::green);
    barraVida->setPen(Qt::NoPen);
}

void EntidadJuego::recibirDano(int cantidad, qreal impulsoX)
{
    if (cantidad <= 0 || estaDerrotado())
        return;

    qDebug() << "Vida antes:" << vida;

    vida -= cantidad;

    velocidadRetrocesoX = impulsoX;

    if (vida < 0)
        vida = 0;

    qreal porcentajeVida = static_cast<qreal>(vida) / vidaMaxima;

    barraVida->setRect(
        0, -10,
        35 * porcentajeVida,
        5
        );

    if (estaDerrotado()) {
        setVisible(false);
    }

    if (porcentajeVida > 0.7) {
        barraVida->setBrush(Qt::green);
    }
    else if (porcentajeVida > 0.4) {
        barraVida->setBrush(Qt::yellow);
    }
    else {
        barraVida->setBrush(Qt::red);
    }

    qDebug() << "Vida despues:" << vida;
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

    moverHorizontalSeguro(desplazamientoX);

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

void EntidadJuego::moverHorizontalSeguro(qreal desplazamientoX)
{
    if (!scene() || desplazamientoX == 0)
        return;

    const QRectF limitesEscena = scene()->sceneRect();
    const QRectF limitesEntidad = mapRectToScene(rect());

    const qreal desplazamientoMinimo =
        limitesEscena.left() - limitesEntidad.left();

    const qreal desplazamientoMaximo =
        limitesEscena.right() - limitesEntidad.right();

    desplazamientoX = qBound(
        desplazamientoMinimo,
        desplazamientoX,
        desplazamientoMaximo
        );

    if (columnasEscenario)
    {
        for (const QGraphicsRectItem* columna : *columnasEscenario)
        {
            if (!columna)
                continue;

            const QRectF limitesColumna =
                columna->mapRectToScene(columna->rect());

            if (limitesEntidad.bottom() <= limitesColumna.top() ||
                limitesEntidad.top() >= limitesColumna.bottom())
            {
                continue;
            }

            if (desplazamientoX > 0 &&
                limitesEntidad.right() <= limitesColumna.left())
            {
                desplazamientoX = qMin(
                    desplazamientoX,
                    limitesColumna.left() - limitesEntidad.right()
                    );
            }

            else if (desplazamientoX < 0 &&
                     limitesEntidad.left() >= limitesColumna.right())
            {
                desplazamientoX = qMax(
                    desplazamientoX,
                    limitesColumna.right() - limitesEntidad.left()
                    );
            }
        }
    }

    moveBy(desplazamientoX, 0);
}

void EntidadJuego::establecerColumnasEscenario(
    const std::vector<QGraphicsRectItem*>& columnas)
{
    columnasEscenario = &columnas;
}

bool EntidadJuego::estaEnRetroceso() const
{
    return qAbs(velocidadRetrocesoX) >= 0.1;
}
