/*
This code was automatically generated using the Riverside-Irvine State machine Builder tool
Version 2.8 --- 10/5/2017 19:45:41 PST
*/

#include "rims.h"

/*This code will be shared between state machines.*/
unsigned char TimerFlag = 0;
void TimerISR() {
   TimerFlag = 1;
}


enum SM1_States { SM1_Wait, SM1_s2, SM1_s3, SM1_s4 } SM1_State;

TickFct_State_machine_1() {
   /*VARIABLES MUST BE DECLARED STATIC*/
/*e.g., static int x = 0;*/
/*Define user variables for this state machine here. No functions; make them global.*/

   switch(SM1_State) { // Transitions
      case -1:
         SM1_State = SM1_Wait;
         break;
      case SM1_Wait:
         if (A0 == 1) {
            SM1_State = SM1_s2;
         }
         break;
      case SM1_s2:
         if (A0 == 1 && A1 == 1) {
            SM1_State = SM1_s3;
         }
         else if (A0 == 0) {
            SM1_State = SM1_Wait;
         }
         break;
      case SM1_s3:
         if (A0 == 1 && A1 != 1) {
            SM1_State = SM1_s2;
         }
         else if (A0 == 1 && A1 == 1 && A2 == 1) {
            SM1_State = SM1_s4;
         }
         else if (A0 == 0) {
            SM1_State = SM1_Wait;
         }
         break;
      case SM1_s4:
         if (A0 == 1 && A1 == 1 && A2 != 1) {
            SM1_State = SM1_s3;
         }
         else if (A0 == 0) {
            SM1_State = SM1_Wait;
         }
         break;
      default:
         SM1_State = SM1_Wait;
      } // Transitions

   switch(SM1_State) { // State actions
      case SM1_Wait:
         B = 0x00;
         break;
      case SM1_s2:
         B = 0x01;
         break;
      case SM1_s3:
         B = 0x03;
         break;
      case SM1_s4:
         B = 0x07;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}

enum SM2_States { SM2_Wait, SM2_s2, SM2_s3, SM2_s4 } SM2_State;

TickFct_State_machine_2() {
   /*VARIABLES MUST BE DECLARED STATIC*/
/*e.g., static int x = 0;*/
/*Define user variables for this state machine here. No functions; make them global.*/

   switch(SM2_State) { // Transitions
      case -1:
         SM2_State = SM2_Wait;
         break;
      case SM2_Wait:
         if (A4 == 1) {
            SM2_State = SM2_s2;
         }
         break;
      case SM2_s2:
         if (A4 == 1 && A5 == 1) {
            SM2_State = SM2_s4;
         }
         else if (A4 == 0) {
            SM2_State = SM2_Wait;
         }
         break;
      case SM2_s3:
         if (A4 == 1 && A5 == 1 && A6 != 1 ) {
            SM2_State = SM2_s4;
         }
         else if (A4 == 0) {
            SM2_State = SM2_Wait;
         }
         break;
      case SM2_s4:
         if (A4 == 1 && A5 == 1 && A6 == 1 ) {
            SM2_State = SM2_s3;
         }
         else if (A4 == 1 && A5 != 1 ) {
            SM2_State = SM2_s2;
         }
         else if (A4 == 0) {
            SM2_State = SM2_Wait;
         }
         break;
      default:
         SM2_State = SM2_Wait;
      } // Transitions

   switch(SM2_State) { // State actions
      case SM2_Wait:
         break;
      case SM2_s2:
         B = 0x17;
         break;
      case SM2_s3:
         B = 0xFF;
         break;
      case SM2_s4:
         B = 0x77;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}
int main() {
   B = 0; //Init outputs
   TimerSet(1000);
   TimerOn();
   SM1_State = -1;
   SM2_State = -1;
   while(1) {
      TickFct_State_machine_1();
      TickFct_State_machine_2();
      while (!TimerFlag);
      TimerFlag = 0;
   }
}