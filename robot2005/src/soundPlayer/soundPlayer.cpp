#include <unistd.h>

#define   SOUND_INFO
#define   SOUND_SHM_INFO
#include "soundPlayer.h"
#include "robotShm.h"

#define LOG_CLASSID CLASS_SOUND
#include "log.h"

SoundPlayerCL* SoundPlayerCL::soundPlayer_=NULL;

// -------------------------------------------------------------------------
// SoundPlayerCL::SoundPlayerCL
// -------------------------------------------------------------------------
// Constructeur 
// -------------------------------------------------------------------------
SoundPlayerCL::SoundPlayerCL():
    shm_(NULL), shmIndex_(0)
{
    assert(!soundPlayer_);
    soundPlayer_=this;
    
    // create or connect to the shared memory
    bool existingShared=false;
    int res = RobotShm::create(SOUND_SHM_FILENAME, 
                               0644,
                               sizeof(SoundShmStruct),
                               (void**)(&(/*(void*)*/shm_)), 
                               existingShared,
                               false);
    if (res != 0 || !existingShared || shm_ == NULL) {
        LOG_WARNING("Sound player not started. You should run \"bin/soundPlayer\" "
                    "before this program\n");
    } else {
        LOG_OK("Initialisation done\n");
        shm_->isPlayingSound=false;
    }
    write(SOUND_CLEAR);
}


// -------------------------------------------------------------------------
// SoundPlayerCL::~SoundPlayerCL
// -------------------------------------------------------------------------
SoundPlayerCL::~SoundPlayerCL() 
{
    soundPlayer_ = NULL;
}

// -------------------------------------------------------------------------
// SoundPlayerCL::write
// -------------------------------------------------------------------------
// Met a jour les donnees de la shared memory 
// -------------------------------------------------------------------------
void SoundPlayerCL::write(SoundId sound, 
                          int data)
{
    if (!shm_) return;
    int counter=0;
    while(shm_->newData && counter++<5) usleep(10000);
    shm_->id      = sound;
    shm_->data    = data;
    shm_->newData = 1;
}

// -------------------------------------------------------------------------
// SoundPlayerCL::write
// -------------------------------------------------------------------------
// Envoie un son dans la shared memory pour qu'il soit joue
// @param priority SND_PRIORITY_PUSH_BACK: le son sera joue quand les 
//        autres qui ont ete ajoutes avant lui seront termines\n
//        SND_PRIORITY_URGENT: efface la liste des sons, stop le son
//        courant et joue ce nouveau son immediatement
// -------------------------------------------------------------------------
void SoundPlayerCL::play(SoundId sound, 
                         SoundPriority priority)
{
    write(sound, (int)priority);
}

// -------------------------------------------------------------------------
// SoundPlayerCL::clearStack
// -------------------------------------------------------------------------
// Arrete la music, les sons joues et efface la liste des sons a
// jouer (sorte de reset)
// -------------------------------------------------------------------------
void SoundPlayerCL::clearStack()
{
    write(SOUND_CLEAR);
}
