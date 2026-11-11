#include "Soldado.h"
#include <QBrush>

Soldado::Soldado()
    : EntidadJuego(QRectF(0,0,35,45), VIDA_INICIAL)
{
    setBrush(Qt::green);
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

void Soldado::actualizarEstado(qreal deltaTiempo, const Jugador* jugador)
{
    if (!jugador || estaDerrotado())
        return;

    detectarJugador(jugador);

    if (estado == EstadoSoldado::VIGILANDO)
        return;

    if (estado == EstadoSoldado::ALERTA) {
        tiempoEstado = duracionAtaque;
        estado = EstadoSoldado::ATACANDO;
        return;
    }

    if (estado == EstadoSoldado::ATACANDO) {
        tiempoEstado -= deltaTiempo;

        if (tiempoEstado <= 0) {
            tiempoAtaque = tiempoEntreAtaques;
            estado = EstadoSoldado::RECUPERANDO;
        }

        return;
    }

    if (estado == EstadoSoldado::RECUPERANDO) {
        tiempoAtaque -= deltaTiempo;

        if (tiempoAtaque <= 0)
            estado = EstadoSoldado::ALERTA;
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
