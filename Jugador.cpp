#include "Jugador.h"
#include "PlataformaMovil.h"
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

    if (!hojaSprites.load(":/sprites/Momento_ll/sprites/Eren.png"))
    {
        qWarning() << "Error al cargar los sprites de Eren";
    }

    setBrush(QBrush(QColor(80, 170, 255)));
    setPen(QPen(Qt::white, 2));
}

void Jugador::establecerPlataformas(
    const std::vector<QGraphicsRectItem*>& plataformas)
{
    this->plataformas = &plataformas;
}

void Jugador::establecerColumnas(
    const std::vector<QGraphicsRectItem*>& columnas)
{
    this->columnas = &columnas;
}

void Jugador::movimientoHorizontal(qreal desplazamientoX,
                                   qreal deltaTiempo)
{
    if (!scene())
        return;

    if (vida <= 0)
        desplazamientoX = 0;

    const bool intentaCaminar = desplazamientoX != 0;
    const qreal xAnterior = pos().x();

    if (desplazamientoX > 0)
        mirandoDerecha = true;
    else if (desplazamientoX < 0)
        mirandoDerecha = false;

    if (velocidadRetroceso != 0) {
        desplazamientoX += velocidadRetroceso * deltaTiempo;

        const qreal desaceleracion = 900;

        if (velocidadRetroceso > 0)
            velocidadRetroceso = qMax(
                0.0, velocidadRetroceso - desaceleracion * deltaTiempo);
        else
            velocidadRetroceso = qMin(
                0.0, velocidadRetroceso + desaceleracion * deltaTiempo);
    }

    const QRectF limites = scene()->sceneRect();

    qreal nuevaX = pos().x() + desplazamientoX;

    QRectF limitesEren = mapRectToScene(rect());
    limitesEren.translate(desplazamientoX,0);

    if (columnas)
    {
        for (QGraphicsRectItem* columna : *columnas)
        {
            if (!columna)
                continue;

            const QRectF limitesColumna =
                columna->mapRectToScene(columna->rect());

            if (limitesEren.intersects(limitesColumna))
            {
                nuevaX = pos().x();
                break;
            }
        }
    }

    QList<QGraphicsItem*> posiblesColisiones = scene()->items(limitesEren, Qt::IntersectsItemBoundingRect);

    QRectF limitesActuales = mapRectToScene(rect());

    for(QGraphicsItem* elemento:posiblesColisiones){

        Soldado* soldado = dynamic_cast<Soldado*>(elemento);

        if(!soldado)
            continue;

        QRectF limitesSoldado = soldado->mapRectToScene(soldado->rect());

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

    if (estaEnElSuelo())
    {
        const bool seDesplazo =
            qAbs(nuevaX - xAnterior) > 0.001;

        if (intentaCaminar && seDesplazo)
            cambiarEstado(CAMINANDO);
        else
            cambiarEstado(QUIETO);
    }
}

void Jugador::saltar()
{
    if (vida <= 0 || estaEnElAire() || !scene())
        return;

    yInicial = pos().y();
    velocidadYInicial = -280;

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

void Jugador::buscarSuperficieAterrizaje(
    const std::vector<QGraphicsRectItem*>& superficies,
    qreal yAnterior,
    qreal yFinal,
    QGraphicsRectItem*& mejorSuperficie,
    qreal& menorFraccion) const
{
    const QRectF cuerpo = mapRectToScene(rect());

    const qreal piesAnteriores = yAnterior + rect().height();
    const qreal piesFinales = yFinal + rect().height();

    for (QGraphicsRectItem* superficie : superficies)
    {
        if (!superficie)
            continue;

        const QRectF limites =
            superficie->mapRectToScene(superficie->rect());

        if (cuerpo.right() <= limites.left() ||
            cuerpo.left() >= limites.right())
        {
            continue;
        }

        const qreal altura = limites.top();
        qreal alturaAnterior = altura;

        const PlataformaMovil* movil =
            dynamic_cast<const PlataformaMovil*>(superficie);

        if (movil)
            alturaAnterior -= movil->obtenerDesplazamientoY();

        const qreal distanciaInicial =
            alturaAnterior - piesAnteriores;

        const qreal distanciaFinal =
            altura - piesFinales;

        if (distanciaInicial < 0 || distanciaFinal > 0)
            continue;

        const qreal recorridoRelativo = distanciaInicial - distanciaFinal;

        if (recorridoRelativo <= 0)
            continue;

        const qreal fraccion =
            distanciaInicial / recorridoRelativo;

        if (fraccion < menorFraccion)
        {
            menorFraccion = fraccion;
            mejorSuperficie = superficie;
        }
    }
}

void Jugador::actualizarCaida(qreal deltaTiempo)
{
    tiempo += deltaTiempo;

    const qreal yAnterior = pos().y();

    qreal yFinal = yInicial
                   + velocidadYInicial * tiempo
                   + 0.5 * GRAVEDAD * tiempo * tiempo;

    if (velocidadYInicial + GRAVEDAD * tiempo > 0)
    {
        cambiarEstado(CAYENDO);

        QGraphicsRectItem* mejorSuperficie = nullptr;
        qreal menorFraccion = 2.0;

        if (plataformas)
            buscarSuperficieAterrizaje(
                *plataformas, yAnterior, yFinal,
                mejorSuperficie, menorFraccion);

        if (columnas)
            buscarSuperficieAterrizaje(
                *columnas, yAnterior, yFinal,
                mejorSuperficie, menorFraccion);

        if (mejorSuperficie)
        {
            yFinal = mejorSuperficie
                         ->mapRectToScene(mejorSuperficie->rect())
                         .top() - rect().height();

            tiempo = 0;
            plataformaActual = mejorSuperficie;

            cambiarEstado(QUIETO);
        }
    }

    setY(yFinal);
}

void Jugador::cambiarEstado(EstadoJugador nuevoEstado)
{
    estadoActual = nuevoEstado;
}

void Jugador::atacar()
{
    if (atacando || vida <= 0)
        return;

    if (invulnerable &&
        DURACION_INVULNERABILIDAD - tiempoInvulnerabilidad
            < DURACION_REACCION_DANO)
    {
        return;
    }

    atacando = true;
    tiempoAtaque = 0;

    soldadosGolpeados.clear();
}

void Jugador::actualizarMovimiento(qreal deltaTiempo){
    if (!scene())
        return;

    actualizarAnimacion(deltaTiempo);

    if (invulnerable) {
        tiempoInvulnerabilidad -= deltaTiempo;

        if (tiempoInvulnerabilidad <= 0) {
            tiempoInvulnerabilidad = 0;
            invulnerable = false;
        }
    }

    if (estaEnElSuelo()) {

        if (plataformaActual != nullptr) {

            const QRectF limitesPlataforma =
                plataformaActual->mapRectToScene(
                    plataformaActual->rect());

            const QRectF cuerpo = obtenerCuerpoFisico();

            const qreal izquierdaEren = cuerpo.left();
            const qreal derechaEren = cuerpo.right();

            bool sigueSobrePlataforma = derechaEren > limitesPlataforma.left() && izquierdaEren < limitesPlataforma.right();

            if (!sigueSobrePlataforma) {
                yInicial = pos().y();
                velocidadYInicial = 0;
                tiempo = 0;
                cambiarEstado(CAYENDO);
            }
            else {
                PlataformaMovil* plataformaMovil = dynamic_cast<PlataformaMovil*>(plataformaActual);

                if (plataformaMovil) {
                    setY(pos().y() +
                         plataformaMovil->obtenerDesplazamientoY());
                }
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

    const QRectF cuerpo = mapRectToScene(rect());

    const qreal xAtaque = mirandoDerecha
                              ? cuerpo.right()
                              : cuerpo.left() - anchoAtaque;

    const qreal yAtaque =
        cuerpo.center().y() - altoAtaque / 2;

    return QRectF(
        xAtaque, yAtaque,
        anchoAtaque, altoAtaque
        );
}

QRectF Jugador::obtenerCuerpoFisico() const
{
    return mapRectToScene(rect());
}

void Jugador::actualizarAtaque(qreal deltaTiempo)
{
    if (!atacando)
        return;

    tiempoAtaque += deltaTiempo;

    if (tiempoAtaque >= DURACION_ATAQUE)
    {
        atacando = false;
        tiempoAtaque = 0;
        return;
    }

    if (tiempoAtaque < INICIO_IMPACTO ||
        tiempoAtaque >= FIN_IMPACTO)
    {
        return;
    }

    const QRectF areaAtaque = calcularAreaAtaque();

    QList<QGraphicsItem*> elementos =
        scene()->items(areaAtaque,
                       Qt::IntersectsItemBoundingRect);

    for (QGraphicsItem* elemento : elementos)
    {
        Soldado* soldado =
            dynamic_cast<Soldado*>(elemento);

        if (!soldado)
            continue;

        if (soldadosGolpeados.contains(soldado))
            continue;

        const QRectF cuerpoSoldado =
            soldado->mapRectToScene(soldado->rect());

        if (!areaAtaque.intersects(cuerpoSoldado))
            continue;

        const qreal impulso =
            mirandoDerecha ? 300 : -300;

        soldado->recibirDano(1, impulso);

        soldadosGolpeados.append(soldado);
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

    if (atacando) {
        qDebug() << "Ataque interrumpido por impacto";

        atacando = false;
        tiempoAtaque = 0;
    }

    tiempoInvulnerabilidad = DURACION_INVULNERABILIDAD;

    const qreal FUERZA_RETROCESO = 350;
    velocidadRetroceso = direccion * FUERZA_RETROCESO;

    qDebug() << "Vida de Eren:" << vida;
}

int Jugador::obtenerVida() const
{
    return vida;
}

void Jugador::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget)
{
    if (hojaSprites.isNull())
    {
        QGraphicsRectItem::paint(painter, option, widget);
        return;
    }

    const QRect origen(
        fotogramaActual * TAM_FOTOGRAMA,
        static_cast<int>(animacionActual) * TAM_FOTOGRAMA,
        TAM_FOTOGRAMA,
        TAM_FOTOGRAMA
        );

    painter->save();

    if (!mirandoDerecha) {
        painter->translate(2 * rect().center().x(), 0);
        painter->scale(-1, 1);
    }

    const qreal lado = rect().height();

    const QRectF destino(
        rect().center().x() - lado / 2,
        rect().bottom() - lado,
        lado,
        lado
        );

    painter->drawPixmap(destino, hojaSprites, origen);

    painter->restore();
}

QRectF Jugador::boundingRect() const{
    const qreal lado = rect().height();

    const QRectF limitesSprite(
        rect().center().x() - lado / 2,
        rect().bottom() - lado,
        lado,
        lado
        );

    return QGraphicsRectItem::boundingRect()
        .united(limitesSprite);
}

void Jugador::actualizarAnimacion(qreal deltaTiempo)
{
    if (vida <= 0) {

        if (animacionActual != AnimacionJugador::MORIR) {
            animacionActual = AnimacionJugador::MORIR;
            fotogramaActual = 0;
            tiempoAnimacion = 0;
            update();
            return;
        }

        tiempoAnimacion += deltaTiempo;

        if (fotogramaActual == FOTOGRAMAS_POR_FILA - 1) {
            tiempoAnimacion = qMin(
                tiempoAnimacion,
                DURACION_FOTOGRAMA_MUERTE
                );

            return;
        }

        if (tiempoAnimacion >= DURACION_FOTOGRAMA_MUERTE) {
            tiempoAnimacion -= DURACION_FOTOGRAMA_MUERTE;
            ++fotogramaActual;
            update();
        }

        return;
    }

    AnimacionJugador nuevaAnimacion = AnimacionJugador::REPOSO;

    if (invulnerable && vida > 0) {
        const qreal tiempoDesdeImpacto =
            DURACION_INVULNERABILIDAD - tiempoInvulnerabilidad;

        if (tiempoDesdeImpacto < DURACION_REACCION_DANO) {

            const int nuevoFotograma = qMin(
                static_cast<int>(
                    tiempoDesdeImpacto /
                    (DURACION_REACCION_DANO / FOTOGRAMAS_POR_FILA)
                    ),
                FOTOGRAMAS_POR_FILA - 1
                );

            if (animacionActual != AnimacionJugador::HERIDO ||
                fotogramaActual != nuevoFotograma)
            {
                animacionActual = AnimacionJugador::HERIDO;
                fotogramaActual = nuevoFotograma;
                tiempoAnimacion = 0;
                update();

                qDebug() << "HERIDO - Fila:"
                         << static_cast<int>(animacionActual)
                         << "Fotograma:" << fotogramaActual
                         << "Tiempo:" << tiempoDesdeImpacto;
            }

            return;
        }
    }

    if (atacando) {
        nuevaAnimacion = estaEnElAire()
        ? AnimacionJugador::ATAQUE_AEREO
        : AnimacionJugador::ATAQUE;

        const int nuevoFotograma = qMin(
            static_cast<int>(
                tiempoAtaque /
                (DURACION_ATAQUE / FOTOGRAMAS_POR_FILA)
                ),
            FOTOGRAMAS_POR_FILA - 1
            );

        if (animacionActual != nuevaAnimacion ||
            fotogramaActual != nuevoFotograma)
        {
            animacionActual = nuevaAnimacion;
            fotogramaActual = nuevoFotograma;
            tiempoAnimacion = 0;
            update();
        }

        return;
    }

    switch (estadoActual) {
    case QUIETO:
        nuevaAnimacion = AnimacionJugador::REPOSO;
        break;

    case CAMINANDO:
        nuevaAnimacion = AnimacionJugador::CORRER;
        break;

    case SALTANDO:
        nuevaAnimacion = AnimacionJugador::SALTO;
        break;

    case CAYENDO:
        nuevaAnimacion = AnimacionJugador::CAIDA;
        break;
    }

    if (nuevaAnimacion != animacionActual) {
        animacionActual = nuevaAnimacion;
        fotogramaActual = 0;
        tiempoAnimacion = 0;
        update();
        return;
    }

    const bool animacionCiclica =
        animacionActual == AnimacionJugador::REPOSO ||
        animacionActual == AnimacionJugador::CORRER;

    if (!animacionCiclica &&
        fotogramaActual == FOTOGRAMAS_POR_FILA - 1)
    {
        return;
    }

    const qreal duracionFotograma =
        animacionActual == AnimacionJugador::CORRER
                        ? 0.10
                        : 0.25;;

    tiempoAnimacion += deltaTiempo;

    if (tiempoAnimacion >= duracionFotograma) {
        tiempoAnimacion -= duracionFotograma;

        fotogramaActual =
            (fotogramaActual + 1) % FOTOGRAMAS_POR_FILA;

        update();
    }
}

bool Jugador::estaDerrotado() const
{
    return vida <= 0;
}

bool Jugador::animacionMuerteFinalizada() const
{
    return estaDerrotado() &&
           animacionActual == AnimacionJugador::MORIR &&
           fotogramaActual == FOTOGRAMAS_POR_FILA - 1 &&
           tiempoAnimacion >= DURACION_FOTOGRAMA_MUERTE;
}
