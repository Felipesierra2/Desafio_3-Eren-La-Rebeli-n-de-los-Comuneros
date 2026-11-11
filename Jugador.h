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

    int vida = 5;
    static constexpr int VIDA_MAXIMA = 5;
    bool invulnerable = false;
    qreal tiempoInvulnerabilidad = 0;
    static constexpr qreal DURACION_INVULNERABILIDAD = 1.0;
    qreal velocidadRetroceso = 0;
public:
    explicit Jugador(QGraphicsItem *parent = nullptr);

    void saltar();
    void actualizarMovimiento(qreal deltaTiempo);
    void movimientoHorizontal(qreal desplazamientoX, qreal deltaTiempo);
    void establecerPlataformas(const std::vector<QGraphicsRectItem*>& plataformas);
    void establecerColumnas(const std::vector<QGraphicsRectItem*>& columnas);
    void atacar();
    void actualizarAtaque(qreal deltaTiempo);

    void desaparecerSoldado(Soldado* soldado);

    void recibirDano(int cantidad, qreal direccion);
    int obtenerVida() const;

protected:
};


#endif // JUGADOR_H
