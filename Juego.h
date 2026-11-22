#ifndef JUEGO_H
#define JUEGO_H

#include <QGraphicsView>
#include <QElapsedTimer>
#include <vector>


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

    bool izquierdaPresionada = false;
    bool derechaPresionada = false;

    QElapsedTimer reloj;
    QGraphicsScene *escena;
    std::vector<QGraphicsRectItem*> plataformas;
    std::vector<QGraphicsRectItem*> columnas;
    Jugador *jugador;
    QVector<Soldado*> soldados;
    QTimer *temporizador;
    std::vector<Proyectil*> proyectiles;
};

#endif // JUEGO_H
