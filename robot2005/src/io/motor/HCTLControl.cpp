
#include <sys/io.h>
#include <errno.h>
#include <stdio.h>

#include <sys/time.h>
#include <unistd.h>

#include "HCTLControl.h"
#include "hct.h"

//#define HCTL_NEW_VALUES

#ifdef HCTL_NEW_VALUES
#define STARTUP_ACCELERATION 0x40 
#else
#define STARTUP_ACCELERATION 0x10  // 0x20
#endif

/***************************************************
       S E T T I N G   M O T O R S   S P E E D
****************************************************/
void set_speed( int vitesse1, int vitesse2)
{
    //vit_com_int( 1, vitesse1);
    vit_com_int( 2, -vitesse2 );
    vit_com_int( 1, -vitesse1);
  
    
    // rajouté, pour que le HCTL exécute la commande
    com_int_vit(2);
    com_int_vit(1);

}

void hctlSetMotorSpeed(int rightSpeed, int leftSpeed)
{
    if (rightSpeed > 127)  rightSpeed =  127;
    if (leftSpeed > 127)   leftSpeed  =  127;
    if (rightSpeed < -127) rightSpeed = -127;
    if (leftSpeed < -127)  leftSpeed  = -127;
  
    set_speed((int)rightSpeed, (int)leftSpeed);
}


/***************************************************
       G E T T I N G   M O T O R S   P O S
****************************************************/

int hctlGetRightPos()
{
  int dummy;
  dummy =-lecture_position_1();
  return dummy;
}

int hctlGetLeftPos()
{
  int dummy;
  dummy=-lecture_position_2();
  return dummy;
}


/***************************************************
            C O N F I G U R A T I O N
****************************************************/

/* This is a copy of the 1998's source code */
static void initHCTL()
{
    int A1, B1, K1, T1, offset1;
    int A2, B2, K2, T2, offset2;
    
    // Fait un software reset
    reinitialisation_logiciel( 2 );
    reinitialisation_logiciel( 1 );
    
    initialisation_repos( 1 );
    initialisation_repos( 2 );
    
    // Clear les Position register a 0:
    raz_dist(1);
    raz_dist(2);
    
    // trois bricoles dans le Status Register...
    config_out( 1, 0 );
    config_out( 2, 0 );
    
#ifdef  HCTL_NEW_VALUES
    A1 = A2 = 0xE5;
    B1 = B2 = 0x40;
    K1 = K2 = 0x30;
    //2003  K1 = K2 = 0x20; 
    //2000  K1 = K2 = 0x16;
    T1 = T2 = 0x80;
#else
    A1 = A2 = 0xE5;
    B1 = B2 = 0x40;
    K1 = K2 = 0x20;
    //2003  K1 = K2 = 0x20; 
    //2000  K1 = K2 = 0x16;
    T1 = T2 = 0x40;
#endif

    offset1 = offset2 = 0;
    filter( 1, A1, B1, K1 );
    filter( 2, A2, B2, K2 );
    timer( 1, T1 );
    timer( 2, T2 );
    pwm( 1, offset1 );
    pwm( 2, offset2 );  
} 


/* This is used to grant access to the io ports */
void makeIOPerm()
{
    if (iopl(3)) { 
        perror("Error IOPerm..."); 
        _exit(1); 
    } 
}


static void setMode()
{
    com_int_vit( 1 );
    com_int_vit( 2 );
    
    set_speed(0,0);
    mode_commande(1);
    mode_commande(2);
}

void hctlSetAcceleration(int acc)
{  
    struct deux_reg vit1, vit2;
    
    vit1.octet1=0;
    vit2.octet1=0;
    vit1.octet2=acc;
    vit2.octet2=acc;
    acc_trap( 1, vit1.octet1, vit1.octet2 );
    acc_trap( 2, vit2.octet1, vit2.octet2 );
}



/*****************************************
    I N I T   C L E A N U P   R E S E T
******************************************/
	
void hctlInit( void )
{
    makeIOPerm();
    initHCTL();
    hctlSetAcceleration(STARTUP_ACCELERATION);
    setMode();
    hctlSetMotorSpeed(0,0);
}

void hctlCleanUp(void)
{  
}

void hctlIdle()
{
   initialisation_repos(1);
   initialisation_repos(2);
}

/***************************************************
       G E T T I N G   P W M   V A L U E
****************************************************/
char hctlGetRightPWM()
{
    unsigned char res;
    read_pwm(1,&res);
    return (signed char)res;
}

char hctlGetLeftPWM()
{
    unsigned char res;
    read_pwm(2,&res);
    return (signed char)res;
}






