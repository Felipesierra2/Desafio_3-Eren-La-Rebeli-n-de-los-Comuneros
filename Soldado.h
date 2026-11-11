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
    RECUPERANDO,
    PERSIGUIENDO
};

class Soldado : public EntidadJuego
{
private:
    static constexpr int VIDA_INICIAL = 3;

    TipoSoldado tipo;
    EstadoSoldado estado = EstadoSoldado::VIGILANDO;

    qreal rangoDeteccion = 150;
    qreal rangoAtaque = 55;
    qreal alcanceGolpe = 65;

    qreal tiempoEntreAtaques;
    qreal tiempoAtaque = 0;
    qreal duracionAtaque = 0.4;
    qreal tiempoEstado = 0;

    bool jugadorDetectado = false;

public:

    Soldado();
    Soldado(TipoSoldado tipoSoldado);

    void detectarJugador(const Jugador* jugador);
    void actualizarEstado(qreal deltaTiempo, Jugador* jugador);

    EstadoSoldado obtenerEstado() const;
    TipoSoldado obtenerTipo() const;

    qreal obtenerDireccionJugador(const Jugador* jugador) const;
    void actualizarPersecucion(qreal deltaTiempo, const Jugador* jugador);
};

#endif // SOLDADO_H
