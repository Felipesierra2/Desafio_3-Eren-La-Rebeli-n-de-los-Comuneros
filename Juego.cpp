#include "Juego.h"
#include "Jugador.h"
#include "Proyectil.h"
#include "Soldado.h"

#include <QGraphicsScene>
#include <QKeyEvent>
#include <QTimer>
#include <QtMath>
#include <QSizePolicy>

Juego::Juego(QWidget *parent)
    : QGraphicsView(parent),
    escena(new QGraphicsScene(this)),
    jugador(new Jugador()),
    temporizador(new QTimer(this))
{
    setWindowTitle("Rebelion de los Comuneros");
    setSizePolicy(QSizePolicy::Expanding,
                  QSizePolicy::Expanding);

    setScene(escena);
    scale(2.0, 2.0);

    setRenderHint(QPainter::Antialiasing);
    setBackgroundBrush(QBrush(QColor(28, 32, 45)));

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    construirNivel1();
    actualizarCamara();

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    connect(temporizador, &QTimer::timeout,
            this, &Juego::actualizarJuego);

    reloj.start();
    temporizador->start(16);
}

void Juego::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        event->accept();
        return;
    }

    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_A:
        izquierdaPresionada = true;
        break;

    case Qt::Key_Right:
    case Qt::Key_D:
        derechaPresionada = true;
        break;

    case Qt::Key_Up:
    case Qt::Key_W:
        jugador->saltar();
        break;

    case Qt::Key_Space:
        if (!event->isAutoRepeat())
            jugador->atacar();
        break;

    default:
        QGraphicsView::keyPressEvent(event);
        return;
    }

    event->accept();
}

void Juego::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        event->accept();
        return;
    }

    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_A:
        izquierdaPresionada = false;
        break;

    case Qt::Key_Right:
    case Qt::Key_D:
        derechaPresionada = false;
        break;

    default:
        QGraphicsView::keyReleaseEvent(event);
        return;
    }

    event->accept();
}

void Juego::actualizarJuego()
{
    qreal deltaTiempo = reloj.restart() / 1000.0;

    deltaTiempo = qMin(deltaTiempo, 0.05);

    if (plataformaMovil)
        plataformaMovil->actualizar(deltaTiempo);

    const qreal velocidadHorizontal = 180;

    qreal direccionHorizontal = 0;

    if (izquierdaPresionada)
        direccionHorizontal -= 1;

    if (derechaPresionada)
        direccionHorizontal += 1;

    jugador->movimientoHorizontal(direccionHorizontal * velocidadHorizontal * deltaTiempo,
                                  deltaTiempo);
    jugador->actualizarMovimiento(deltaTiempo);

    for (Soldado* soldado : std::as_const(soldados))
    {
        if (soldado->estaDerrotado())
            continue;

        soldado->actualizarEstado(deltaTiempo, jugador);

        Proyectil* nuevoProyectil = soldado->recogerProyectil();

        if (nuevoProyectil) {
            escena->addItem(nuevoProyectil);
            proyectiles.push_back(nuevoProyectil);
        }

        soldado->actualizarPersecucion(deltaTiempo, jugador);

        soldado->actualizarAnimacion(deltaTiempo);
    }

    for (auto it = soldados.begin(); it != soldados.end(); ){
        Soldado* soldado = *it;

        if (soldado->estaDerrotado())
        {
            jugador->desaparecerSoldado(soldado);

            scene()->removeItem(soldado);
            delete soldado;

            it = soldados.erase(it);
        }
        else
        {
            soldado->actualizar(deltaTiempo);
            ++it;
        }
    }

    for (auto it = proyectiles.begin(); it != proyectiles.end(); )
    {
        Proyectil* proyectil = *it;

        proyectil->actualizar(deltaTiempo);

        QRectF limitesProyectil = proyectil->sceneBoundingRect();

        bool impactoObstaculo = false;

        for (QGraphicsRectItem* columna : columnas)
        {
            if (columna &&
                limitesProyectil.intersects(columna->sceneBoundingRect()))
            {
                impactoObstaculo = true;
                break;
            }
        }

        if (!impactoObstaculo)
        {
            for (QGraphicsRectItem* plataforma : plataformas)
            {
                if (plataforma &&
                    limitesProyectil.intersects(plataforma->sceneBoundingRect()))
                {
                    impactoObstaculo = true;
                    break;
                }
            }
        }

        if (impactoObstaculo)
        {
            escena->removeItem(proyectil);
            delete proyectil;

            it = proyectiles.erase(it);
            continue;
        }

        QRectF limitesEscena = escena->sceneRect();

        const QRectF limitesEren =
            jugador->obtenerCuerpoFisico();

        if (limitesProyectil.intersects(limitesEren))
        {
            qreal direccionRetroceso =
                proyectil->x() < limitesEren.center().x() ? 1 : -1;

            jugador->recibirDano(1, direccionRetroceso);

            escena->removeItem(proyectil);
            delete proyectil;

            it = proyectiles.erase(it);
            continue;
        }

        if (limitesProyectil.right() < limitesEscena.left() ||
            limitesProyectil.left() > limitesEscena.right())
        {
            escena->removeItem(proyectil);
            delete proyectil;

            it = proyectiles.erase(it);
        }
        else {
            ++it;
        }
    }

    actualizarCamara();
}

