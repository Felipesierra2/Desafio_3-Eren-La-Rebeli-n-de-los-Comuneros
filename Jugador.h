#ifndef JUGADOR_H
#define JUGADOR_H

#include <QGraphicsRectItem>
#include <vector>
#include <QVector>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

enum EstadoJugador {
    QUIETO,
    CAMINANDO,
    SALTANDO,
    CAYENDO
};

enum class AnimacionJugador {
    REPOSO,
    CORRER,
    SALTO,
    CAIDA,
    ATAQUE,
    ATAQUE_AEREO,
    HERIDO,
    MORIR
};

class Soldado;

class Jugador : public QGraphicsRectItem {
private:
    QPixmap hojaSprites;

    int fotogramaActual = 0;
    qreal tiempoAnimacion = 0;

    static constexpr int TAM_FOTOGRAMA = 256;
    static constexpr int FOTOGRAMAS_POR_FILA = 4;
    static constexpr qreal DURACION_REACCION_DANO = 0.70;
    static constexpr qreal DURACION_FOTOGRAMA_MUERTE = 0.25;

    const qreal GRAVEDAD = 700;
    qreal velocidadYInicial = -280;
    qreal yInicial = 0;
    qreal tiempo = 0;
    QGraphicsRectItem *plataformaActual = nullptr;
    QRectF calcularAreaAtaque() const;
    QVector<Soldado*> soldadosGolpeados;

    const std::vector<QGraphicsRectItem*>* plataformas = nullptr;
    const std::vector<QGraphicsRectItem*>* columnas = nullptr;

    EstadoJugador estadoActual = QUIETO;
    AnimacionJugador animacionActual = AnimacionJugador::REPOSO;

    bool mirandoDerecha = true;
    void buscarSuperficieAterrizaje(
        const std::vector<QGraphicsRectItem*>& superficies,
        qreal yAnterior,
        qreal yFinal,
        QGraphicsRectItem*& mejorSuperficie,
        qreal& menorFraccion) const;

    bool estaEnElAire() const;
    bool estaEnElSuelo() const;
    bool atacando = false;
    bool invulnerable = false;

    qreal tiempoAtaque = 0;
    static constexpr qreal DURACION_ATAQUE = 0.30;
    static constexpr qreal INICIO_IMPACTO = 0.20;
    static constexpr qreal FIN_IMPACTO = 0.30;

    void actualizarCaida(qreal deltaTiempo);
    void cambiarEstado(EstadoJugador nuevoEstado);

    int vida = 5;
    static constexpr int VIDA_MAXIMA = 5;

    qreal tiempoInvulnerabilidad = 0;
    static constexpr qreal DURACION_INVULNERABILIDAD = 1.0;
    qreal velocidadRetroceso = 0;

    void paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget
        ) override;
    void actualizarAnimacion(qreal deltaTiempo);

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
    bool estaDerrotado() const;
    bool animacionMuerteFinalizada() const;

    QRectF obtenerCuerpoFisico() const;
    QRectF boundingRect() const override;

};


#endif // JUGADOR_H
