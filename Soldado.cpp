#include "Soldado.h"
#include "Proyectil.h"
#include <QBrush>
#include <QDebug>
#include <qpainter.h>
#include <QPainterPath>


Soldado::Soldado(TipoSoldado tipoSoldado)
    : EntidadJuego(QRectF(0, 0, 35, 45), VIDA_INICIAL),
    tipo(tipoSoldado)
{
    setBrush(Qt::red);

    if (tipo == TipoSoldado::DISTANCIA) {
        rangoDeteccion = 250;
        rangoAtaque = 180;
        tiempoEntreAtaques = 2.5;

        hojaSprites.load(":/sprites/Momento_ll/sprites/SoldadoDistancia.png");

        bool imagenCargada =
            hojaSprites.load(":/sprites/Momento_ll/sprites/SoldadoDistancia.png");

        qDebug() << "Imagen cargada:" << imagenCargada;
        qDebug() << "Dimensiones:" << hojaSprites.size();
    }
    else {
        rangoDeteccion = 150;
        rangoAtaque = 45;
        tiempoEntreAtaques = 1.2;

        hojaSprites.load(":/sprites/Momento_ll/sprites/Soldado.png");

        if (hojaSprites.isNull())
            qDebug() << "Error al cargar los sprites del soldado";
        else
            qDebug() << "Sprites del soldado cargados correctamente";
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

    if (estado == EstadoSoldado::RETROCEDIENDO)
        nuevaDireccion *= -1;

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

        if (tipo == TipoSoldado::DISTANCIA &&
            distancia < distanciaMinima)
        {
            estado = EstadoSoldado::RETROCEDIENDO;
            return;
        }

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

            else if (tipo == TipoSoldado::DISTANCIA) {
                proyectilPendiente = atacarDistancia();
            }

            tiempoAtaque = tiempoEntreAtaques;
            estado = EstadoSoldado::RECUPERANDO;
        }

        return;
    }

    if (estado == EstadoSoldado::RETROCEDIENDO)
    {
        qreal distancia = qAbs(
            sceneBoundingRect().center().x() -
            jugador->sceneBoundingRect().center().x()
            );

        if (distancia >= 130)
        {
            estado = EstadoSoldado::ALERTA;
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

    if (estado == EstadoSoldado::PERSIGUIENDO ||
        estado == EstadoSoldado::RETROCEDIENDO){
        nuevaFila = 1;
    }

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

Proyectil* Soldado::atacarDistancia()
{
    if (tipo != TipoSoldado::DISTANCIA || estaDerrotado())
        return nullptr;

    Proyectil* proyectil = new Proyectil(direccionVisual);

    QRectF limitesSoldado = sceneBoundingRect();

    qreal posicionX;

    if (direccionVisual > 0)
        posicionX = limitesSoldado.right();
    else
        posicionX = limitesSoldado.left() - proyectil->rect().width();

    qreal posicionY = limitesSoldado.center().y();

    proyectil->setPos(posicionX, posicionY);

    return proyectil;
}

Proyectil* Soldado::recogerProyectil()
{
    Proyectil* proyectil = proyectilPendiente;

    proyectilPendiente = nullptr;

    return proyectil;
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

    const qreal velocidadPersecucion = 100;
    const qreal velocidadRetirada = 180;

    qreal direccion = obtenerDireccionJugador(jugador);

    if (estado == EstadoSoldado::RETROCEDIENDO)
    {
        qreal desplazamientoX =
            -direccion * velocidadRetirada * deltaTiempo;

        moveBy(desplazamientoX, 0);
        return;
    }

    if (estado != EstadoSoldado::PERSIGUIENDO)
        return;

    qreal desplazamientoX =
        direccion * velocidadPersecucion * deltaTiempo;

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
