#ifndef SOLDADO_H
#define SOLDADO_H

#include <QPixmap>

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

class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

class Soldado : public EntidadJuego
{
private:
    QPixmap hojaSprites;
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
    qreal tiempoAnimacion = 0;
    qreal duracionFotograma = 0.12;

    int fotogramaActual = 0;

    bool jugadorDetectado = false;

public:

    Soldado();
    Soldado(TipoSoldado tipoSoldado);

    void detectarJugador(const Jugador* jugador);
    void actualizarEstado(qreal deltaTiempo, Jugador* jugador);

    EstadoSoldado obtenerEstado() const;
    TipoSoldado obtenerTipo() const;

    qreal obtenerDireccionJugador(const Jugador* jugador) const;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void actualizarPersecucion(qreal deltaTiempo, const Jugador* jugador);
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget = nullptr) override;
};

#endif // SOLDADO_H
