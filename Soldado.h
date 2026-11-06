#ifndef SOLDADO_H
#define SOLDADO_H

#include "EntidadJuego.h"

class Soldado : public EntidadJuego
{
public:
    Soldado();

private:
    static constexpr int VIDA_INICIAL = 3;
};

#endif // SOLDADO_H
