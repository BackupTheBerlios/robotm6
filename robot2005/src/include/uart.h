/**
 * @file uart.h
 *
 * @author Laurent Saint-Marcel
 *
 * gestion de la carte UART du PC104 et de toutes les cartes qui y sont 
 * nnectees ou qui sont connectees sur un port serie classiqu
 */
#ifndef __UART_H__
#define __UART_H__

#define USE_ROBOT_FIFO

#ifndef USE_ROBOT_FIFO
#include <deque>
#else
#include "robotFifo.h"
#endif

#include "robotBase.h"
#include "serialPort.h"
#include "mthread.h"

// TODO: call this UartManager instead of UartMgr?
#define UartMgr UartManagerCL::instance()

//#define TTYS0_NOT_USED
//#define TTYS1_NOT_USED

// ============================================================================
// ===========================  typedef and static  ===========================
// ============================================================================

typedef enum UartPort {
    UART_STTY0=0,
    UART_STTY1,
    UART_PORT_NBR
} UartPort;

typedef unsigned short UartPortMask;
typedef unsigned short UartPingId;
typedef char*          UartName;
typedef unsigned char  UartByte;
#ifndef USE_ROBOT_FIFO
typedef std::deque<UartByte>    UartBuf;
#else
typedef RobotFifo<UartByte, 50> UartBuf;
#endif
typedef bool (*UartFilterFun)(UartByte byte);

/** @brief Liste des cartes connectées sur un port de la carte UART */
typedef enum UartId {
    UART_NONE=0,
    UART_LCD_03,
    UART_COLOR_03,
    UART_VOLTAGE_03,
    UART_SERVO_03,
    UART_ENV_DETECTOR_03,

    UART_LCD_04,
    UART_ODOMETER_04,
    UART_SERVO_04,
    UART_VOLTAGE_04,
    UART_ENV_DETECTOR_04_0,
    UART_ENV_DETECTOR_04_1,
    UART_GONIO_04,  
    UART_CATAPULT_04,
    UART_K2000_04,

    UART_NBR
} UartId;

/** @brief Type d'uart: lecture directe sur le /dev/uart ou lecture filtree 
 * et bufferisee, ce qui permet aux cartes connectees d'etre maitre (cad de 
 * parler sans qu'on leur demande) 
 */
typedef enum UartType {
    UART_BASIC,
    UART_FILTER
}UartType;

/** @brief Informations definissant la carte qui est connectée sur un port de 
    l'UART */
typedef struct UartInfo {
    UartId     id;
    UartPingId pingId;
    UartType   type;
    char       name[64];
} UartInfo;

/** @brief Définition des caractéristiques de chacun de chacune des cartes de 
    l'UART */
static const UartInfo uartInfos_[UART_NBR] = {
  //  UART_ID     PING_ANSWER_ID  TYPE      NAME  
    {UART_NONE,            0x00, UART_BASIC,  "UART_NONE" },

    {UART_LCD_03,          0xA2, UART_BASIC,  "UART_LCD_03" },
    {UART_COLOR_03,        0xA3, UART_BASIC,  "UART_COLOR_03" },
    {UART_SERVO_03,        0xA4, UART_BASIC,  "UART_SERVO_03" },
    {UART_VOLTAGE_03,      0xAB, UART_BASIC,  "UART_VOLTAGE_03" },
    {UART_ENV_DETECTOR_03, 0xA9, UART_FILTER, "UART_ENV_DETECTOR_03" },

    {UART_LCD_04,            0xB2, UART_FILTER, "UART_LCD_04" },
    {UART_ODOMETER_04,       0xB7, UART_FILTER, "UART_ODOMETRE_04" },
    {UART_SERVO_04,          0xB4, UART_BASIC,  "UART_SERVO_04" },
    {UART_VOLTAGE_04,        0xBB, UART_BASIC,  "UART_VOLTAGE_04" },
    {UART_ENV_DETECTOR_04_0, 0xB8, UART_FILTER, "UART_ENV_DETECTOR_04_0" },
    {UART_ENV_DETECTOR_04_1, 0xB9, UART_FILTER, "UART_ENV_DETECTOR_04_1" },
    {UART_GONIO_04,          0xB5, UART_FILTER, "UART_GONIO_04" },
    {UART_CATAPULT_04,       0xB1, UART_FILTER, "UART_CATAPULT_04" },
    {UART_K2000_04,          0xBC, UART_BASIC,  "UART_K2000_04" }
};

static const UartByte uartPingReq_ = 0xAA;

static const Millisecond UART_DEFAULT_TIMEOUT=30;

// ============================================================================
// ================================  class Uart   =============================
// ============================================================================

