/** 
 * @file classConfig.h
 *
 * Defini la configuration de chaque module du robot pour les logs:
 * permet de savoir quel est le niveau de debug de chaque partie de code
 */

#ifndef __CLASS_CONFIG_H__
#define __CLASS_CONFIG_H__

#include "robotTypes.h"

// ===========================================================================
// enum ClassId
// ===========================================================================

/** @brief Liste de tous les noms de classe */
typedef enum ClassId {
  CLASS_DEFAULT=0,

  CLASS_ROBOT_TIMER,
  CLASS_SHM,
  CLASS_MTHREAD,
  CLASS_SOCKET,
  CLASS_LOCK,

  CLASS_SIMULATOR,
  CLASS_VIEWER_3D,
  CLASS_LOG,
  CLASS_SOUND,
  CLASS_EVENTS_MANAGER,

  CLASS_ROBOT_DEVICES,
  CLASS_UART_MANAGER,
  CLASS_UART,
  CLASS_UBART,
  CLASS_UBART_MULTIPLEXER,
  CLASS_IO_MANAGER,
  CLASS_SERIAL_PORT,
  CLASS_SERIAL_DEVICE,
  CLASS_MOTOR_ODOM,
  CLASS_LCD,
  CLASS_ODOMETER,
  CLASS_BUMPER,
  CLASS_ENV_DETECTOR,
  CLASS_MOTOR,
  CLASS_BUTTON,
  CLASS_SERVO,
  CLASS_ALIM,
  CLASS_TESLA, // carte electro aimant
  CLASS_SKITTLE_DETECTOR,
  CLASS_CRANE,  // grue electro aimant
 
  CLASS_MOVEMENT_MANAGER,
  CLASS_MOVE,
  CLASS_ROBOT_POSITION,
  CLASS_MOVEMENT,

  CLASS_ROBOT_MAIN,
  CLASS_STRATEGY,
  CLASS_HLI,
  CLASS_TRAJGEN,
  CLASS_SHORTEST_WAY,
  CLASS_EXPLORE,

  CLASS_NBR
} ClassId;

// ===========================================================================
// class ClassConfig
// ===========================================================================

/**
 * @class ClassConfig 
 * Parametres qui configurent une class
 */
class ClassConfig {
 private:
  VerboseLevel verbose_;
  ClassId      id_;

 public:
  ClassConfig(ClassId id);
  /** @brief Retourne l'id de la classe */
  ClassId             id() const;
  /** @brief Retourne le niveau de debug */
  VerboseLevel        verbose() const;
  /** @brief met a jour le niveau de debug */
  void                setVerboseLevel(VerboseLevel level);
  /** @brief Renvoie la classConfig correspondant a un id donne */
  static ClassConfig* find(ClassId id);
};

// ===========================================================================
// inline functions
// ===========================================================================

// ---------------------------------------------------------------------------
// ClassConfig::ClassConfig
// ---------------------------------------------------------------------------
inline ClassConfig::ClassConfig(ClassId id) :
    verbose_(VERBOSE_DEFAULT), id_(id) 
{
}

// ---------------------------------------------------------------------------
// ClassConfig::verbose
// ---------------------------------------------------------------------------
inline VerboseLevel ClassConfig::verbose() const
{
    return verbose_;
}

// ---------------------------------------------------------------------------
// ClassConfig::setVerboseLevel
// ---------------------------------------------------------------------------
inline void ClassConfig::setVerboseLevel(VerboseLevel level)
{
    verbose_ = level;
}

// ---------------------------------------------------------------------------
// ClassConfig::setVerboseLevel
// ---------------------------------------------------------------------------
inline ClassId ClassConfig::id() const
{
    return id_;
}



#endif // __CLASS_CONFIG_H__
