// variables static definies dans robot*.h
#include "classConfig.h"

// --------------------------------------------------------------------------
// ClassConfig.h
// --------------------------------------------------------------------------
static ClassConfig* classConfigArray_[CLASS_NBR];
static bool initClassConfig_=true;

void initClassConfig() {
    if (!initClassConfig_) return;
    initClassConfig_ = false;
    for (int i=0; i<CLASS_NBR; i++) {
	classConfigArray_[i] = new ClassConfig((ClassId)i);
    }
}

ClassConfig* ClassConfig::find(ClassId id)
{
    initClassConfig();
    return classConfigArray_[(int)id];
}
