/**
 * @file ioManager.h
 * 
 * Partie du code gerant toutes les entrees/sorties: celles qui sont sur la 
 * carte E/S, les cartes connectes sur la carte UART et les cartes connectees
 * sur les ports serie du PC.
 * Un thread verifie periodiquement l'etat des E/S: les capteurs et cartes 
 * enregistrent des callbacks executes periodiquement et la classe gere aussi
 * des callback sur des fileDescriptor. Ainsi, les cartes atmels qui sont
 * connectees a la carte UART et qui parlent toute seules enregistre un 
 * callback sur leur filedescriptor et quand une nouvelle donnee est 
 * disponible sur l'UART ces callbacks sont appeles. On peut utiliser le 
 * meme principe avec la carte E/S avec le module dridaqFifo. Par contre 
 * en dridaqDirect, c'est juste un callback periodic qui va lire l'etat 
 * des Entrees de la carte regulierement
 * 
 * @Author Laurent Saint-Marcel
 */

#ifndef __IO_MANAGER_H__
#define __IO_MANAGER_H__

#include "mthread.h"
#include "robotBase.h"

#include <unistd.h>
#include <map>

// ============================================================================
// class & struct
// ============================================================================
#define IOMGR IoManagerCL::instance()
#define IoManager IoManagerCL::instance()

class UartManagerCL;

typedef void (*IoManagerCallBack)(void*, int); //void* userdata, int fileDescriptor

typedef struct IoManagerCBData {
    int fd;
    IoManagerCallBack callback;
    void* userData;
    char callbackName[100];

    IoManagerCBData(): fd(-1), callback(NULL), userData(NULL) {
        callbackName[0]=0;
    }
} IoManagerCBData;

typedef std::map<int, IoManagerCBData> IoManagerCBDataList;

// ============================================================================
// class IoManager
// ============================================================================

/** 
 * @class IoManager
 * Gestion basic (initialisation et reset) basic de toutes les cartes 
 * connectées sur les ports séries ou sur la carte d'entrées sorties
 */
class IoManagerCL : public RobotComponent
{
 public:
  /** @brief Constructeur. Il ne peut etre appele qu'une fois */
  IoManagerCL();
  /** @brief Destructeur */
  ~IoManagerCL();
  /** @brief Retourne un pointeur vers l'instance unique de cette classe */
  static IoManagerCL* instance();
   /** @brief Retourne un pointeur vers l'instance unique de cette classe */
  static IoManagerCL* instanceNoCheck();
  /** @brief Reset le composant */
  bool reset();
  /** @brief Retourne true si le composant est teste et qu'il n'a pas de bug */
  bool validate();

  /** 
   * @brief Coupe les actionneurs "dangereux" en cas d'arret d'urgence 
   * ou Ctr+C 
   */
  void emergencyStop();

 protected:
  UartManagerCL* uartMgr_;

 protected:
  /** 
   * @brief Enregistre un callback appele quand un nouveau message arrive 
   * sur le file descriptor.
   * Utilise pour attendre des messages sur les ports de l'uart et sur celui 
   * de dridaq
   */
  void registerFileDescriptor(int fd, 
			      IoManagerCallBack cb, 
			      const char* cbName, 
			      void* data);
  /**
   * @brief De-enregistre un callback sur un filedescriptor 
   * @see registerFileDescriptor
   */
  void unregisterFileDescriptor(int fd);

  friend class UartBuffer;

  /** 
   *@brief tache periodik qui attend qu'un file descriptor soit modifie 
   * pour appeler les callbacks correspondant
   */
  void periodicTask();
  friend void* IoManagerThreadBody(void*);

 private:
  int maxFd_;
  fd_set listenedFds_;
  IoManagerCBDataList cbList_;
  static IoManagerCL* ioManager_;
  pthread_t thread_;
};

// ============================================================================
// Inline functions
// ============================================================================

// ----------------------------------------------------------------------------
// IoManager::instance
// ----------------------------------------------------------------------------
inline IoManagerCL* IoManagerCL::instance()
{
  assert(ioManager_);
  return ioManager_;
}

// ----------------------------------------------------------------------------
// IoManager::instance
// ----------------------------------------------------------------------------
inline IoManagerCL* IoManagerCL::instanceNoCheck()
{
  return ioManager_;
}

#endif // __IO_MANAGER_H__
