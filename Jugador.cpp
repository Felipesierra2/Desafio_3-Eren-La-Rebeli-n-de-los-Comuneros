#include "Jugador.h"
#include "Soldado.h"

#include <QBrush>
#include <QColor>
#include <QPen>
#include <QGraphicsScene>
#include <QtMath>
#include <QDebug>

Jugador::Jugador(QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    setRect(0, 0, 35, 45);
    setBrush(QBrush(QColor(80, 170, 255)));
    setPen(QPen(Qt::white, 2));

    visualAtaque = new QGraphicsRectItem(this);

    visualAtaque->setRect(0, 0, 45, 35);
    visualAtaque->setBrush(QColor(255, 0, 0, 100));
    visualAtaque->setPen(Qt::NoPen);
    visualAtaque->setVisible(false);
}

void Jugador::establecerPlataformas(const std::vector<QGraphicsRectItem *> &plataformas)
{
    this->plataformas = plataformas;
}

void Jugador::establecerColumnas(
    const std::vector<QGraphicsRectItem*>& columnas)
{
    this->columnas = columnas;
}

void Jugador::movimientoHorizontal(qreal desplazamientoX, qreal deltaTiempo)
{
    if (!scene())
        return;

    if (velocidadRetroceso != 0) {
        desplazamientoX += velocidadRetroceso * deltaTiempo;

        const qreal desaceleracion = 900;

        if (velocidadRetroceso > 0)
            velocidadRetroceso = qMax(0.0, velocidadRetroceso - desaceleracion * deltaTiempo);
        else
            velocidadRetroceso = qMin(0.0, velocidadRetroceso + desaceleracion * deltaTiempo);
    }

    if (desplazamientoX > 0)
        mirandoDerecha = true;
    else if (desplazamientoX < 0)
        mirandoDerecha = false;

    const QRectF limites = scene()->sceneRect();

    qreal nuevaX = pos().x() + desplazamientoX;

    QRectF limitesEren = sceneBoundingRect();
    limitesEren.translate(desplazamientoX,0);

    for (QGraphicsRectItem* columna : columnas) {
        if (!columna)
            continue;

        if (limitesEren.intersects(columna->sceneBoundingRect())) {
            nuevaX = pos().x();
            break;
        }
    }

    QList<QGraphicsItem*> posiblesColisiones = scene()->items(limitesEren, Qt::IntersectsItemBoundingRect);

    QRectF limitesActuales = sceneBoundingRect();

    for(QGraphicsItem* elemento:posiblesColisiones){

        Soldado* soldado = dynamic_cast<Soldado*>(elemento);

        if(!soldado)
            continue;

        QRectF limitesSoldado = soldado->sceneBoundingRect();

        if(limitesEren.intersects(limitesSoldado)){

            qreal distanciaActul = qAbs(limitesActuales.center().x() - limitesSoldado.center().x());

            qreal distanciaNueva = qAbs(limitesEren.center().x() - limitesSoldado.center().x());

            if(!limitesActuales.intersects(limitesSoldado) || distanciaNueva < distanciaActul){

                nuevaX = pos().x();
                break;
            }
        }
    }

    const qreal maxX = limites.right() - rect().width();

    nuevaX = qBound(limites.left(), nuevaX, maxX);

    setX(nuevaX);
}

void Jugador::saltar()
{
    if (estaEnElAire() || !scene())
        return;

    yInicial = pos().y();
    velocidadYInicial = -300;

    tiempo = 0;
    cambiarEstado(SALTANDO);
}

bool Jugador::estaEnElAire() const
{
    return estadoActual == SALTANDO ||
           estadoActual == CAYENDO;
}

bool Jugador::estaEnElSuelo() const
{
    return estadoActual == QUIETO ||
           estadoActual == CAMINANDO;
}

bool Jugador::comprobarAterrizaje(
    const std::vector<QGraphicsRectItem*>& superficies,
    qreal yAnterior,
    qreal& yFinal)
{
    for (QGraphicsRectItem* superficie : superficies) {
        if (!superficie)
            continue;

        QRectF limites = superficie->sceneBoundingRect();
        qreal altura = limites.top();

        if (yAnterior + rect().height() <= altura &&
            yFinal + rect().height() >= altura) {

            qreal izquierdaEren = pos().x();
            qreal derechaEren = izquierdaEren + rect().width();

            if (derechaEren > limites.left() &&
                izquierdaEren < limites.right()) {

                yFinal = altura - rect().height();
                tiempo = 0;
                plataformaActual = superficie;
                cambiarEstado(QUIETO);

                return true;
            }
        }
    }

    return false;
}

