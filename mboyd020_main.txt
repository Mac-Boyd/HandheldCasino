#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "timer.h"
#include "cards.h"

unsigned short Period = 25;

unsigned short timeRand = 0;

void initLCD();
void LCDcommand(unsigned char comD, unsigned char comC);
bool busyFlag = true;
char dispL1[] = "               ";
unsigned char dispSize1 = 8;
char dispL2[] = "               ";
unsigned char dispSize2 = 4;  

bool enable = 0;

unsigned char buzzCount = 0;

unsigned short chips = 1000;
short bet = 0;
short prevBet = 0;
unsigned char* hands[4] = {malloc(11 * sizeof(unsigned char)), malloc(11 * sizeof(unsigned char)), malloc(11 * sizeof(unsigned char)), malloc(11 * sizeof(unsigned char))};
unsigned char handRet[4] = {0, 0, 0, 0};
unsigned char* playerHand = hands[0]; //maximum number of cards in a blackjack hand is 4 aces + 4 2s + 3 3s = 11
unsigned char* dealerHand = malloc(11 * sizeof(unsigned char));
unsigned char phSize = 0; //size of current hand
unsigned char dhSize = 0;
unsigned char phTot = 0;
unsigned char dhTot = 0;
char playerWin = 2;
unsigned short winTimer = 0;
char target = 0;
char numHands = 1;

struct task{
   unsigned char state;
   unsigned short period;
   unsigned short timeElapsed;
   unsigned char (*tickFnc)(unsigned char);
   task(unsigned char state, unsigned short period, unsigned short timeElapsed, unsigned char (*tickFnc)(unsigned char)) : state(state), period(period), timeElapsed(timeElapsed), tickFnc(tickFnc) {};
};
unsigned char dispTick(unsigned char state); unsigned char casinoTick(unsigned char state); unsigned char blackjackTick(unsigned char state); unsigned char baccaratTick(unsigned char state); unsigned char buzzTick(unsigned char state);
task disp(0, 500, 500, *dispTick);
task casino(0, 50, 50, *casinoTick);
task blackjack(0, 200, 200, *blackjackTick);
task baccarat(0, 50, 50, *baccaratTick);
task buzz(3, 50, 50, *buzzTick);
unsigned char taskSize = 3;
task tasks[3] = {casino, disp, buzz};

int main(){
   DDRC = 0xFF;
   DDRD = 0xFF;
   DDRB = 0x00;
   initLCD();

   TimerSet(Period);
   TimerOn();
   while(1){    
      while(!TimerFlag){timeRand ++;};
      for(int i = 0; i < taskSize; i++){
         if(tasks[i].timeElapsed >= tasks[i].period){
            tasks[i].state = tasks[i].tickFnc(tasks[i].state);
            tasks[i].timeElapsed = 0;
         }
         tasks[i].timeElapsed += Period;
      }
      TimerFlag = 0;
   };
   return 0;
}

void initLCD(){
   _delay_ms(50);
   LCDcommand(0x38, 0x00);
   //display off
   LCDcommand(0x0F, 0x00);
   //display clear
   LCDcommand(0x01, 0x00);
   //entry mode set
   LCDcommand(0x06, 0x00);
   
}
void LCDcommand(unsigned char comD, unsigned char comC){
   while(busyFlag){PORTC = 0x10; busyFlag = PORTD & 0x80;}
   PORTC = 0x00;
   PORTC += comC << 4;
   PORTD = comD;
   PORTC |= 0x08;
   busyFlag = 1;
   _delay_ms(1);
   PORTC &= 0xF7;
}

unsigned char dispTick(unsigned char state)
{
   switch(state){
      case 0:
         LCDcommand(0x01, 0x00);
         LCDcommand(0x0C, 0x00);
         for(int i = 0; i < dispSize1; i++){
            LCDcommand(dispL1[i], 0x02);
         }
         for(int i = dispSize1; i < 40; i++){
            LCDcommand(32, 0x02);
         }
         for(int i = 0; i < dispSize2; i++){
            LCDcommand(dispL2[i], 0x02);
         }
         for(int i = dispSize2; i < 16; i++){
            LCDcommand(32, 0x02);
         }
         //_delay_ms(25); //stalls for a second to ensure what's on the screen is able to be read
         break;
      default:
         break;
   }
   switch(state){
      case 0:
         return 1;
      case 1:
         return 1;
      default:
         return 0;
   }
   return 0;
}

