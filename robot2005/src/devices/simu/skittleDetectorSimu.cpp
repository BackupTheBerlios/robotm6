#include "devices/implementation/skittleDetectorSimu.h"
#include "simulatorClient.h"
#include "events.h"


// envoie la grue a une certaine position, retourne la derniere
// position detectee de quille, meme si la quille n'est plus detectee
// actuellement il y a l'ancienne valeur
bool SkittleDetectorSimu::getValue(SkittlePosition& pos)
{
    // TODO
    pos=0;
    return true;
}
// active l'evenement de detection de la quille // par defaut a true
void SkittleDetectorSimu::enableDetection()
{
    detectionEnabled_ = true;
}
// desactive l'evenement de detection de la quille
void SkittleDetectorSimu::disableDetection()
{
    detectionEnabled_ = false;
}
// arrete les moteurs
void SkittleDetectorSimu::periodicTask()
{
    // TODO
    // Events->raise(EVENTS_SKITTLE_DETECTED);
}