/** 
 * @class Uart
 * Gestion d'un port de la carte Uart: read/write/ping. Les fonctions read et
 * write lisent/ecrivent directement sur le module uart.
 */
class Uart : public RobotBase {
 public:
    Uart();
    virtual ~Uart();
    
    /**
     * Ouvre une connection sur le port spécifié
     */
    virtual bool open(UartPort   port);
    /**
     * Ouvre une connection sur le port spécifié et défini le type de carte 
     * attendue lors d'une requete ping
     */
    bool open(UartPort port, UartInfo deviceInfo);
    /**
     * Ferme la connection
     */
    virtual bool close();
    /**
     * Reset la connection (la ferme puis la réouvre)
     */
    virtual bool reset();
  
    /**
     * Envoie une requete d'identification de la carte connectee. 
     * Si l'UartInfo a ete specifie, la fonction retourne true seulement si 
     * c'est la carte specifiee qui repond.
     * Si l'uartInfo n'a pas ete specifie dans la commande open, la fonction
     * met a jour l'UartInfo en fonction de l'id retourne par la carte et
     * ping retourne true. Si la carte ne repond pas ou que l'id n'est pas
     * defini dans uartInfos_, ping retourne false
     */
    bool ping();

    /** 
     * Lit 1 octet sur l'UART
     * Retourne true en cas de succés, false en cas d'erreur. En cas 
     * d'erreur, length contient le nombre réel d'octets recus 
     */
    virtual bool read( UartByte* buf);

    /** 
     * Lit length octets sur l'UART
     * Retourne true en cas de succés, false en cas d'erreur. En cas 
     * d'erreur, length contient le nombre réel d'octets recus 
     */
    virtual bool read( UartByte* buf, unsigned int& length);
    /** 
     * Ecrit un octet sur l'UART. 
     * Retourne true en cas de succés, false sinon
     */
    virtual bool write(UartByte  buf);
    /** 
     * Ecrit length octets sur l'UART
     * Retourne true en cas de succés, false en cas d'erreur. En cas d'erreur,
     * length contient le nombre réel d'octets envoyés
     */
    virtual bool write(UartByte* buf, unsigned int& length);

    /**
     * Met à jour le timeout de lecture
     */
    virtual bool setReadTimeout(Millisecond timeout);
    /**
     * Retourne le nom de la carte retournée ou UART_NONE si la carte n'a
     * pas ete identifiée par une requete ping
     */
    const char* name();
    /**
     * Retourne l'UartInfo correspondant au pingId spécifié. 
     * Retourne uartInfos_[UART_NONE] en cas d'id inconnu.
     */
    static UartInfo getUartInfo(UartPingId pingId);

    UartInfo getInfo()  {return info_;}
    bool     isOpened() {return fd_>=0;}
    bool checkMSerieIsAlive();
   /**
     * Quand il y a une erreur de com il risque d'y avoir un decalage donc la prochaine 
     * fois qu'on ecrit, on vide le buffer
     */
    void comErrorDetected();

 protected:
    /** 
     * Envoie une requète de ping et attend la réponse
     */
    bool requestPingId(UartPingId& pingId);
    /**
     * Vide le buffer de lecture
     */
    virtual void clearFifo();

 protected:
    UartPort      port_;
    int           fd_;
    UartInfo      info_;
    // when using /dev/stty0 or /dev/stty1
    SerialPort    serialPort_;
    bool          isSerial_;
    bool          needClearFifo_;
};

// ============================================================================
// ============================  class UartBuffer   ===========================
// ============================================================================

/** 
 * @class UartBuffer
 * Gestion d'un port de la carte Uart qui utilise une fonction qui filtre les 
 * données envoyées, ce qui permet de gérer les events et le cas ou la carte 
 * connectée est maitre (elle parle quand elle veut)
 */
class UartBuffer : public Uart {
 public:
    UartBuffer();
    virtual ~UartBuffer();
    /**
     * Ouvre une connection sur le port spécifié
     */
    virtual bool open(UartPort port);
    virtual bool close();
    /**
     * Reset la connection (la ferme puis la réouvre) et clear le buffer
     */
    virtual bool reset();

    /** 
     * Lit length octets sur l'UART
     * Retourne true en cas de succés, false en cas d'erreur. En cas d'erreur, 
     * length contient le nombre réel d'octets recus 
     */
    virtual bool read(UartByte* buf,  unsigned int& length);

    /** 
     * Lit 1 octet sur l'UART
     * Retourne true en cas de succés, false en cas d'erreur. En cas d'erreur, 
     * length contient le nombre réel d'octets recus 
     */
    virtual bool read(UartByte* buf);
    
