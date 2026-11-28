#ifndef JUEGO_H
#define JUEGO_H

#include "PlataformaMovil.h"
#include <QGraphicsView>
#include <QElapsedTimer>
#include <vector>

enum class TipoSoldado;
class QGraphicsRectItem;
class QGraphicsScene;
class QKeyEvent;
class QTimer;
class Jugador;
class Soldado;
class Proyectil;

class Juego : public QGraphicsView
{
public:
    explicit Juego(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void actualizarJuego();

    void construirNivel1();
    void construirZonaSocorro();
    void construirZonaResistencia();
    void construirZonaObstaculos();
    void construirZonaEncuentro();

    void actualizarCamara();
    void crearSoldado(TipoSoldado tipo, qreal x, qreal y);
    void crearPlataforma(qreal x, qreal y,
                         qreal ancho, qreal alto);

    void crearColumna(qreal x, qreal y,
                      qreal ancho, qreal alto);

    bool izquierdaPresionada = false;
    bool derechaPresionada = false;

    QElapsedTimer reloj;
    QGraphicsScene *escena;
    std::vector<QGraphicsRectItem*> plataformas;
    std::vector<QGraphicsRectItem*> columnas;
    PlataformaMovil* plataformaMovil = nullptr;
    Jugador *jugador;
    QVector<Soldado*> soldados;
    QTimer *temporizador;
    std::vector<Proyectil*> proyectiles;

    static constexpr int ANCHO_VISTA = 800;
    static constexpr int ALTO_VISTA = 600;
    static constexpr int ANCHO_NIVEL1 = 3200;
    static constexpr int ALTURA_PISO = 560;
};

#endif // JUEGO_H
