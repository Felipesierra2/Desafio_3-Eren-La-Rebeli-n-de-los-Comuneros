#include "Soldado.h"
#include <QBrush>
#include <QDebug>
#include <qpainter.h>
#include <QPainterPath>

Soldado::Soldado()
    : EntidadJuego(QRectF(0,0,35,45), VIDA_INICIAL)
{
    setBrush(Qt::green);
    hojaSprites.load(":/sprites/Momento_ll/sprites/Soldado.png");

    if (hojaSprites.isNull())
        qDebug() << "Error al cargar los sprites del soldado";
    else
        qDebug() << "Sprites del soldado cargados correctamente";
}

Soldado::Soldado(TipoSoldado tipoSoldado)
    : EntidadJuego(QRectF(0, 0, 35, 45), VIDA_INICIAL),
    tipo(tipoSoldado)
{
    setBrush(Qt::red);

    if (tipo == TipoSoldado::DISTANCIA) {
        rangoDeteccion = 250;
        rangoAtaque = 180;
        tiempoEntreAtaques = 2.5;
    }
    else {
        rangoDeteccion = 150;
        rangoAtaque = 45;
        tiempoEntreAtaques = 1.2;
    }
}

void Soldado::detectarJugador(const Jugador* jugador)
{
    if (!jugador || estaDerrotado()) {
        jugadorDetectado = false;
        return;
    }

    qreal distancia = qAbs(
        sceneBoundingRect().center().x() -
        jugador->sceneBoundingRect().center().x()
        );

    jugadorDetectado = distancia <= rangoDeteccion;
}

void Soldado::actualizarEstado(qreal deltaTiempo, Jugador *jugador)
{
    if (!jugador || estaDerrotado())
        return;

    detectarJugador(jugador);

    if (estado == EstadoSoldado::VIGILANDO){
        if(jugadorDetectado)
            estado = EstadoSoldado::ALERTA;

        return;
    }

    if (estado == EstadoSoldado::ALERTA) {
        if(!jugadorDetectado){
            estado = EstadoSoldado::VIGILANDO;

            return;
        }

        qreal distancia = qAbs(
            sceneBoundingRect().center().x() -
            jugador->sceneBoundingRect().center().x()
            );

        if (distancia <= rangoAtaque) {
            tiempoEstado = duracionAtaque;
            estado = EstadoSoldado::ATACANDO;
            return;
        }
        else {
            estado = EstadoSoldado::PERSIGUIENDO;
        }

        return;
    }

    if (estado == EstadoSoldado::ATACANDO) {
        tiempoEstado -= deltaTiempo;

        if (tiempoEstado <= 0) {
            QRectF limitesSoldado = sceneBoundingRect();
            QRectF limitesEren = jugador->sceneBoundingRect();

            qreal distanciaX = qAbs(limitesSoldado.center().x() - limitesEren.center().x());

            bool estaAlAlcance =
                distanciaX <= alcanceGolpe &&
                limitesSoldado.top() < limitesEren.bottom() &&
                limitesSoldado.bottom() > limitesEren.top();

            if(estaAlAlcance){
                qreal direccionRetroceso = limitesEren.center().x() >
                                                   limitesSoldado.center().x() ? 1:-1;

                jugador->recibirDano(1, direccionRetroceso);
            }

            tiempoAtaque = tiempoEntreAtaques;
            estado = EstadoSoldado::RECUPERANDO;
        }

        return;
    }

    if (estado == EstadoSoldado::PERSIGUIENDO) {
        if (!jugadorDetectado) {
            estado = EstadoSoldado::VIGILANDO;
            return;
        }

        qreal distancia = qAbs(
            sceneBoundingRect().center().x() -
            jugador->sceneBoundingRect().center().x()
            );

        if (distancia <= rangoAtaque) {
            qDebug() << "Soldado entra en rango de ataque";

            tiempoEstado = duracionAtaque;
            estado = EstadoSoldado::ATACANDO;
        }

        return;
    }

    if (estado == EstadoSoldado::RECUPERANDO) {
        tiempoAtaque -= deltaTiempo;

        if (tiempoAtaque <= 0){
            if(jugadorDetectado)
                estado = EstadoSoldado::ALERTA;
            else
                estado = EstadoSoldado::VIGILANDO;
        }
    }

}

EstadoSoldado Soldado::obtenerEstado() const
{
    return estado;
}

TipoSoldado Soldado::obtenerTipo() const
{
    return tipo;
}

qreal Soldado::obtenerDireccionJugador(const Jugador* jugador) const
{
    if (!jugador)
        return 0;

    qreal diferencia =
        jugador->sceneBoundingRect().center().x() -
        sceneBoundingRect().center().x();

    if (diferencia > 0)
        return 1;

    if (diferencia < 0)
        return -1;

    return 0;
}

void Soldado::actualizarPersecucion(qreal deltaTiempo, const Jugador* jugador){
    if (!jugador || estaDerrotado())
        return;

    if (estado != EstadoSoldado::PERSIGUIENDO)
        return;

    const qreal velocidadPersecucion = 100;

    qreal direccion = obtenerDireccionJugador(jugador);

    qreal desplazamientoX = direccion * velocidadPersecucion * deltaTiempo;

    moveBy(desplazamientoX, 0);
}

void Soldado::paint(QPainter *painter,
                    const QStyleOptionGraphicsItem *option,
                    QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (hojaSprites.isNull())
        return;

    QRectF origen(0, 0, 256, 256);

    QRectF destino(-10, -10, 100, 65);

    painter->drawPixmap(destino, hojaSprites, origen);
}

QRectF Soldado::boundingRect() const
{
    return QRectF(-10, -10, 100, 65);
}

QPainterPath Soldado::shape() const
{
    QPainterPath camino;
    camino.addRect(rect());

    return camino;
}
