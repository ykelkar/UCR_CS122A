/*
This code was automatically generated using the Riverside-Irvine State machine Builder tool
Version 2.8 --- 10/5/2017 19:43:34 PST
*/

#include "rims.h"

/*This code will be shared between state machines.*/

typedef struct task {
   int state;
   unsigned long period;
   unsigned long elapsedTime;
   int (*TickFct)(int);
} task;

task tasks[4];

const unsigned char tasksNum = 4;
const unsigned long periodPattern_Sequencer = 1000;
const unsigned long periodBouncing_LED = 1000;
const unsigned long periodMusic_Accompaniment = 500;
const unsigned long periodAll_LED = 1000;

const unsigned long tasksPeriodGCD = 500;

int TickFct_Pattern_Sequencer(int state);
int TickFct_Bouncing_LED(int state);
int TickFct_Music_Accompaniment(int state);
int TickFct_All_LED(int state);

unsigned char processingRdyTasks = 0;
void TimerISR() {
   unsigned char i;
   if (processingRdyTasks) {
      printf("Period too short to complete tasks\n");
   }
   processingRdyTasks = 1;
   for (i = 0; i < tasksNum; ++i) { // Heart of scheduler code
      if ( tasks[i].elapsedTime >= tasks[i].period ) { // Ready
         tasks[i].state = tasks[i].TickFct(tasks[i].state);
         tasks[i].elapsedTime = 0;
      }
      tasks[i].elapsedTime += tasksPeriodGCD;
   }
   processingRdyTasks = 0;
}
int main() {
   // Priority assigned to lower position tasks in array
   unsigned char i=0;
   tasks[i].state = -1;
   tasks[i].period = periodPattern_Sequencer;
   tasks[i].elapsedTime = tasks[i].period;
   tasks[i].TickFct = &TickFct_Pattern_Sequencer;

   ++i;
   tasks[i].state = -1;
   tasks[i].period = periodBouncing_LED;
   tasks[i].elapsedTime = tasks[i].period;
   tasks[i].TickFct = &TickFct_Bouncing_LED;

   ++i;
   tasks[i].state = -1;
   tasks[i].period = periodMusic_Accompaniment;
   tasks[i].elapsedTime = tasks[i].period;
   tasks[i].TickFct = &TickFct_Music_Accompaniment;

   ++i;
   tasks[i].state = -1;
   tasks[i].period = periodAll_LED;
   tasks[i].elapsedTime = tasks[i].period;
   tasks[i].TickFct = &TickFct_All_LED;

   ++i;
   TimerSet(tasksPeriodGCD);
   TimerOn();
   
   while(1) { Sleep(); }

   return 0;
}

