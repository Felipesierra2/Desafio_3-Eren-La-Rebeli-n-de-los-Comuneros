#include "Soldado.h"
#include <QBrush>
#include <QDebug>
#include <qpainter.h>
#include <QPainterPath>

Soldado::Soldado()
    : EntidadJuego(QRectF(0,0,35,45), VIDA_INICIAL)
{
    setBrush(Qt::green);
    hojaSprites.load(":/sprites/Momento_ll/sprites/Soldado.png");

    if (hojaSprites.isNull())
        qDebug() << "Error al cargar los sprites del soldado";
    else
        qDebug() << "Sprites del soldado cargados correctamente";
}

Soldado::Soldado(TipoSoldado tipoSoldado)
    : EntidadJuego(QRectF(0, 0, 35, 45), VIDA_INICIAL),
    tipo(tipoSoldado)
{
    setBrush(Qt::red);

    if (tipo == TipoSoldado::DISTANCIA) {
        rangoDeteccion = 250;
        rangoAtaque = 180;
        tiempoEntreAtaques = 2.5;
    }
    else {
        rangoDeteccion = 150;
        rangoAtaque = 45;
        tiempoEntreAtaques = 1.2;
    }
}

void Soldado::detectarJugador(const Jugador* jugador)
{
    if (!jugador || estaDerrotado()) {
        jugadorDetectado = false;
        return;
    }

    qreal distancia = qAbs(
        sceneBoundingRect().center().x() -
        jugador->sceneBoundingRect().center().x()
        );

    jugadorDetectado = distancia <= rangoDeteccion;
}

void Soldado::actualizarEstado(qreal deltaTiempo, Jugador *jugador)
{
    if (!jugador || estaDerrotado())
        return;

    qreal nuevaDireccion = obtenerDireccionJugador(jugador);

    if (nuevaDireccion != 0 && nuevaDireccion != direccionVisual){
        direccionVisual = nuevaDireccion;
        update();
    }

    detectarJugador(jugador);

    if (estado == EstadoSoldado::VIGILANDO){
        if(jugadorDetectado)
            estado = EstadoSoldado::ALERTA;

        return;
    }

    if (estado == EstadoSoldado::ALERTA) {
        if(!jugadorDetectado){
            estado = EstadoSoldado::VIGILANDO;

            return;
        }

        qreal distancia = qAbs(
            sceneBoundingRect().center().x() -
            jugador->sceneBoundingRect().center().x()
            );

        if (distancia <= rangoAtaque) {
            tiempoEstado = duracionAtaque;
            estado = EstadoSoldado::ATACANDO;
            return;
        }
        else {
            estado = EstadoSoldado::PERSIGUIENDO;
        }

        return;
    }

    if (estado == EstadoSoldado::ATACANDO) {
        tiempoEstado -= deltaTiempo;

        if (tiempoEstado <= 0) {

            if (tipo == TipoSoldado::CUERPO_A_CUERPO) {
                atacarCuerpoACuerpo(jugador);
            }

            tiempoAtaque = tiempoEntreAtaques;
            estado = EstadoSoldado::RECUPERANDO;
        }

        return;
    }

    if (estado == EstadoSoldado::PERSIGUIENDO) {
        if (!jugadorDetectado) {
            estado = EstadoSoldado::VIGILANDO;
            return;
        }

        qreal distancia = qAbs(
            sceneBoundingRect().center().x() -
            jugador->sceneBoundingRect().center().x()
            );

        if (distancia <= rangoAtaque) {
            qDebug() << "Soldado entra en rango de ataque";

            tiempoEstado = duracionAtaque;
            estado = EstadoSoldado::ATACANDO;
        }

        return;
    }

    if (estado == EstadoSoldado::RECUPERANDO) {
        tiempoAtaque -= deltaTiempo;

        if (tiempoAtaque <= 0){
            if(jugadorDetectado)
                estado = EstadoSoldado::ALERTA;
            else
                estado = EstadoSoldado::VIGILANDO;
        }
    }

}

