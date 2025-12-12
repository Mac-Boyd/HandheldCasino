# HandheldCasino
### User Guide
A user of the Mini-Casino has 4 buttons with which to interface with the system. Upon powering it on, they should wait until they hear the buzzer beep once to signal that the system is ready. Then, they can use the leftmost two buttons to cycle through the game options, and the third from the left button to select a game. Once a game is selected they will be prompted for their bet, which can be scrolled through using the two leftmost buttons. Increasing the bet at a bet of your maximum chips will result in the bet “wrapping around” to zero, and decreasing the bet at zero will similarly “wrap around” to your maximum bet. The third from the left button can be used to select their bet and move on. If they are playing Baccarat the user will then be asked to select the target of their bet. Again, they can use the left two buttons to scroll, and the third from the left button to “lock in” their choice. If the user is playing Baccarat, they can then sit back and relax while the Arduino plays out the remainder of the hand and determines the winner. If they are playing blackjack they’ll have to make a few more decisions. The buttons correspond to the following actions: Hit, Stay, Double, Split (left to right). After the first round of play, the only buttons that will work are Hit and Stay. The user can then play out the hand with those buttons. Once the result of a round of blackjack or baccarat is found, the arduino resets to the casino for the user to pick a new game.
### Hardware
● 4x Button

● LCD 1602

● Potentiometer (For adjusting LCD contrast)

● Active Buzzer
### Wiring Diagram
<img width="658" height="434" alt="image" src="https://github.com/user-attachments/assets/ffb1e7d6-d8dd-490f-b26d-9ae9bf18b4cd" />