    /**
     * Enregistre une fonction qui filtre les données recues sur l'uart. Cette
     * fonction est appelée pour chaque octet recu. Elle doit etre non 
     * bloquante et doit retourner false pour les octects qui peuvent 
     * apparaitre dans le buffer quand on lance la commande read
     */
    void registerFilterFunction(UartFilterFun filter);

    /**
     * Met à jour le timeout de lecture
     */
    bool setReadTimeout(Millisecond timeout);

 protected:
    /**
     * Tache periodique qui remplie le buffer. 
     * === Cette function est reservee au thread robotTimer ===
     */
    void fileDescriptorTask(int fd);
    friend void UartBufferFileDescriptorTask(void* userData, int fd);
    virtual void clearFifo();

 protected:
    UartFilterFun filterFn_;
    UartBuf*      buffer_;
    Millisecond   timeout_;
    pthread_mutex_t mutex_;
    bool fillBuffer();
};

// ============================================================================
// =============================  class UartManager   =========================
// ============================================================================
class LcdCL;
class OdometerCL;

/** 
 * @class UartManager
 * Gestion de la carte Uart: autodetection des cartes presentes et
 * auto-assignation des cartes sur le port surlequel elles sont connectees
 */
class UartManagerCL : public RobotComponent
{
 public:
    /** 
     * Crée un UartManager, ouvre les ports specifiés dans robotConfig et y
     * attache les uartInfos correspondant 
     */
    UartManagerCL(bool dontScan=false);
    virtual ~UartManagerCL();
    static UartManagerCL* instance();
    // TODO: remove this method and always instanciate with default-param? [flo]
    static UartManagerCL* instanceNoCheck();

    bool reset();
    bool validate();

    /** 
     * Retourne l'uart correspondant à un UartId donné si le port 
     * correspondant a été ouvert par le constructeur et qu'il y a 
     * bien une carte connectée
     */
    Uart* getUartById(UartId id);
    /** 
     * Retourne l'uart correspondant à un port donné si le port 
     * correspondant a été ouvert par le constructeur et qu'il y a 
     * bien une carte connectée
     */
    Uart* getUartByPort(UartPort port);

    /** 
     * Scan les port qui sont encore libres et alloue le device correpondant
     * Utilise pendant l'auto-verification du robot. Si un composant est en
     * mode simule, c'est qu'il n'a pas ete detecte donc on peut appeler
     * cette fonction pour essayer de le detecter
     */
    void scanAndAlloc();

    /**
     * Envoie dans log_info la liste des ports connectes
     */
    void listConnected();

    /** 
     * Coupe les actionneurs "dangereux" en cas d'arret d'urgence 
     * ou Ctr+C 
     */
    void emergencyStop();
    bool checkMSerieIsAlive();
 protected:
    /**
     * Scanne tous les port, rempli uartListByPort_ & uartListById_ avec la 
     * liste des cartes detectees et laisse la connection ouverte pour de
     * futurs acces
     */
    int  searchAndOpen();
    /**
     * Close all uart ports
     */
    void close();
    /**
     * Scanne tous les ports, met a jour infos avec les cartes detectees et 
     * ferme les ports
     */
    void scan(UartInfo infos[UART_PORT_NBR]);
    /** 
     * Initialise les instances des controleurs des cartes: Lcd*, Servo*...
     */
    void allocDevices();

    /**
     * @brief Initialise la class Lcd en mode 04 ou simu en fonction
     * de la detection de la carte pendant la phase de scan
     * @see Lcd 
     */
    void allocLcd();
    /**
     * @brief Initialise la class Odometer en mode 04 ou simu en fonction
     * de la detection de la carte pendant la phase de scan
     * @see Odometer 
     */
    void allocOdometer();
 
    friend int main(int argc, char* argv[]);

 protected:
    static UartManagerCL* uartManager_;
    Uart* uartListByPort_[UART_PORT_NBR];
    Uart* uartListById_[UART_NBR];

 private:
    // liste des composant initialise par le uartManager et qui se connectent 
    // sur un uart
    LcdCL*         lcd_;
    OdometerCL*    odometer_;
};

// ============================================================================
// =============================   Inline functions   =========================
// ============================================================================

// ----------------------------------------------------------------------------
// UartManager::instance
// ----------------------------------------------------------------------------
inline void Uart::comErrorDetected() 
{
    needClearFifo_ = true;
    checkMSerieIsAlive();
}

// ----------------------------------------------------------------------------
// UartManager::instance
// ----------------------------------------------------------------------------
inline UartManagerCL* UartManagerCL::instance() 
{
  assert(uartManager_);
  return uartManager_;
}

// ----------------------------------------------------------------------------
// UartManager::instanceNoCheck
// ----------------------------------------------------------------------------
inline UartManagerCL* UartManagerCL::instanceNoCheck() 
{
  return uartManager_;
}


#endif // __UART_H__
