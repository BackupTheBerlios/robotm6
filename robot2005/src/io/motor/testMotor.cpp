#include <unistd.h>
#include <stdio.h>
#include "HCTLControl.h"


int main(int argc, char* argv[]) 
{
    hctlInit();
    printf("Init done\n");
    printf("%d\n",hctlGetRightPos());
    sleep(2);
    printf("hctlSetMotorSpeed(10,10)\n");
    hctlSetMotorSpeed(10, 10);
    printf("%d\n",hctlGetRightPos());
    sleep(4);
    printf("hctlSetMotorSpeed(-10,-10)\n");
    hctlSetMotorSpeed(-10, -10);
    sleep(4);
    printf("hctlSetMotorSpeed(0,0)\n");
    hctlSetMotorSpeed(0, 0);
    sleep(2);
    printf("hctlIdle\n");
    printf("%d\n",hctlGetRightPos());
    hctlIdle();
}
