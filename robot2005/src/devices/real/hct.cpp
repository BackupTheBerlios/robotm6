#include "hct.h"

#include <stdio.h>

#define inline
#ifndef __hpux__
#include <asm/io.h>   /* for PC io access */
#endif 

#ifdef TEST_MAIN
#ifndef DEBUG
#define DEBUG
#endif
#endif

#ifdef DEBUG
#define debug_printf printf
#else
#define debug_printf(arg...) do {}while(0)
#endif

// ================================== e_s.cpp =============================
namespace {
  // D'après Bitch :
#define addr_hctl_1   0x308
#define data_hctl_1   0x30A

#define addr_hctl_2   0x30C
#define data_hctl_2   0x30F


#define stop_hctl     0x302
#define limit_hctl    0x304
#define reset_hctl    0x306



#define   VIDE             0xFF
#define   ZERO             0x00

  void outport (unsigned short port, unsigned   char   value)
  {
    outb_p (value, port);
  }
  
  int inport (unsigned short port)
  {
    return inb_p (port);
  }

  void reset_hctls(){
    outport(reset_hctl,0x11);
  }
  
  unsigned char lire_registre(int hctl, int adr);
  void ecrire_registre(int hctl, int adr, unsigned char val);
  
  void ecrire_registre(int hctl, int adr, unsigned char val)
  {
    int data_hctl, addr_hctl;
    unsigned char relecture;
    //  unsigned char valeur;
    
    if (hctl==1)
      {data_hctl = data_hctl_1; addr_hctl = addr_hctl_1; }
    else
      {data_hctl = data_hctl_2; addr_hctl = addr_hctl_2; }
    
    outb(adr,addr_hctl);
    outb(val,data_hctl);        /* ecriture de la donnee    */
    // printf("fin de ecrire_registre\n");
    
    if ( 0 && adr != 5 && ( relecture = lire_registre(hctl,adr)) !=val)
      {
	//      ecrire_registre(hctl,adr,val);
	//      printf("ERREUR ECRITURE BUS adr=%x val=%x relu=%x\n\n\b", adr,(int)val,(int)relecture);
      }
    
  }

  
  unsigned char lire_registre(int hctl, int adr)
  {
    int data_hctl, addr_hctl;
    unsigned char valeur;
    //  char r_w, ale, cs, oe;
  
    /*  printf("Momentanement INTERDIT..."); return 0; exit (-1);*/
    
    if (hctl==1)
      {data_hctl = data_hctl_1; addr_hctl = addr_hctl_1; }
    else
      {data_hctl = data_hctl_2; addr_hctl = addr_hctl_2; }
    

    outb(adr,addr_hctl);              /* Ecrire l'adresse sur le bus */
    
    /* Lecture de la donne sur le bus  */
    valeur=inb(data_hctl);
    
    return(valeur);

  }


  void print_bin(byte c){
    int i;
    for(i=7; i>=0;--i){
      if ( c & (1<<i) ) {
	debug_printf("1"); } 
      else {
	debug_printf("0");
      }
      
    }
  }
}

// =================================== hct.cpp ===========================

int lecture_position_1()
{
  int posi=0;
  struct trois_reg position1;

  debug_printf("lecture_position_1\n");
  pos_reelle(1,&position1);
  debug_printf("Pos1 : %x%x%x", position1.octet1, position1.octet2, position1.octet3);
  // posi=0x00ffffff-((65536*position1.octet1)+ (256* position1.octet2)+  position1.octet3);
  posi=0;
  posi=posi|(int)position1.octet1;
  posi=posi<<8;
  posi=posi|(int)position1.octet2;
  posi=posi<<8;
  posi=posi|(int)position1.octet3;

  if (posi>0x00800000) {
    posi=~posi;
    posi=posi+1;
    posi=0x00ffffff&posi;
    posi=-posi;
  }
  // posi=0x00ffffff-posi;
  //  debug_printf("posi utilise en in roue 1=%d",posi);
  return posi;
}

int lecture_position_2()
{
  int posi=0;
  struct trois_reg position2;
  debug_printf("lecture_position_2\n");

  pos_reelle(2,&position2);
  //  debug_printf(" Pos2 : %x%x%x\n", position2.octet1, position2.octet2, position2.octet3);
  //  posi=(65536*position2.octet1)+ (256* position2.octet2)+  position2.octet3;
  posi=0;
  posi=posi|(int)position2.octet1;
  posi=posi<<8;
  posi=posi|(int)position2.octet2;
  posi=posi<<8;
  posi=posi|(int)position2.octet3;

  if (posi>0x00800000) {
    posi=~posi;
    posi=posi+1;
    posi=0x00ffffff&posi;
    /*   posi=-posi; */
  }
  else posi=-posi; // modifie 
  // debug_printf("posi en int roue 2=%d",posi);

  return posi;
}

