/*
This code was automatically generated using the Riverside-Irvine State machine Builder tool
Version 2.8 --- 10/3/2017 19:26:27 PST
*/

#include "rims.h"

/*Define user variables and functions for this state machine here.*/
int i = 0;
int arr[10] = {0x31, 0xAB, 0x78, 0x11, 0x2F, 0xC3, 0xFF, 0xF5, 0x4F, 0x37};
unsigned char SM1_Clk;
void TimerISR() {
   SM1_Clk = 1;
}

enum SM1_States { SM1_s1, SM1_s2, SM1_s3, SM1_s4, SM1_s5, SM1_s6, SM1_s7, SM1_s8, SM1_s9, SM1_s10 } SM1_State;

TickFct_State_machine_1() {
   switch(SM1_State) { // Transitions
      case -1:
         SM1_State = SM1_s1;
         break;
         case SM1_s1: 
         if (A0 == 0) {
            SM1_State = SM1_s2;
            i++;
         }
         else if (A0 == 1) {
            SM1_State = SM1_s10;
         }
         break;
      case SM1_s2: 
         if (A0 == 1) {
            SM1_State = SM1_s1;
            i--;
         }
         else if (A0 == 0) {
            SM1_State = SM1_s3;
            i++;
         }
         break;
      case SM1_s3: 
         if (A0 == 0) {
            SM1_State = SM1_s4;
            i++;
         }
         else if (A0 == 1) {
            SM1_State = SM1_s2;
            i--;
         }
         break;
      case SM1_s4: 
         if (A0 == 1) {
            SM1_State = SM1_s3;
            i--;
         }
         else if (A0 == 0) {
            SM1_State = SM1_s5;
            i++;
         }
         break;
      case SM1_s5: 
         if (A0 == 1) {
            SM1_State = SM1_s4;
            i--;
         }
         else if (A0 == 0) {
            SM1_State = SM1_s6;
            i++;
         }
         break;
      case SM1_s6: 
         if (A0 == 1) {
            SM1_State = SM1_s5;
            i--;
         }
         else if (A0 == 0) {
            SM1_State = SM1_s7;
            i++;
         }
         break;
      case SM1_s7: 
         if (A0 == 1) {
            SM1_State = SM1_s6;
            i--;
         }
         else if (A0 == 0) {
            SM1_State = SM1_s8;
            i++;
         }
         break;
      case SM1_s8: 
         if (A0 == 1) {
            SM1_State = SM1_s7;
            i--;
         }
         else if (A0 == 0) {
            SM1_State = SM1_s9;
            i++;
         }
         break;
      case SM1_s9: 
         if (A0 == 1) {
            SM1_State = SM1_s8;
         }
         else if (A0 == 0) {
            SM1_State = SM1_s10;
            i++;
         }
         break;
      case SM1_s10: 
         if (A0 == 1) {
            SM1_State = SM1_s9;
         }
         else if (A0 == 0) {
            SM1_State = SM1_s1;
            i++;
         }
         break;
      default:
         SM1_State = SM1_s1;
   } // Transitions

   switch(SM1_State) { // State actions
      case SM1_s1:
         i = 0;
         B = arr[i];
         
         break;
      case SM1_s2:
         B = arr[i];
         break;
      case SM1_s3:
         B = arr[i];
         break;
      case SM1_s4:
         B = arr[i];
         break;
      case SM1_s5:
         B = arr[i];
         break;
      case SM1_s6:
         B = arr[i];
         break;
      case SM1_s7:
         B = arr[i];
         break;
      case SM1_s8:
         B = arr[i];
         break;
      case SM1_s9:
         B = arr[i];
         break;
      case SM1_s10:
         B = arr[i];
         break;
      default: // ADD default behaviour below
      break;
   } // State actions

}

int main() {

   const unsigned int periodState_machine_1 = 1000; // 1000 ms default
   TimerSet(periodState_machine_1);
   TimerOn();
   
   SM1_State = -1; // Initial state
   B = 0; // Init outputs

   while(1) {
      TickFct_State_machine_1();
      while(!SM1_Clk);
      SM1_Clk = 0;
   } // while (1)
} // Main