enum SM1_States { SM1_s1, SM1_s2, SM1_s3, SM1_s4, SM1_s5, SM1_s6, SM1_s7, SM1_s8, SM1_s9, SM1_s10, SM1_Wait } SM1_State;
int TickFct_Pattern_Sequencer(int state) {
   /*VARIABLES MUST BE DECLARED STATIC*/
/*e.g., static int x = 0;*/
/*Define user variables for this state machine here. No functions; make them global.*/
static int i = 0;
static int arr[10] = {0x31, 0xAB, 0x78, 0x11, 0x2F, 0xC3, 0xFF, 0xF5, 0x4F, 0x37};
   switch(state) { // Transitions
      case -1:
         state = SM1_Wait;
         break;
      case SM1_s1:
         if (A6 == 1 && A7 == 0 && A0 == 0) {
            state = SM1_s2;
            i++;
         }
         else if (A6 == 1 && A7 == 0 && A0 == 1) {
            state = SM1_s10;
         }
         else if (A6 != 1 || A7 != 0) {
            state = SM1_Wait;
         }
         break;
      case SM1_s2:
         if (A6 == 1 && A7 == 0 && A0 == 1) {
            state = SM1_s1;
            i--;
         }
         else if (A6 == 1 && A7 == 0 && A0 == 0) {
            state = SM1_s3;
            i++;
         }
         else if (A6 != 1 || A7 != 0) {
            state = SM1_Wait;
         }
         break;
      case SM1_s3:
         if (A6 == 1 && A7 == 0 && A0 == 0) {
            state = SM1_s4;
            i++;
         }
         else if (A6 == 1 && A7 == 0 && A0 == 1) {
            state = SM1_s2;
            i--;
         }
         else if (A6 != 1 || A7 != 0) {
            state = SM1_Wait;
         }
         break;
      case SM1_s4:
         if (A6 == 1 && A7 == 0 && A0 == 1) {
            state = SM1_s3;
            i--;
         }
         else if (A6 == 1 && A7 == 0 && A0 == 0) {
            state = SM1_s5;
            i++;
         }
         else if (A6 != 1 || A7 != 0) {
            state = SM1_Wait;
         }
         break;
      case SM1_s5:
         if (A6 == 1 && A7 == 0 && A0 == 1) {
            state = SM1_s4;
            i--;
         }
         else if (A6 == 1 && A7 == 0 && A0 == 0) {
            state = SM1_s6;
            i++;
         }
         else if (A6 != 1 || A7 != 0) {
            state = SM1_Wait;
         }
         break;
      case SM1_s6:
         if (A6 == 1 && A7 == 0 && A0 == 1) {
            state = SM1_s5;
            i--;
         }
         else if (A6 == 1 && A7 == 0 && A0 == 0) {
            state = SM1_s7;
            i++;
         }
         else if (A6 != 1 || A7 != 0) {
            state = SM1_Wait;
         }
         break;
      case SM1_s7:
         if (A6 == 1 && A7 == 0 && A0 == 1) {
            state = SM1_s6;
            i--;
         }
         else if (A6 == 1 && A7 == 0 && A0 == 0) {
            state = SM1_s8;
            i++;
         }
         else if (A6 != 1 || A7 != 0) {
            state = SM1_Wait;
         }
         break;
      case SM1_s8:
         if (A6 == 1 && A7 == 0 && A0 == 1) {
            state = SM1_s7;
            i--;
         }
         else if (A6 == 1 && A7 == 0 && A0 == 0) {
            state = SM1_s9;
            i++;
         }
         else if (A6 != 1 || A7 != 0) {
            state = SM1_Wait;
         }
         break;
      case SM1_s9:
         if (A6 == 1 && A7 == 0 && A0 == 1) {
            state = SM1_s8;
         }
         else if (A6 == 1 && A7 == 0 && A0 == 0) {
            state = SM1_s10;
            i++;
         }
         else if (A6 != 1 || A7 != 0) {
            state = SM1_Wait;
         }
         break;
      case SM1_s10:
         if (A6 == 1 && A7 == 0 && A0 == 1) {
            state = SM1_s9;
         }
         else if (A6 == 1 && A7 == 0 && A0 == 0) {
            state = SM1_s1;
            i++;
         }
         else if (A6 != 1 || A7 != 0) {
            state = SM1_Wait;
         }
         break;
      case SM1_Wait:
         if (A6 == 1 && A7 == 0 && A0 == 0) {
            state = SM1_s1;
         }
         else if (A6 != 1 || A7 != 0) {
            state = SM1_Wait;
         }
         break;
      default:
         state = -1;
      } // Transitions

   switch(state) { // State actions
      case SM1_s1:
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
      case SM1_Wait:
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
   SM1_State = state;
   return state;
}


enum SM2_States { SM2_s1, SM2_s2, SM2_s3, SM2_s4, SM2_s5, SM2_s6, SM2_s7, SM2_s8, SM2_Wait } SM2_State;
int TickFct_Bouncing_LED(int state) {
   /*VARIABLES MUST BE DECLARED STATIC*/
/*e.g., static int x = 0;*/
/*Define user variables for this state machine here. No functions; make them global.*/
   switch(state) { // Transitions
      case -1:
         state = SM2_Wait;
         break;
      case SM2_s1:
         if (A6 == 0 && A7 == 0 && A0 == 0) {
            state = SM2_s2;
         }
         else if (A6 == 0 && A7 == 0 && A0 == 1) {
            state = SM2_s8;
         }
         else if (A6 != 0 || A7 != 0) {
            state = SM2_Wait;
         }
         break;
      case SM2_s2:
         if (A6 == 0 && A7 == 0 && A0 == 1) {
            state = SM2_s1;
         }
         else if (A6 == 0 && A7 == 0 && A0 == 0) {
            state = SM2_s3;
         }
         else if (A6 != 0 || A7 != 0) {
            state = SM2_Wait;
         }
         break;
      case SM2_s3:
         if (A6 == 0 && A7 == 0 && A0 == 1) {
            state = SM2_s2;
         }
         else if (A6 == 0 && A7 == 0 && A0 == 0) {
            state = SM2_s4;
         }
         else if (A6 != 0 || A7 != 0) {
            state = SM2_Wait;
         }
         break;
      case SM2_s4:
         if (A6 == 0 && A7 == 0 && A0 == 1) {
            state = SM2_s3;
         }
         else if (A6 == 0 && A7 == 0 && A0 == 0) {
            state = SM2_s5;
         }
         else if (A6 != 0 || A7 != 0) {
            state = SM2_Wait;
         }
         break;
      case SM2_s5:
         if (A6 == 0 && A7 == 0 && A0 == 1) {
            state = SM2_s4;
         }
         else if (A6 == 0 && A7 == 0 && A0 == 0) {
            state = SM2_s6;
         }
         else if (A6 != 0 || A7 != 0) {
            state = SM2_Wait;
         }
         break;
      case SM2_s6:
         if (A6 == 0 && A7 == 0 && A0 == 1) {
            state = SM2_s5;
         }
         else if (A6 == 0 && A7 == 0 && A0 == 0) {
            state = SM2_s7;
         }
         else if (A6 != 0 || A7 != 0) {
            state = SM2_Wait;
         }
         break;
      case SM2_s7:
         if (A6 == 0 && A7 == 0 && A0 == 1) {
            state = SM2_s6;
         }
         else if (A6 == 0 && A7 == 0 && A0 == 0) {
            state = SM2_s8;
         }
         else if (A6 != 0 || A7 != 0) {
            state = SM2_Wait;
         }
         break;
      case SM2_s8:
         if (A6 == 0 && A7 == 0 && A0 == 1) {
            state = SM2_s7;
         }
         else if (A6 == 0 && A7 == 0 && A0 == 0) {
            state = SM2_s1;
         }
         else if (A6 != 0 || A7 != 0) {
            state = SM2_Wait;
         }
         break;
      case SM2_Wait:
         if (A6 != 0 || A7 != 0) {
            state = SM2_Wait;
         }
         else if (A6 == 0 && A7 == 0) {
            state = SM2_s1;
         }
         break;
      default:
         state = -1;
      } // Transitions

   switch(state) { // State actions
      case SM2_s1:
         B = 0x01;
         break;
      case SM2_s2:
         B = 0x02;
         break;
      case SM2_s3:
         B = 0x04;
         break;
      case SM2_s4:
         B = 0x08;
         break;
      case SM2_s5:
         B = 0x10;
         break;
      case SM2_s6:
         B = 0x20;
         break;
      case SM2_s7:
         B = 0x40;
         break;
      case SM2_s8:
         B = 0x80;
         break;
      case SM2_Wait:
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
   SM2_State = state;
   return state;
}


enum SM3_States { SM3_Wait, SM3_s1, SM3_s2, SM3_s3 } SM3_State;
int TickFct_Music_Accompaniment(int state) {
   /*VARIABLES MUST BE DECLARED STATIC*/
/*e.g., static int x = 0;*/
/*Define user variables for this state machine here. No functions; make them global.*/
static int i = 0;
static int count = 0;
static arr[] = {0x5A, 0x37, 0xCC, 0xFF, 0x00};
   switch(state) { // Transitions
      case -1:
         state = SM3_Wait;
         break;
      case SM3_Wait:
         if (A6 != 0 || A7 != 1) {
            state = SM3_Wait;
         }
         else if (A6 == 0 && A7 == 1 ) {
            state = SM3_s1;
            i = 0;
         }
         break;
      case SM3_s1:
         if (A6 != 0 || A7 != 1) {
            state = SM3_Wait;
         }
         else if (A6 == 0 && A7 == 1 && count == 2) {
            state = SM3_s2;
         }
         else if (A6 == 0 && A7 == 1 && count == 9) {
            state = SM3_s3;
         }
         break;
      case SM3_s2:
         if (A6 == 0 && A7 == 1 && count == 8) {
            state = SM3_s1;
         }
         else if (A6 != 0 || A7 != 1) {
            state = SM3_Wait;
         }
         else if (A6 == 0 && A7 == 1 && count == 4) {
            state = SM3_s3;
            count++;
B = arr[2];
         }
         break;
      case SM3_s3:
         if (A6 == 0 && A7 == 1 && count == 6) {
            state = SM3_s2;
         }
         else if (A6 != 0 || A7 != 1) {
            state = SM3_Wait;
         }
         else if (A6 == 0 && A7 == 1 && count == 10) {
            state = SM3_s1;
         }
         break;
      default:
         state = -1;
      } // Transitions

   switch(state) { // State actions
      case SM3_Wait:
         break;
      case SM3_s1:
         if (count == 0)
         {
         B = arr[0];
         }
         else if (count == 8)
         {
         B = arr[3];
         }
         else if (count == 10)
         {
         B = arr[4];
         }
         else if (count == 11)
         {
         B = arr[3];
         }
         else if (count == 12)
         {
         B = arr[0];
         }
         else if (count == 13)
         {
         count = 0;
         }
         count++;
         
         break;
      case SM3_s2:
         if (count == 2)
         {
         B = arr[1];
         }
         else if (count == 7)
         {
         B = arr[3];
         }
         count++;
         break;
      case SM3_s3:
         if (count == 4)
         {
         B = arr[2];
         }
         count++;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
   SM3_State = state;
   return state;
}


enum SM4_States { SM4_Wait, SM4_s2 } SM4_State;
int TickFct_All_LED(int state) {
   /*VARIABLES MUST BE DECLARED STATIC*/
/*e.g., static int x = 0;*/
/*Define user variables for this state machine here. No functions; make them global.*/
   switch(state) { // Transitions
      case -1:
         state = SM4_Wait;
         break;
      case SM4_Wait:
         if (A6 != 1 && A7 != 1) {
            state = SM4_Wait;
         }
         else if (A6 == 1 && A7 == 1) {
            state = SM4_s2;
         }
         break;
      case SM4_s2:
         if (A6 != 1 && A7 != 1) {
            state = SM4_Wait;
         }
         break;
      default:
         state = -1;
      } // Transitions

   switch(state) { // State actions
      case SM4_Wait:
         break;
      case SM4_s2:
         B = 0xFF;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
   SM4_State = state;
   return state;
}

