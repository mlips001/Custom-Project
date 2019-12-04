/*
 * SimonSays-CustomProject.c
 *
 * Created: 11/20/2019 11:51:17 AM
 * Author : Matthew L
 */ 

#include <avr/io.h>
//#include <avr/eeprom.h>
#include "timer.h"
#include "io.h"
#include "Randomize.c"
#include "PWM.h"
#include "Joystick.c"

int16_t j_Ldir = 0x00;
int16_t j_Rdir = 0x00;
int16_t j_Udir = 0x00;
int16_t j_Ddir = 0x00;

//unsigned char EEMEM eeprom_array[10];	


// The value for (~PINA & 0x0F) or  (j_Ldir) | (j_Rdir << 1) | (j_Udir << 2) | (j_Ddir << 3);
unsigned char currentValue;

void getInput() {	
	//a and b are initialized in Joystick.c
	
	j_Ldir = (a & 0x01) == 0x01;
	j_Udir = (b & 0x02) == 0x02;
	j_Rdir = (c & 0x03) == 0x03;
	j_Ddir = (d & 0x04) == 0x04;

}

// Game state.
enum Simon_Says{Wait, Init, Add_LED, Repeat_Order, Get_Input, Get_Next_Input, Lose, Win, Add_Points } game_state;


#define MAX_ROUNDS 9
// How long to way during playback.
#define MAX_WAIT_PER_LED 2


unsigned char Pattern[MAX_ROUNDS];	// Generated pattern

unsigned char Make_nextposition = 0;	// Next location for generation

unsigned char Rounds_Count = 0;	// How many values have been generated so far

unsigned char Repeatpattern_nextposition = 0;	// The next location for playback

unsigned char Check_nextposition = 0;	// The next location for program to check the user input against

// Counters for timing .
unsigned char PlayBackCount = 0;

