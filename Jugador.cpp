#include "Jugador.h"

#include <QBrush>
#include <QColor>
#include <QPen>
#include <QGraphicsScene>
#include <QtMath>

Jugador::Jugador(QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    setRect(0, 0, 35, 45);
    setBrush(QBrush(QColor(80, 170, 255)));
    setPen(QPen(Qt::white, 2));
}

void Jugador::establecerPlataformas(QGraphicsRectItem **plataformas,int cantidadPlataformas)
{
    this->plataformas = plataformas;
    this->cantidadPlataformas = cantidadPlataformas;
}

void Jugador::mover2D(qreal desplazamientoX, qreal desplazamientoY)
{
    if (!scene())
        return;

    const QRectF limites = scene()->sceneRect();

    qreal nuevaX = pos().x() + desplazamientoX;
    qreal nuevaY = pos().y() + desplazamientoY;

    const qreal maxX = limites.right() - rect().width();

    nuevaX = qBound(limites.left(), nuevaX, maxX);

    const qreal sueloY = 560 - rect().height();
    nuevaY = qBound(limites.top(), nuevaY, sueloY);

    setPos(nuevaX, nuevaY);

}

void Jugador::saltar()
{
    if (saltando || !scene())
        return;

    yInicial = pos().y();
    velocidadYInicial = -300;

    tiempo = 0;
    saltando = true;
    enSuelo = false;
}

void Jugador::actualizarMovimiento(qreal deltaTiempo){
    if (!scene())
        return;

    if (!saltando) {
        if (enSuelo && plataformaActual != nullptr) {
            QRectF limitesPlataforma =
                plataformaActual->sceneBoundingRect();

            qreal izquierdaEren = sceneBoundingRect().left();
            qreal derechaEren = sceneBoundingRect().right();

            bool sigueSobrePlataforma = derechaEren > limitesPlataforma.left() && izquierdaEren < limitesPlataforma.right();

            if (!sigueSobrePlataforma) {
                yInicial = pos().y();
                velocidadYInicial = 0;
                tiempo = 0;
                saltando = true;
                enSuelo = false;
            }
        }

        if (!saltando)
            return;
    }

    tiempo += deltaTiempo;

    qreal yAnterior = pos().y();

    qreal yFinal = yInicial
                   + velocidadYInicial * tiempo
                   + 0.5 * GRAVEDAD * tiempo * tiempo;


    if (velocidadYInicial + GRAVEDAD * tiempo > 0) {
        for (int i = 0; i < cantidadPlataformas; i++) {
            qreal alturaPlataforma = plataformas[i]->sceneBoundingRect().top();

            if (yAnterior + rect().height() <= alturaPlataforma &&
                yFinal + rect().height() >= alturaPlataforma) {

                qreal izquierdaEren = pos().x();
                qreal derechaEren = izquierdaEren + rect().width();

                QRectF limitesPlataforma = plataformas[i]->sceneBoundingRect();

                if (derechaEren > limitesPlataforma.left() &&
                    izquierdaEren < limitesPlataforma.right()) {

                    yFinal = alturaPlataforma - rect().height();
                    saltando = false;
                    enSuelo = true;
                    tiempo = 0;
                    plataformaActual = plataformas[i];
                    break;
                }
            }
        }
    }

    const qreal sueloY = 560 - rect().height();

    if (yFinal >= sueloY) {
        yFinal = sueloY;
        saltando = false;
        enSuelo = true;
        tiempo = 0;
        plataformaActual = nullptr;
    }

    setY(yFinal);

}