unsigned char casinoTick(unsigned char state)
{
   if((PINB & 0x01) == 0){
      if((PINB & 0x02) == 0){
         if((PINB & 0x04) == 0){
            if((PINB & 0x08) == 0){
               enable = true;
            }
         }
      }
   }
   strcpy(dispL1, "Pick a game:");
   dispSize1 = 12;
   switch(state){
      case 0:
         strcpy(dispL2, "Blackjack");
         dispSize2 = 9;
         break;
      case 1:
         strcpy(dispL2, "Baccarat");
         dispSize2 = 8;
         break;
   }
   switch(state){
      case 0:
         if((((PINB & 0x01) + (PINB & 0x02)) > 0) && enable){
            enable = false;
            tasks[1].state = 0;
            return 1;
         }
         if((((PINB & 0x04) + (PINB & 0x08)) > 0) && enable){
            enable = false;
            srand(timeRand);
            tasks[0] = blackjack;
            return 0;
         }
         return 0;
         break;
      case 1:
         if((((PINB & 0x01) + (PINB & 0x02)) > 0) && enable){
            enable = false;
            tasks[1].state = 0;
            return 0;
         }
         if((((PINB & 0x04) + (PINB & 0x08)) > 0) && enable){
            enable = false;
            srand(timeRand);
            tasks[0] = baccarat;
            return 0;
         }
         return 1;
         break;
   }
   return 0;
}

