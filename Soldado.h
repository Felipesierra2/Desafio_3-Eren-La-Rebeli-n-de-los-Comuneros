#ifndef SOLDADO_H
#define SOLDADO_H

#include "EntidadJuego.h"
#include "Jugador.h"

enum class TipoSoldado {
    DISTANCIA,
    CUERPO_A_CUERPO
};

enum class EstadoSoldado {
    VIGILANDO,
    ALERTA,
    ATACANDO,
    RECUPERANDO
};

class Soldado : public EntidadJuego
{
private:
    static constexpr int VIDA_INICIAL = 3;

    TipoSoldado tipo;
    EstadoSoldado estado = EstadoSoldado::VIGILANDO;

    qreal rangoDeteccion;
    qreal rangoAtaque;

    qreal tiempoEntreAtaques;
    qreal tiempoAtaque = 0;
    qreal duracionAtaque = 0.4;
    qreal tiempoEstado = 0;

    bool jugadorDetectado = false;

public:

    Soldado();
    Soldado(TipoSoldado tipoSoldado);

    void detectarJugador(const Jugador* jugador);
    void actualizarEstado(qreal deltaTiempo, const Jugador* jugador);

    EstadoSoldado obtenerEstado() const;
    TipoSoldado obtenerTipo() const;
};

#endif // SOLDADO_H
