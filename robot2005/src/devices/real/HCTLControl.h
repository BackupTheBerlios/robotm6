#ifndef __HCTL_CONTROL__
#define __HCTL_CONTROL__

#ifdef __cplusplus
extern "C" {
#endif

  //extern void makeIOPerm();
  extern void hctlInit( void );
  extern void hctlCleanUp(void); 
  extern void hctlSetMotorSpeed( int leftSpeed, int rightSpeed );
  extern int  hctlGetRightPos();
  extern int  hctlGetLeftPos();
  extern char hctlGetRightPWM();
  extern char hctlGetLeftPWM();
  extern void hctlIdle(); // reset
  extern void hctlSetAcceleration(int acc);

#ifdef __cplusplus
}
#endif

#endif // __HCTL_CONTROL__