void Juego::construirNivel1()
{
    escena->setSceneRect(
        0, 0, ANCHO_NIVEL1, ALTO_VISTA
        );

    crearPlataforma(
        0, ALTURA_PISO,
        ANCHO_NIVEL1, ALTO_VISTA - ALTURA_PISO
        );

    construirZonaSocorro();
    construirZonaResistencia();
    construirZonaObstaculos();
    construirZonaEncuentro();

    escena->addItem(jugador);
    jugador->setPos(50, ALTURA_PISO - 45);

    jugador->establecerColumnas(columnas);
    jugador->establecerPlataformas(plataformas);
}

void Juego::construirZonaSocorro()
{
    crearColumna(350, ALTURA_PISO - 30, 35, 30);

    crearSoldado(
        TipoSoldado::CUERPO_A_CUERPO,
        260, ALTURA_PISO - 45
        );

    crearSoldado(
        TipoSoldado::CUERPO_A_CUERPO,
        650, ALTURA_PISO - 45
        );

    crearSoldado(
        TipoSoldado::DISTANCIA,
        400, ALTURA_PISO - 45
        );
}

void Juego::construirZonaResistencia()
{
}

void Juego::construirZonaObstaculos()
{
}

void Juego::construirZonaEncuentro()
{
}

void Juego::actualizarCamara()
{
    const qreal alturaVisible =
        viewport()->height() / transform().m22();

    centerOn(
        jugador->sceneBoundingRect().center().x(),
        ALTO_VISTA - alturaVisible / 2.0
        );
}

void Juego::crearSoldado(TipoSoldado tipo, qreal x, qreal y)
{
    Soldado* soldado = new Soldado(tipo);

    soldado->setPos(x, y);
    soldado->establecerColumnasEscenario(columnas);

    escena->addItem(soldado);
    soldados.append(soldado);
}

void Juego::crearPlataforma(qreal x, qreal y,
                            qreal ancho, qreal alto)
{
    plataformas.push_back(
        escena->addRect(
            x, y, ancho, alto,
            QPen(Qt::NoPen),
            QBrush(QColor(180, 130, 80))
            )
        );
}

void Juego::crearColumna(qreal x, qreal y,
                         qreal ancho, qreal alto)
{
    columnas.push_back(
        escena->addRect(
            x, y, ancho, alto,
            QPen(Qt::NoPen),
            QBrush(QColor(130, 130, 130))
            )
        );
}
