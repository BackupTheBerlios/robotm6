
#include "devices/implementation/teslaSimu.h"
#include "simulatorClient.h"

/** arrete l'electro aimant */
bool BigTeslaSimu::stopTesla()
{
    // TODO
    return true;
}
/** passer la carte alime dans le mode correspondant avant de 
    demarrer l'electroaimant! */
bool BigTeslaSimu::startTesla(TeslaMode mode)
{
    // TODO
    return true;
}
/** tache peridoci qui verifie si on a accroche */
void BigTeslaSimu::periodicTask() 
{
    // TODO
    // events->raise(EVENTS_SKITTLE_ATTACHED);
}