void Jugador::actualizarCaida(qreal deltaTiempo)
{

    tiempo += deltaTiempo;

    qreal yAnterior = pos().y();

    qreal yFinal = yInicial
                   + velocidadYInicial * tiempo
                   + 0.5 * GRAVEDAD * tiempo * tiempo;

    if (velocidadYInicial + GRAVEDAD * tiempo > 0) {
        cambiarEstado(CAYENDO);

        if (!comprobarAterrizaje(plataformas, yAnterior, yFinal))
            comprobarAterrizaje(columnas, yAnterior, yFinal);

    }

    const qreal sueloY = 560 - rect().height();

    if (yFinal >= sueloY) {
        yFinal = sueloY;
        tiempo = 0;
        plataformaActual = nullptr;
        cambiarEstado(QUIETO);
    }

    setY(yFinal);
}

void Jugador::cambiarEstado(EstadoJugador nuevoEstado)
{
    estadoActual = nuevoEstado;
}

void Jugador::atacar()
{
    if (atacando)
        return;

    atacando = true;
    tiempoAtaque = 0;

    soldadosGolpeados.clear();
}

void Jugador::actualizarMovimiento(qreal deltaTiempo){
    if (!scene())
        return;

    if (invulnerable) {
        tiempoInvulnerabilidad -= deltaTiempo;

        if (tiempoInvulnerabilidad <= 0) {
            tiempoInvulnerabilidad = 0;
            invulnerable = false;
        }
    }

    if (estaEnElSuelo()) {
        if (plataformaActual != nullptr) {
            QRectF limitesPlataforma =
                plataformaActual->sceneBoundingRect();

            qreal izquierdaEren = sceneBoundingRect().left();
            qreal derechaEren = sceneBoundingRect().right();

            bool sigueSobrePlataforma = derechaEren > limitesPlataforma.left() && izquierdaEren < limitesPlataforma.right();

            if (!sigueSobrePlataforma) {
                yInicial = pos().y();
                velocidadYInicial = 0;
                tiempo = 0;
                cambiarEstado(CAYENDO);
            }
        }

        if (!estaEnElAire()){
            actualizarAtaque(deltaTiempo);
            return;
        }
    }

    actualizarCaida(deltaTiempo);
    actualizarAtaque(deltaTiempo);
}

QRectF Jugador::calcularAreaAtaque() const
{
    const qreal anchoAtaque = 45;
    const qreal altoAtaque = 35;

    qreal xAtaque;

    if (mirandoDerecha)
        xAtaque = sceneBoundingRect().right();
    else
        xAtaque = sceneBoundingRect().left() - anchoAtaque;

    qreal yAtaque = sceneBoundingRect().center().y() - altoAtaque / 2;

    return QRectF(xAtaque, yAtaque,
                  anchoAtaque, altoAtaque);
}

bool Jugador::hayImpacto(QGraphicsItem *objetivo) const
{
    if (objetivo == nullptr)
        return false;

    QRectF areaAtaque = calcularAreaAtaque();
    QRectF limitesObjetivo = objetivo->sceneBoundingRect();

    return areaAtaque.intersects(limitesObjetivo);
}

void Jugador::actualizarAtaque(qreal deltaTiempo)
{
    if (!atacando)
        return;

    tiempoAtaque += deltaTiempo;

    QRectF areaAtaque = calcularAreaAtaque();

    QList<QGraphicsItem*> elementos = scene()->items(areaAtaque, Qt::IntersectsItemBoundingRect);

    for(QGraphicsItem* elemento: elementos){

        Soldado* soldado = dynamic_cast<Soldado*>(elemento);

        if(!soldado)
            continue;

        if(soldadosGolpeados.contains(soldado))
            continue;

        qreal impulso = mirandoDerecha ? 300:-300;

        soldado->recibirDano(1,impulso);

        soldadosGolpeados.append(soldado);
    }

    visualAtaque->setPos(
        mirandoDerecha ? rect().width() : -45,
        rect().height() / 2 - 35 / 2
        );

    visualAtaque->setVisible(true);

    if (tiempoAtaque >= DURACION_ATAQUE) {
        atacando = false;
        tiempoAtaque = 0;
        visualAtaque->setVisible(false);
    }
}

void Jugador::desaparecerSoldado(Soldado* soldado)
{
    soldadosGolpeados.removeAll(soldado);
}

void Jugador::recibirDano(int cantidad, qreal direccion)
{
    if (cantidad <= 0 || vida <= 0 || invulnerable)
        return;

    vida -= cantidad;

    if (vida < 0)
        vida = 0;

    invulnerable = true;
    tiempoInvulnerabilidad = DURACION_INVULNERABILIDAD;

    const qreal FUERZA_RETROCESO = 350;
    velocidadRetroceso = direccion * FUERZA_RETROCESO;

    qDebug() << "Vida de Eren:" << vida;
}

int Jugador::obtenerVida() const
{
    return vida;
}