void Soldado::actualizarAnimacion(qreal deltaTiempo)
{
    if (recibiendoDano) {
        tiempoReaccionDano -= deltaTiempo;

        if (tiempoReaccionDano <= 0) {
            tiempoReaccionDano = 0;
            recibiendoDano = false;
        }
    }

    int nuevaFila = 0;

    if (estado == EstadoSoldado::PERSIGUIENDO)
        nuevaFila = 1;

    else if (estado == EstadoSoldado::ATACANDO)
        nuevaFila = 2;

    if (recibiendoDano)
        nuevaFila = 3;

    if (nuevaFila != filaAnimacion) {
        filaAnimacion = nuevaFila;
        fotogramaActual = 0;
        tiempoAnimacion = 0;
    }

    qreal duracionActual = duracionFotograma;

    bool animacionAtaque =
        estado == EstadoSoldado::ATACANDO && !recibiendoDano;

    if (animacionAtaque)
        duracionActual = duracionFotogramaAtaque;

    tiempoAnimacion += deltaTiempo;

    if (tiempoAnimacion >= duracionActual) {
        tiempoAnimacion -= duracionActual;

        if (animacionAtaque) {
            if (fotogramaActual < 3)
                fotogramaActual++;
        }
        else {
            fotogramaActual = (fotogramaActual + 1) % 4;
        }
    }

    // if (estado == EstadoSoldado::ATACANDO && !recibiendoDano)
    //     fotogramaActual = 0;

    static int ultimoFotograma = -1;

    if (fotogramaActual != ultimoFotograma) {
        qDebug() << "Fila:" << filaAnimacion
                 << "Columna:" << fotogramaActual;

        ultimoFotograma = fotogramaActual;
    }

    update();
}

EstadoSoldado Soldado::obtenerEstado() const
{
    return estado;
}

TipoSoldado Soldado::obtenerTipo() const
{
    return tipo;
}

void Soldado::recibirDano(int cantidad, qreal impulsoX)
{

    if (cantidad <= 0 || estaDerrotado())
        return;

    EntidadJuego::recibirDano(cantidad, impulsoX);

    if (estaDerrotado())
        return;

    recibiendoDano = true;
    tiempoReaccionDano = 0.25;
}

void Soldado::atacarCuerpoACuerpo(Jugador *jugador)
{
    if (!jugador)
        return;

    QRectF limitesSoldado = sceneBoundingRect();
    QRectF limitesEren = jugador->sceneBoundingRect();

    qreal distanciaX = qAbs(
        limitesSoldado.center().x() -
        limitesEren.center().x()
        );

    bool estaAlAlcance =
        distanciaX <= alcanceGolpe &&
        limitesSoldado.top() < limitesEren.bottom() &&
        limitesSoldado.bottom() > limitesEren.top();

    if (estaAlAlcance) {
        qreal direccionRetroceso =
            limitesEren.center().x() >
                    limitesSoldado.center().x() ? 1 : -1;

        jugador->recibirDano(1, direccionRetroceso);
    }
}

qreal Soldado::obtenerDireccionJugador(const Jugador* jugador) const
{
    if (!jugador)
        return 0;

    qreal diferencia =
        jugador->sceneBoundingRect().center().x() -
        mapRectToScene(rect()).center().x();

    if (diferencia > 0)
        return 1;

    if (diferencia < 0)
        return -1;

    return 0;
}

void Soldado::actualizarPersecucion(qreal deltaTiempo, const Jugador* jugador){
    if (!jugador || estaDerrotado())
        return;

    if (estado != EstadoSoldado::PERSIGUIENDO)
        return;

    const qreal velocidadPersecucion = 100;

    qreal direccion = obtenerDireccionJugador(jugador);

    qreal desplazamientoX = direccion * velocidadPersecucion * deltaTiempo;

    moveBy(desplazamientoX, 0);
}

void Soldado::paint(QPainter *painter,
                    const QStyleOptionGraphicsItem *option,
                    QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (hojaSprites.isNull())
        return;

    static bool fotogramasGuardados = false;

    QRectF origen(
        fotogramaActual * 256,
        filaAnimacion * 256,
        256,
        256
        );

    QRectF destino(-10, -10, 100, 65);

    painter->save();

    if(direccionVisual < 0){
        qreal centroX = rect().center().x();

        painter->translate(2 * centroX, 0);
        painter->scale(-1,1);
    }

    painter->drawPixmap(destino, hojaSprites, origen);

    painter->setPen(QPen(Qt::green, 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(destino);

    painter->restore();
}

QRectF Soldado::boundingRect() const
{
    return QRectF(-65, -10, 165, 65);
}

QPainterPath Soldado::shape() const
{
    QPainterPath camino;
    camino.addRect(rect());

    return camino;
}