unsigned char blackjackTick(unsigned char state)
{
   if((PINB & 0x01) == 0){
      if((PINB & 0x02) == 0){
         if((PINB & 0x04) == 0){
            if((PINB & 0x08) == 0){
               enable = true;
            }
         }
      }
   }
   
   switch(state){
      case 0:
         if(enable && ((PINB & 0x01) > 0)){
            enable = false;
            prevBet = bet;
            bet += 25;
            if(bet > chips){bet = 0;}
         }
         if(enable && ((PINB & 0x02) > 0)){
            enable = false;
            prevBet = bet;
            bet -= 25;
            if(bet < 0){bet = chips;}
         }
         if(bet != prevBet)
         {
            tasks[1].state = 0;
         }
         strcpy(dispL1, "Make your bet:");
         dispSize1 = 14;
         itoa(bet, dispL2, 10);
         dispSize2 = log10(bet) + 1;
         break;
      case 1:
         strcpy(dispL1, "               ");
         dispSize1 = 2;
         strcpy(dispL2, "               ");
         dispSize2 = 2;
         dispL1[0] = getFace(dealerHand[0]);
         dispL1[1] = dealerHand[1];
         dispL2[0] = getFace(playerHand[0]);
         dispL2[1] = getFace(playerHand[1]);
         tasks[1].state = 0;
         break;
      case 2:
         strcpy(dispL1, "               ");
         dispSize1 = 2;
         strcpy(dispL2, "               ");
         dispSize2 = phSize;
         dispL1[0] = getFace(dealerHand[0]);
         dispL1[1] = dealerHand[1];
         for(int i = 0; i < phSize; i++){
            dispL2[i] = getFace(playerHand[i]);
         }
         tasks[1].state = 0;
         break;
      case 3:
         strcpy(dispL1, "               ");
         dispSize1 = dhSize;
         for(int i = 0; i < dhSize; i++){
            dispL1[i] = getFace(dealerHand[i]);
         }
         for(int i = 0; i < phSize; i++){
            dispL2[i] = getFace(playerHand[i]);
         }
         dispSize2 = phSize;
         tasks[1].state = 0;
         _delay_ms(15);
         break;
      case 4:
         if(winTimer == 0 && (playerWin == 2))
         {
            for(int i = 0; i < phSize; i++){
               if((getVal(playerHand[i]) == 1) && ((phTot + 10) < 22)){
                  phTot += 10;
                  break;
               }
            }
            for(int i = 0; i < dhSize; i++){
               if((getVal(dealerHand[i]) == 1) && ((dhTot + 10) < 22)){
                  dhTot += 10;
                  break;
               }
            }
            playerWin = phTot > dhTot;
         }
         if(winTimer == 0){
            chips += (2 * bet * (playerWin));
            tasks[2].state = playerWin?1 : 2;
         }
         if(playerWin){
            strcpy(dispL1, "YOU WIN :)");
            dispSize1 = 10;
         }
         else{
            strcpy(dispL1, "YOU LOSE :(");
            dispSize1 = 11;
         }
         winTimer += blackjack.period;
         dispSize2 = 0;
         tasks[1].state = 0;
         break;
   }
   switch(state){
      case 0:
         if(enable && ((PINB & 0x04) > 0) && (bet > 0)){
            enable = false;
            playerHand[0] = draw();
            phTot += getVal(playerHand[0]);
            playerHand[1] = draw();
            phTot += getVal(playerHand[1]);
            phSize = 2;
            dealerHand[0] = draw();
            dhTot += getVal(dealerHand[1]);
            dealerHand[1] = 0x2A;
            dhSize = 2;
            chips = chips - bet;
            return 1;
         }
         return 0;
         break;
      case 1:
         if(enable && ((PINB & 0x01) > 0)){
            playerHand[2] = draw();
            phTot += getVal(playerHand[2]);
            phSize = 3;
            enable = 0;
            if(phTot > 21){
               strcpy(dispL1, "BUST!");
               dispSize1 = 5;
               for(int i = 0; i < phSize; i++){
                  dispL2[i] = getFace(playerHand[i]);
               }
               dispSize2 ++;
               tasks[1].state = 0;
               playerWin = 0;
               winTimer = 0;
               return 4;
            }
            return 2;
         }
         if(enable && ((PINB & 0x02) > 0)){
            enable = 0;
            dealerHand[1] = draw();
            if(numHands > 1){
               handRet[numHands - 1] = 3;
               for(int i = numHands - 2; i >= 0; i--){
                  if(handRet[i] != 3){
                     playerHand = hands[i];
                     return 2;
                  }
               }
            }
            return 3;
         }
         if(enable && ((PINB & 0x04) > 0)){
            //double
            enable = 0;
            chips = chips - bet;
            bet += bet;
            playerHand[2] = draw();
            phTot += getVal(playerHand[2]);
            phSize = 3;
            if(phTot > 21){
               strcpy(dispL1, "BUST!");
               dispSize1 = 5;
               for(int i = 0; i < phSize; i++){
                  dispL2[i] = getFace(playerHand[i]);
               }
               dispSize2 ++;
               tasks[1].state = 0;
               playerWin = 0;
               winTimer = 0;
               return 4;
            }
            if(numHands > 1){
               handRet[numHands - 1] = 3;
               for(int i = numHands - 2; i >= 0; i--){
                  if(handRet[i] != 3){
                     playerHand = hands[i];
                     return 2;
                  }
               }
            }
            dealerHand[1] = draw();
            return 3;
         }
         if(enable && ((PINB & 0x08) > 0) && (chips >= bet) && (getVal(playerHand[0]) == getVal(playerHand[1]))){
            //split
            numHands ++;
            char temp = playerHand[1];
            playerHand[1] = draw();
            playerHand = hands[numHands - 1];
            playerHand[0] = temp;
            playerHand[1] = draw();
            chips -= bet;
            return 1;
         } 
         return 1;
      case 2:
         if(enable && ((PINB & 0x01) > 0)){
            enable = 0;
            playerHand[phSize] = draw();
            phTot += getVal(playerHand[phSize]);
            phSize ++;
            if(phTot > 21){
               strcpy(dispL1, "BUST!");
               dispSize1 = 5;
               tasks[1].state = 0;
               for(int i = 0; i < phSize; i++){
                  dispL2[i] = getFace(playerHand[i]);
               }
               dispSize2 ++;
               playerWin = 0;
               winTimer = 0;
               return 4;
            }
            return 2;
         }
         if(enable && ((PINB & 0x02) > 0)){
            enable = 0;
            if(numHands > 1){
               handRet[numHands - 1] = 3;
               for(int i = numHands - 2; i >= 0; i--){
                  if(handRet[i] != 3){
                     playerHand = hands[i];
                     return 2;
                  }
               }
            }
            return 3;
         }
         return 2;
      case 3:
         if(dhTot < 17){
            //dealer hits
            dealerHand[dhSize] = draw();
            dhTot += getVal(dealerHand[dhSize]);
            dhSize ++;
            if(dhTot > 21){
               strcpy(dispL2, "DEALER BUST!");
               dispSize2 = 12;
               tasks[1].state = 0;
               playerWin = 1;
               winTimer = 0;
               return 4;
            }
            return 3;
         }
         //dealer stays
         winTimer = 0;
         return 4;
      case 4:
         if(winTimer >= 1000){
            if(numHands > 1){
               winTimer = 0;
               numHands --;
               playerHand = hands[numHands];
               return 4;
            }
            phSize = 0;
            dhSize = 0;
            phTot = 0;
            dhTot = 0;
            bet = 0;
            prevBet = 0;
            tasks[0] = casino;
            playerWin = 2;
            return 0;
         }
         return 4;
   }
   return 0;
}