void reinitialisation_materielle(){
  reset_hctls();
}

void reinitialisation_logiciel(int hctl)
{
  debug_printf("reinitialisation_logiciel(hctl %d)\n", hctl);
  ecrire_registre(hctl,0x5,0x0);
}

void initialisation_repos(int hctl)
{
  debug_printf("initialisation_repos(hctl %d)\n",hctl);

  ecrire_registre(hctl,0x5,0x1);
}

void alignement(int hctl)
{
  debug_printf("alignement(hctl %d)\n",hctl);
  ecrire_registre(hctl,0x5,0x2);
}


void lire_config(int hctl,byte* status)
{
  debug_printf("lire_config(hctl %d)\n",hctl);
  *status=lire_registre(hctl,0x07);
}


void mode_commande(int hctl)
{
  debug_printf("mode_commande(hctl %d)\n",hctl);
  ecrire_registre(hctl,0x5,0x3);
}


void com_position(int hctl)
{
  debug_printf("com_position(hctl %d)\n",hctl);
  ecrire_registre(hctl,0x0,0x0);
  ecrire_registre(hctl,0x0,0x3);
  ecrire_registre(hctl,0x0,0x5);
}

void com_prop_vit(int hctl)
{
  debug_printf("com_prop_vit(hctl %d)\n",hctl);
  ecrire_registre(hctl,0x0,0x0);
  ecrire_registre(hctl,0x0,0xB);
  ecrire_registre(hctl,0x0,0x5);
}

void com_int_vit(int hctl)
{
  debug_printf("com_int_vit(hctl %d)\n",hctl);
  ecrire_registre(hctl,0x0,0x0);
  ecrire_registre(hctl,0x0,0x3);
  ecrire_registre(hctl,0x0,0xD);
}

void com_prof_trap(int hctl)
{
  debug_printf("com_prof_trap(hctl %d)\n",hctl);
  ecrire_registre(hctl,0x0,0x8);
  ecrire_registre(hctl,0x0,0x3);
  ecrire_registre(hctl,0x0,0x5);
}

void pos_com(int hctl,byte pos_fort,byte pos,byte pos_faible)
{
  debug_printf("pos_com(hctl %d)\n",hctl);
  ecrire_registre(hctl,0x0C,pos_fort);
  ecrire_registre(hctl,0x0D,pos);
  ecrire_registre(hctl,0x0E,pos_faible);
}

void pos_reelle(int hctl,struct trois_reg *pos)
{
  debug_printf("pos_reelle(hctl %d)\n",hctl);
  pos->octet3=lire_registre(hctl,0x14);         /* poids faible */
  pos->octet2=lire_registre(hctl,0x13);
  pos->octet1=lire_registre(hctl,0x12);         /* poids fort   */
}

void vit_reelle(int hctl,struct deux_reg *vit)
{
  debug_printf("vit_reelle(hctl %d)\n",hctl);
  vit->octet2=lire_registre(hctl,0x34);         /* poids faible */
  vit->octet1=lire_registre(hctl,0x35);         /* poids fort   */
}

void vit_com(int hctl,byte pos_fort,byte pos_faible)
{
  debug_printf("vit_com(hctl %d)\n",hctl);

  if (hctl==2) {
    ecrire_registre(hctl,0x24,pos_fort);
    ecrire_registre(hctl,0x23,0xff);
  } else {
    ecrire_registre(hctl,0x24,-pos_fort);
    ecrire_registre(hctl,0x23,0x00);
  }
}

void vit_com1(byte pos_fort, byte pos_faible,int signe1) {
  debug_printf("vit_com1\n");

  if (signe1<0)
    {
      ecrire_registre(1,0x24,pos_fort);
      ecrire_registre(1,0x23,pos_faible);
    } else {
      ecrire_registre(1,0x24,~pos_fort);
      ecrire_registre(1,0x23,~pos_faible);
    }
  /*  debug_printf("v1 bit faible=%x, bit fort=%x\n",pos_faible,pos_fort);
      ecrire_registre(1,0x24,pos_fort);
      ecrire_registre(1,0x23,pos_faible);
  */
}

void vit_com2(byte pos_fort, byte pos_faible, int signe2) {
  debug_printf("vit_com2\n");

  if (signe2>0)
    {
      ecrire_registre(2,0x24,~pos_fort);
      ecrire_registre(2,0x23,~pos_faible);
    } else {
      ecrire_registre(2,0x24,pos_fort);
      ecrire_registre(2,0x23,pos_faible);
    }
 
  /*  debug_printf("v2 bit faible=%x, bit fort=%x\n",pos_faible,pos_fort);
      ecrire_registre(1,0x24,pos_fort);
      ecrire_registre(1,0x23,pos_faible);
  */
}


