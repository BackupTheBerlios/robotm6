/**
 * @file RobotComponent.h
 *
 * @author Laurent Saint-Marcel
 *
 * Class de base dont herite tous les composants du robot. Elle definie des
 * fonctions permettant de definir le niveau de debug du composant et elle
 * donne acces a la configuration du robot (config.h)
 */

#ifndef __ROBOT_BASE_H__
#define __ROBOT_BASE_H__

#include <assert.h>

#include "classConfig.h"

// ============================================================================
// =============================  class RobotBase   ===========================
// ============================================================================

/** 
 * @class RobotBase
 * Class de base dont herite tous les composants du robot: inputs, move, uart\n
 * Elle definie des fonctions virtuelles de communes
 */
class RobotBase
{ 
  public:
    RobotBase(const char* name, ClassId classId);
    virtual ~RobotBase(){}
    /** @brief Cette fonction retourne le nom du composant */
    const char*        getName() const;
    /** @brief Cette fonction retourne le numéro du composant */
    ClassId            getClassId() const;
    /** @brief Cette fonction retourne le numéro du composant */
    ClassConfig*       getClassConfig() const;
    /** @return the verbose level of this class */
    VerboseLevel       verbose() const;
    /** @return set the verbose level (for log & debug) of this class */
    void               setVerboseLevel(const VerboseLevel level);
    
  protected:
    // const
    static const int NAME_LENGTH_MAX = 64;
    // members
    char               robotBaseName_[NAME_LENGTH_MAX];
    ClassConfig*       classConfig_;
};

// ============================================================================
// =========================  class RobotComponent   ==========================
// ============================================================================

/** 
 * @class RobotComponent
 * Class de base dont herite tous les composants du robot qui doivent etre
 * reinitialises quand le match recommence
 */
class RobotComponent : public RobotBase {
 public:
    RobotComponent(const char* name,
		   ClassId     classId) 
	: RobotBase(name, classId), init_(false) {}
    virtual ~RobotComponent(){}
    /** @brief fonction qui reinitialise le composant avant de commencer 
	une strategie */
    virtual bool reset()=0;
    /** @brief Retourne true si le composant est initialise correctement */
    bool isInitialized() { return init_; }
    /** @brief Indique si le composant est initialise correctement */
    void setInitialized(bool initDone) { init_ = initDone; }

 protected:
    bool init_;
};

// ============================================================================
// =============================  INLINE FUNCTIONS  ===========================
// ============================================================================

// ----------------------------------------------------------------------------
// RobotBase::RobotBase
// ----------------------------------------------------------------------------
inline RobotBase::RobotBase(const char*        name,
			    ClassId            classId)
{
    classConfig_ = ClassConfig::find(classId);
    strncpy(robotBaseName_, name, NAME_LENGTH_MAX);
    robotBaseName_[NAME_LENGTH_MAX-1]=0;
}

// ----------------------------------------------------------------------------
// RobotBase::name
// ----------------------------------------------------------------------------
inline const char* RobotBase::getName() const
{
    return robotBaseName_;
}

// ----------------------------------------------------------------------------
// RobotBase::classid
// ----------------------------------------------------------------------------
inline ClassConfig* RobotBase::getClassConfig() const
{
    return classConfig_;
}

// ----------------------------------------------------------------------------
// RobotBase::classid
// ----------------------------------------------------------------------------
inline ClassId RobotBase::getClassId() const
{
    if (classConfig_) {
        return classConfig_->id();
    } else {
        return CLASS_DEFAULT;
    }
}

// ----------------------------------------------------------------------------
// RobotBase::verbose
// ----------------------------------------------------------------------------
inline VerboseLevel RobotBase::verbose() const
{
    if (classConfig_) {
        return classConfig_->verbose();
    } else {
        return VERBOSE_DEFAULT;
    }
}

// ----------------------------------------------------------------------------
// RobotBase::setVerboseLevel
// ----------------------------------------------------------------------------
inline void RobotBase::setVerboseLevel(const VerboseLevel level)
{
    if (classConfig_) classConfig_->setVerboseLevel(level);
}

#endif /* __ROBOT_COMPONENT_H__ */
