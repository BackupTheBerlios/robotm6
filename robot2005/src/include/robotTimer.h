#ifndef __ROBOT_TIMER_H__
#define __ROBOT_TIMER_H__

#include <sys/time.h>
#include <unistd.h>

#include "robotBase.h"

#define Timer RobotTimerCL::instance()

typedef void (*TimerFunction)(void* userData, Millisecond time);
static const int TIMER_FUNCTION_NAME_LENGTH=32;
typedef struct TimerRegisterFunction {
  TimerFunction fn;
  char fnName[TIMER_FUNCTION_NAME_LENGTH];
  void* userData;
} TimerRegisterFunction;
static const Millisecond ROBOT_TIMER_RESET_VALUE = TIME_NEVER;

/** 
 * @class RobotTimerCL
 * Gestion de taches periodiques (hors mis I/O et move/position) : actions 
 * a exectuter a une date precise, gestion du temps ecoule depuis le debut 
 * du match
 */
class RobotTimerCL : public RobotComponent 
{
 public:
  virtual ~RobotTimerCL();
  /** Retourne un pointeur vers le singleton RobotTimer */
  static RobotTimerCL* instance();
  /** @brief Retourne l'heure actuelle, <0 avant le debut du match */
  Millisecond time();
  /** @brief Met a jour l'heure */
  void set(Millisecond time);
  /** @brief Met le compteur a 0 (ce qui le demarre et appelle les timerCallback */
  void startMatch();
  /** @brief unregister toutes les fonctions timer et remet l'heure a -1. le 
   * compteur ne sera pas incremente avant startMatch, mais les taches 
   * periodiques sont quand meme lancees 
   */
  bool reset();
  /** 
   * @brief Enregistre une fonction qui sera executee quand time()==time 
   * ATTENTION, cette fonction doit etre rapide est non bloquante
   */
  void registerTimerFunction(TimerFunction fn, 
			     const char* fnName,
			     void* userData,
			     Millisecond time);
  /** 
   * @brief Enregistre une fonction qui est executee regulierement.
   * ATTENTION, cette fonction doit etre rapide est non bloquante
   */
  void registerPeriodicFunction(TimerFunction fn, 
				const char* fnName,
				void* userData);

  /** @brief tic inits  a chronometer, identified  by the tv_before that  it is
   * passed
   */
  static void tic(struct timeval* tv_before );
  /** @brief tac tells the number of  seconds ellapsed since the last call
   *  to  tac, or  the call  to tic  if tac  is the  first call  on that
   *  chronometer
   */
  static Millisecond tac(struct timeval* tv_before);
  
  void periodicTask();
  friend void* RobotTimerThreadBody(void*);
  
 private:
  RobotTimerCL();
  
  static RobotTimerCL* timer_;
  Millisecond time_;
  std::deque< std::pair< Millisecond, TimerRegisterFunction > > callbacks_;
  std::deque< TimerRegisterFunction > periodicTasks_;
  pthread_t thread_;
};

// ===========================================================================
// inline functions
// ===========================================================================

// ---------------------------------------------------------------------------
// RobotTimerCL::instance
// ---------------------------------------------------------------------------
inline RobotTimerCL* RobotTimerCL::instance() 
{
    if (!timer_) timer_ = new RobotTimerCL();
    return timer_;
}

// ---------------------------------------------------------------------------
// RobotTimerCL::set
// ---------------------------------------------------------------------------
inline void RobotTimerCL::set(Millisecond time)
{
  time_ = time;
}

#endif // __ROBOT_TIMER_H__