void GAME_TICK()
{
	// Transition Table
	switch(game_state)
	{
		case Wait:
			if(j_Ldir || j_Rdir || j_Udir || j_Ddir ) {
				// Push any button to begin the game.
				game_state = Init;
				LCD_ClearScreen();
				LCD_DisplayString(1, "   Simon Says!   Move joystick.");
			}
			break;
		
		case Init:
			if( j_Ldir || j_Rdir || j_Udir || j_Ddir) {
				// Game starts after button press
				game_state = Init;
			}
			
			else {
				// They let go of the button, now its time to start the game.
				game_state = Add_LED;
			}
			break;
		
		case Add_LED:
			game_state = Repeat_Order;
			LCD_ClearScreen();
			LCD_DisplayString(1, "Repeat after me.  Points: ");
			LCD_WriteData('0' + Rounds_Count - 1);
			break;
		
		case Repeat_Order:
			// We have played back the whole generated sequence.
			if(Repeatpattern_nextposition >= Rounds_Count) {
				// Go wait for their input.
				game_state = Get_Input;
				// Reset everything.
				Repeatpattern_nextposition = 0;
				PORTB = 0x00;
				LCD_ClearScreen();
				LCD_DisplayString(1, "    You try.       Points: ");
				LCD_WriteData('0' + Rounds_Count - 1);
			}
			
			else {
				// We have given them enough time to look at the LED.
				if(PlayBackCount > MAX_WAIT_PER_LED) {
					game_state = Repeat_Order;
					Repeatpattern_nextposition++;
					PlayBackCount = 0;
				}
				else {
					// Wait for them to see the LED.
					PlayBackCount++;
				}
			}
			break;
		
		case Add_Points:
			// Update points
			PORTB = 0;
			if(j_Ldir || j_Rdir || j_Udir || j_Ddir) {
				game_state = Add_Points;
				LCD_ClearScreen();
				LCD_DisplayString(1, "That is correct! Points: ");
				LCD_WriteData('0' + (Rounds_Count));
			}
			
			else if(Check_nextposition < MAX_ROUNDS) {
				Check_nextposition = 0;
				game_state = Add_LED;
			}
			
			else {
				// They win the game if they hit 9 rounds
				game_state = Win;
				LCD_ClearScreen();
				LCD_DisplayString(1, "    You win!       Play again!");
			}
			break;
		
		case Win:
			if(j_Ldir || j_Rdir || j_Udir || j_Ddir) {
				// Restart the game.
				game_state = Wait;
				LCD_ClearScreen();
				LCD_DisplayString(1, "   Simon Says!   Move joystick");
				playSound(0);
			}
			break;
		
		case Lose:
			if(j_Ldir || j_Rdir || j_Udir || j_Ddir) {
				// Restart the game.
				game_state = Wait;
				LCD_ClearScreen();
				LCD_DisplayString(1, "   Simon Says!   Move joystick");
				playSound(0);
			}
			break;
		
		case Get_Input:
			playSound(0);
			if(Check_nextposition >= Rounds_Count) {
				// Win for the round because positions match
				game_state = Add_Points;
			}
			
			else {
				game_state = Get_Input;
				playSound(0);
			}
			break;
		
		case Get_Next_Input:
			if(j_Ldir || j_Rdir || j_Udir || j_Ddir) {
				game_state = Get_Next_Input;
			}
		
			else {
				Check_nextposition++;
				game_state = Get_Input;
				playSound(0);
			}
			break;
		
		default:
			game_state = Wait;
			break;
	}
	
	switch(game_state) {
		case Wait:
			playSound(0);
			break;
			
		case Get_Next_Input:
			break;
			
		case Lose:
			break;
			
		case Win:
			break;
			
		case Add_Points:
			break;
		
		case Init:
			// Reset all game states.
			Make_nextposition = 0;
			Repeatpattern_nextposition = 0;
			Rounds_Count = 0;
			Check_nextposition = 0;
			break;
		
		case Add_LED:
			PORTB = 0;
			if(Make_nextposition < MAX_ROUNDS) {
				Pattern[Make_nextposition] = getRandomLed();
				Make_nextposition++;
				Rounds_Count++;
			}
			
			else {
				// Some kind of win sequence.
				game_state = Win;
				playSound(0);
			}
			break;
		
		case Repeat_Order:
			if(Repeatpattern_nextposition < Rounds_Count) {
				PORTB = Pattern[Repeatpattern_nextposition] << 1;
				playSound(Pattern[Repeatpattern_nextposition]);
			}
			break;
		
		case Get_Input:
			currentValue =  (j_Ldir) | (j_Rdir << 1) | (j_Udir << 2) | (j_Ddir << 3);
			PORTB = currentValue << 1;
			playSound(currentValue);
			
			if(currentValue == 0) {	// No input.
				break;
			}
			
			else if(currentValue == Pattern[Check_nextposition]) {
				// It matched, keep moving forward to check next location.
				//Check_nextposition++;
				game_state = Get_Next_Input;
			}
		
			else {
				// Wrong input loses the game
				game_state = Lose;
				playSound(0);
				LCD_ClearScreen();
				LCD_DisplayString(1, "You Lost =(");
				PORTB = 0x00;
			}
			break;
	}
}

int main(void)
{
	// Init the input.
	DDRA = 0x00; PORTA = 0xFF;
	// Init the outputs
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	// Init the LCD.
	LCD_init();
	PWM_on();
	
	// Set the seed for the PNR.
	Initialize(1);				// Use a set seed so it could repeat
	
	TimerSet(200);
	TimerOn();
	
	game_state = Wait;
	
	LCD_ClearScreen();
	LCD_DisplayString(1, "   Simon Says!    *Press a key*");
	set_PWM(0);
	
	while (1)
	{
		LCD_Cursor(0);
		Tick();
		ADC_init();
		getInput();
		GAME_TICK();
		
		while(!TimerFlag);
		TimerFlag = 0;
	}
}