void vit_com_int(int hctl,byte vit)
{
  debug_printf("vit_com_int(hctl %d)\n",hctl);
  debug_printf ("vit = "); print_bin(vit); debug_printf("\n");
  if (hctl==2) ecrire_registre(hctl,0x3C,vit);
  else ecrire_registre(hctl,0x3C,-vit);
}

void acc_com_int(int hctl,byte acc_fort,byte acc_faible)
{
  debug_printf("acc_com_int(hctl %d)\n",hctl);

  ecrire_registre(hctl,0x27,acc_fort);
  ecrire_registre(hctl,0x26,acc_faible);
}

void pos_fin(int hctl,byte pos_fort,byte pos,byte pos_faible)
{
  debug_printf("pos_fin(hctl %d)\n",hctl);

  ecrire_registre(hctl,0x2B,pos_fort);
  ecrire_registre(hctl,0x2A,pos);
  ecrire_registre(hctl,0x29,pos_faible);
}

void acc_trap(int hctl,byte acc_fort,byte acc_faible)
{
  debug_printf("acc_trap(hctl %d)\n",hctl);

  ecrire_registre(hctl,0x27,acc_fort);
  ecrire_registre(hctl,0x26,acc_faible);
}

void vit_max_trap(int hctl,byte vit)
{
  debug_printf("vit_max_trap(hctl %d)\n",hctl);

  ecrire_registre(hctl,0x28,vit);
}

void raz_dist(int hctl)
{
  debug_printf("raz_dist(hctl %d)\n",hctl);

  //  ecrire_registre(hctl,0x13,00);
  //  ecrire_registre(hctl,0x14,00);
  ecrire_registre(hctl, 0xab, 0x15);
  ecrire_registre(hctl, 0xcd, 0x16);
  ecrire_registre(hctl, 0xef, 0x17);
  
  
  debug_printf("raz_dist hctl #%d:0x14:%x 0x13:%x 0x12:%x\n", 
	 hctl,
	 lire_registre(hctl, 0x14),
	 lire_registre(hctl, 0x13),
	 lire_registre(hctl, 0x12) );
}

void filter(int hctl,byte A,byte B,byte K)
{
  debug_printf("filter(hctl %d)\n",hctl);

  ecrire_registre(hctl,0x20,A);
  ecrire_registre(hctl,0x21,B);
  ecrire_registre(hctl,0x22,K);
}

void timer(int hctl,byte T)
{
  debug_printf("timer(hctl %d)\n",hctl);

  ecrire_registre(hctl,0x0f,T);
}

void pwm(int hctl,byte offset)
{
  debug_printf("pwm(hctl %d)\n",hctl);

  ecrire_registre(hctl,0x09,offset);
}

void config_out(int hctl,byte code)
{
    debug_printf("config_out(hctl %d)\n",hctl);

  ecrire_registre(hctl,0x07,code);
}

void read_pwm(int hctl, byte* pwm )
{
  debug_printf("read_pwm(hctl %d)\n",hctl);

  *pwm=lire_registre(hctl,0x09);
}

/* fin module hct.c */



#ifdef TEST_MAIN
int main ()
{
    int hctl=0;
    byte pwm=0;
    byte status=0;
    int choix=0;
    while(1) {
        printf("Menu: 0:setHctl1, 1:setHctl2, 2:getPos1, 3:getPos2, \n"
               "4:resetHard, 5:resetSoft, 6:init_repos, 7:alignement, 8=getPwm, 9=lireconfig\n");
        scanf("%d", &choix);
        switch(choix){
        case 0:
            hctl=1;
            break;
        case 1:
            hctl=2;
            break;
        case 2:
            printf("lecture_position_1=%d\n",
                   lecture_position_1());
            break;
        case 3:
            printf("lecture_position_2=%d\n",
                   lecture_position_2());
            break;
        case 4:
            reinitialisation_materielle();
            break;
        case 5:
            reinitialisation_logiciel(hctl);
            break;
        case 6:
            initialisation_repos(hctl);
            break;
        case 7:
            alignement(hctl);
            break;
        case 8:
            read_pwm(hctl, &pwm);
            printf("pwm d'hctl %d = %d\n", hctl, pwm);
            break;
        case 9:
            lire_config(hctl, &status);
            printf("status d'hctl %d = %d\n", hctl, status);
            break;
        default:
            break;
        }
    }

}
#endif





