#ifndef JUGADOR_H
#define JUGADOR_H

#include <QGraphicsRectItem>
#include <vector>
#include <QVector>

enum EstadoJugador {
    QUIETO,
    CAMINANDO,
    SALTANDO,
    CAYENDO
};

class Soldado;

class Jugador : public QGraphicsRectItem
{
private:
    const qreal GRAVEDAD = 500;
    qreal velocidadYInicial = -300;
    qreal yInicial = 0;
    qreal tiempo = 0;
    QGraphicsRectItem *plataformaActual = nullptr;
    QGraphicsRectItem *visualAtaque = nullptr;
    QRectF calcularAreaAtaque() const;
    QVector<Soldado*> soldadosGolpeados;

    std::vector<QGraphicsRectItem*> plataformas;
    std::vector<QGraphicsRectItem*> columnas;

    EstadoJugador estadoActual = QUIETO;

    bool mirandoDerecha = true;
    bool comprobarAterrizaje(
        const std::vector<QGraphicsRectItem*>& superficies,
        qreal yAnterior,
        qreal& yFinal
        );
    bool estaEnElAire() const;
    bool estaEnElSuelo() const;

    bool hayImpacto(QGraphicsItem *objetivo) const;
    bool atacando = false;
    qreal tiempoAtaque = 0;
    const qreal DURACION_ATAQUE = 0.15;

    void actualizarCaida(qreal deltaTiempo);
    void cambiarEstado(EstadoJugador nuevoEstado);
public:
    explicit Jugador(QGraphicsItem *parent = nullptr);

    void saltar();
    void actualizarMovimiento(qreal deltaTiempo);
    void movimientoHorizontal(qreal desplazamientoX);
    void establecerPlataformas(const std::vector<QGraphicsRectItem*>& plataformas);
    void establecerColumnas(const std::vector<QGraphicsRectItem*>& columnas);
    void atacar();
    void actualizarAtaque(qreal deltaTiempo);

protected:
};


#endif // JUGADOR_H
