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
// ==========================  class RobotIODevice   =========================
// ============================================================================

/** 
 * @brief MAX_NBR_IO est la taille conseille pour le tableau passe a 
 * RobotIODevice::autoTest
 * Il garantit que le tableau ne sera jamais trop peti et evite les
 * allocations dynamiques
 */
#define ROBOT_IO_DEVICE_MAX_NBR_IO 32

/** 
 * @class RobotIODevice
 * Class virtuelle dont herite tous les composants capteurs et actuateurs du 
 * robot
 */
class RobotIODevice: public RobotComponent 
{ 
 public:
    RobotIODevice(const char* name, 
		  ClassId classId,
		  int nbrIO=0);

    /** 
     * @brief Cette fonction retourne TRUE si le device est bien connecte
     * En 2004 cete fonction ne doit pas etre utilisee
     */
    virtual bool ping()=0;
    /**
     * @brief Retourne true si le composant est simulé. Si le composant est
     * simule c'est que la carte correspondante n'a pas repondu! Cette 
     * fonctionnalite remplace le ping pour les cartes UART!
     */
    virtual bool isSimu() const=0;

    /** 
     * @brief Cette fonction retourne TRUE si tous les capteurs
     * fonctionnent correctement. Quand elle retourne FALSE, le 
     * tableau ioStatus est update. Chacque entree correspond a une
     * io (le tableau doit etre de taille au moins getNbrIO()
     */
    virtual bool autoTest(bool* ioStatus) { return false;}
    /**
     * @brief Retourne le nom du capteur specifie par son id
     */
    virtual const char* getIoName(int ioID) const { return "";}
    /**
     * @brief Retourne le nombre de capteurs sur la carte donc la 
     * taille du tableau autoTest(bool* ioStatus)
     */
    virtual int getNbrIO() const { return nbrIO_;}

 private:
    int nbrIO_;
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

// -----------------------------------------------------------------------
// RobotIODevice::RobotIODevice
// -----------------------------------------------------------------------
inline RobotIODevice::RobotIODevice(const char* name, 
				    ClassId     classId,
				    int         nbrIO)
    : RobotComponent(name, classId), nbrIO_(nbrIO) 
{
    assert(nbrIO_ < ROBOT_IO_DEVICE_MAX_NBR_IO);
}
#endif /* __ROBOT_COMPONENT_H__ */
