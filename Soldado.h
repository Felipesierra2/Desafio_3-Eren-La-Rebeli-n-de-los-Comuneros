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
    PERSIGUIENDO,
    RETROCEDIENDO
};

class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class Proyectil;

class Soldado : public EntidadJuego
{
private:
    QPixmap hojaSprites;
    static constexpr int VIDA_INICIAL = 5;

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
    qreal direccionVisual = 1;
    qreal duracionFotogramaAtaque = 0.1;
    qreal distanciaMinima = 90;

    int fotogramaActual = 0;
    int filaAnimacion = 0;

    bool jugadorDetectado = false;
    bool recibiendoDano = false;

    qreal tiempoReaccionDano = 0;

    Proyectil* proyectilPendiente = nullptr;

    void atacarCuerpoACuerpo(Jugador *jugador);

    qreal calcularDistanciaJugador(
        const Jugador* jugador) const;

public:
    Soldado(TipoSoldado tipoSoldado);

    void detectarJugador(const Jugador* jugador);
    void actualizarEstado(qreal deltaTiempo, Jugador* jugador);
    void actualizarAnimacion(qreal deltaTiempo);
    void recibirDano(int cantidad, qreal impulsoX) override;

    EstadoSoldado obtenerEstado() const;
    TipoSoldado obtenerTipo() const;

    qreal obtenerDireccionJugador(const Jugador* jugador) const;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void actualizarPersecucion(qreal deltaTiempo, const Jugador* jugador);
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget = nullptr) override;

    Proyectil* atacarDistancia();
    Proyectil* recogerProyectil();
};

#endif // SOLDADO_H