unsigned char baccaratTick(unsigned char state)
{
   if((PINB & 0x01) == 0){
      if((PINB & 0x02) == 0){
         if((PINB & 0x04) == 0){
            if((PINB & 0x08) == 0){
               enable = true;
            }
         }
      }
   }
   strcpy(dispL1, "Make your bet:");
   dispSize1 = 14;
   tasks[1].state = 0;
   switch(state){
      case 0:
         if(enable && ((PINB & 0x01) > 0)){
            enable = false;
            prevBet = bet;
            bet += 25;
            if(bet > chips){bet = 0;}
         }
         if(enable && ((PINB & 0x02) > 0)){
            enable = false;
            prevBet = bet;
            bet -= 25;
            if(bet < 0){bet = chips;}
         }
         if(bet != prevBet)
         {
            tasks[1].state = 0;
         }
         strcpy(dispL1, "Make your bet:");
         dispSize1 = 14;
         itoa(bet, dispL2, 10);
         dispSize2 = log10(bet) + 1;
         break;
      case 1:
         strcpy(dispL1, "Pick a target:");
         dispSize1 = 14;
         switch(target){
            case 0:
               strcpy(dispL2, "Punto");
               dispSize2 = 5;
               break;
            case 1:
               strcpy(dispL2, "Banco");
               dispSize2 = 5;
               break;
            default:
               strcpy(dispL2, "Egalite");
               dispSize2 = 7;
               break;
         }
         tasks[1].state = 0;
         break;
      case 2:
         playerHand[0] = draw();
         phTot += getVal(playerHand[0]);
         playerHand[1] = draw();
         phTot += getVal(playerHand[1]);
         phSize = 2;
         for(int i = 0; i < phSize; i++){
            dispL2[i] = getFace(playerHand[i]);
         }
         dispSize2 = phSize;
         tasks[1].state = 0;
         break;
      case 3:
         for(int i = 0; i < phSize; i++){
            dispL2[i] = getFace(playerHand[i]);
         }
         dispSize2 = phSize;
         tasks[1].state = 0;
         break;
      case 4:
         if(phSize == 3){
            switch(dhTot % 10){
               case 1:
                  dealerHand[dhSize] = draw();
                  dhTot += dealerHand[dhSize];
                  dhSize ++;
                  break;
               case 2:
                  dealerHand[dhSize] = draw();
                  dhTot += dealerHand[dhSize];
                  dhSize ++;
                  break;
               case 3:
                  if(getVal(playerHand[2]) != 8){
                     dealerHand[dhSize] = draw();
                     dhTot += dealerHand[dhSize];
                     dhSize ++;
                  }
                  break;
               case 4:
                  switch(getVal(playerHand[2])){
                     case 1:
                        break;
                     case 8:
                        break;
                     case 9:
                        break;
                     case 10:
                        break;
                     default:
                        dealerHand[dhSize] = draw();
                        dhTot += dealerHand[dhSize];
                        dhSize ++;
                        break;
                  }
                  break;
               case 5:
                  switch(getVal(playerHand[2]))
                  {
                     case 4:
                        dealerHand[dhSize] = draw();
                        dhTot += dealerHand[dhSize];
                        dhSize ++;
                        break;
                     case 5:
                        dealerHand[dhSize] = draw();
                        dhTot += dealerHand[dhSize];
                        dhSize ++;
                        break;
                     case 6:
                        dealerHand[dhSize] = draw();
                        dhTot += dealerHand[dhSize];
                        dhSize ++;
                        break;
                     case 7:
                        dealerHand[dhSize] = draw();
                        dhTot += dealerHand[dhSize];
                        dhSize ++;
                        break;
                     default:
                        break;
                  }
                  break;
               case 6:
                  switch(playerHand[2]){
                     case 6:
                        dealerHand[dhSize] = draw();
                        dhTot += dealerHand[dhSize];
                        dhSize ++;
                        break;
                     case 7:
                        dealerHand[dhSize] = draw();
                        dhTot += dealerHand[dhSize];
                        dhSize ++;
                        break;
                     default:
                        break;
                  }
               default:
                  break;
            }
         }
         else{
            if((dhTot % 10) <= 5){
               dealerHand[dhSize] = draw();
               dhTot += dealerHand[dhSize];
               dhSize ++;
            }
         }
         for(int i = 0; i < dhSize; i++){
            dispL1[i] = getFace(dealerHand[i]);
         }
         dispSize1 = dhSize;
         for(int i = 0; i < phSize; i++){
            dispL2[i] = getFace(playerHand[i]);
         }
         dispSize2 = phSize;
         tasks[1].state = 0;
         break;
      case 5:
         strcpy(dispL1, "PUNTO!");
         dispSize1 = 6;
         dispSize2 = 0;
         tasks[1].state = 0;
         tasks[2].state = 0;
         chips += 2 * (bet * (target == 0));
         break;
      case 6:
         strcpy(dispL1, "BANCO!");
         dispSize1 = 6;
         dispSize2 = 0;
         tasks[2].state = 1;
         tasks[1].state = 0;
         chips += 1.95 * (bet * (target == 1));
         break;
      case 7:
         strcpy(dispL1, "EGALITE!");
         dispSize1 = 8;
         dispSize2 = 0;
         tasks[1].state = 0;
         chips += 8 * (bet * (target == 2));
         break;
   }
   switch(state){
      case 0:
         if(enable && ((PINB & 0x04) > 0) && (bet > 0)){
            enable = false;
            chips = chips - bet;
            strcpy(dispL1, "Pick a target:");
            dispSize1 = 14;
            target = 0;
            return 1;
         }
         return 0;
      case 1:
         if(enable && ((PINB & 0x02) > 0)){
            enable = 0;
            target = (target <= 0)?2 : target - 1;
            return 1;
         }
         if(enable && ((PINB & 0x01) > 0)){
            enable = 0;
            target = (target >= 2)?0 : target + 1;
            return 1;
         }
         if(enable && ((PINB & 0x04) > 0)){
            enable = 0;
            //we stop taking input here so slowing it down allows for some resting time.
            tasks[0].period = 1000;
            strcpy(dispL1, "               ");
            dispSize1 = 0;
            tasks[1].state = 0;
            return 2;
         }
         return 1;
      case 2:
         if((phTot % 10) < 6){
            playerHand[2] = draw();
            phTot += playerHand[2];
            return 3;
         }
         if((phTot % 10) < 8){
            dealerHand[0] = draw();
            dhTot += getVal(dealerHand[0]);
            dealerHand[1] = draw();
            dhTot += getVal(dealerHand[1]);
            dhSize = 2;
            for(int i = 0; i < dhSize; i++){
               dispL1[i] = getFace(dealerHand[i]);
            }
            dispSize1 = dhSize;
            return 4;
         }
         strcpy(dispL1, "PUNTO!");
         dispSize1 = 6;
         dispSize2 = 0;
         tasks[1].state = 0;
         tasks[2].state = 1;
         tasks[1].state = 0;
         return 5;
      case 3:
         dealerHand[0] = draw();
         dhTot += getVal(dealerHand[0]);
         dealerHand[1] = draw();
         dhTot += getVal(dealerHand[1]);
         dhSize = 2;
         for(int i = 0; i < dhSize; i++){
            dispL1[i] = getFace(dealerHand[i]);
         }
         dispSize1 = dhSize;
         return 4;
      case 4:
         if(dhTot < phTot){
            strcpy(dispL1, "PUNTO!");
            dispSize1 = 6;
            dispSize2 = 0;
            tasks[1].state = 0;
            tasks[2].state = 1;
            return 5;
         }
         if(dhTot > phTot){
            strcpy(dispL1, "BANCO!");
            dispSize1 = 6;
            dispSize2 = 0;
            tasks[1].state = 0;
            tasks[2].state = 2;
            return 6;
         }
         strcpy(dispL1, "EGALITE!");
         dispSize1 = 8;
         dispSize2 = 0;
         tasks[1].state = 0;
         tasks[2].state = (target == 2)?1 : 2;
         return 7;
      default:
         tasks[0] = casino;
         bet = 0;
         return 0;
   }
   return 0;
}

unsigned char buzzTick(unsigned char state)
{
   switch(state){
      //zero = no buzz, one = "happy buzz" (three fast), two = "sad buzz" (two slow), three = "ready buzz" (one fast)
      case 1:
         if((buzzCount % 2) == 0){
            PORTC |= 0x04;
         }
         else{
            PORTC &= 0xFB;
         }
         buzzCount ++;
         break;
      case 2:
         if((buzzCount % 8) == 0){
            PORTC ^= 0x04;
         }
         buzzCount ++;
         break;
      case 3:
         if((buzzCount % 2) == 0){
            PORTC |= 0x04;
         }
         else{
            PORTC &= 0xFB;
         }
         buzzCount ++;
         break;
      default:
         buzzCount = 0;
         PORTC &= 0xFB;
         break;
   }
   switch(state){
      case 1:
         if(buzzCount >= 6){
            return 0;
         }
         return 1;
      case 2:
         if(buzzCount >= 32){
            return 0;
         }
         return 2;
      case 3:
         if(buzzCount >= 2){
            return 0;
         }
         return 3;
   }
   return 0;
}
