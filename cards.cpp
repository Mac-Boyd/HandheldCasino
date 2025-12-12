#ifndef CARDS_H
#define CARDS_H
#include <stdlib.h>

unsigned char* deck = malloc(52 * sizeof(unsigned char));
unsigned char deckSize = 0;
//THIS DRAW FUNCTION RELIES ON THE PROPER APPLICATION OF "CUT CARDS"
//THE DECK SHOULD BE RESHUFFLED ABOUT EVERY 28 CARDS
bool inDeck(unsigned char c);
unsigned char draw(){
   unsigned char toDraw = rand() % 52;
   if(deckSize > 30){
      deckSize = 0;
   }
   while(inDeck(toDraw)){
      toDraw = rand() % 52;
   }
   deck[deckSize] = toDraw;
   deckSize ++;
   return toDraw;
}
//there may be a faster way to optimize this but I will wait to see if it is necessary cause it will involve sorting
bool inDeck(unsigned char c){
   for (int i = 0; i < deckSize; i++){
      if(deck[i] == c){
         return true;
      }
   }
   return false;
}
//These could be implemented manually but this will provide better clarity of code
//0-12 A-K of CLUBS
//13-25 A-K of HEARTS
//26-38 A-K of SPADES
//39-51 A-K of DIAMONDS
unsigned char getSuit(unsigned char c){
   return (c/13);
}
//currently the rules of blackjack + baccarat mean that I only care about Ace + 2-9 + 10
unsigned char getFace(unsigned char c){
   char toReturn = c%13;
   switch (toReturn)
   {
      case 0:
         toReturn = 'A';
         return toReturn;
      case 9:
         toReturn = 1;
         break;
      case 10:
         toReturn = 'J';
         return toReturn;
      case 11:
         toReturn = 'Q';
         return toReturn;
      case 12:
         toReturn = 'K';
         return toReturn;
      default:
         break;
   }
   return (toReturn + 49);
}
unsigned char getVal(unsigned char c){
   return ((c%13 + 1) > 10)?10 : (c%13 + 1);
}
#endif
