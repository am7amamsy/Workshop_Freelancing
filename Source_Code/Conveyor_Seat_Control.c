/*
 * Conveyor_Seat_Control.c
 *
 * Created: 7/21/2016 3:18:28 PM
 *  Author: Carnival
 */ 

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

#define ButtonPORT PIND
#define ButtonPIN PIND0

#define LimitSwitchPORT PIND
#define LSRight PIND1
#define LSLeft  PIND2

#define RelayPORT PORTD
#define RelayRight PIND5
#define RelayLeft PIND6

#define OperationTime 240
#define DelayPeriod 500
#define NoiseTime 200

char StartOrStop = 0;
int OperationTimer = 0;
int DelayCounter = 0;

void InputCombinations(void)
{
	if(bit_is_clear(ButtonPORT,ButtonPIN) && StartOrStop == 0)		//When pushing button for 1st time, the motor starts moving right
	{																	
		_delay_ms(NoiseTime);
		while(bit_is_clear(ButtonPORT,ButtonPIN));
		_delay_ms(NoiseTime);
		StartOrStop ++;												//Flag is set that the button is pressed
		TCNT1 = 0;
		OperationTimer = 0;											//Start operation timer in seconds
		RelayPORT |= (1 << RelayRight);
	}/*
	if(bit_is_clear(ButtonPORT,ButtonPIN) && StartOrStop == 1)		//When pushing button for 2nd time, the motor stops
	{
		_delay_ms(NoiseTime);
		RelayPORT &= ~(1 << RelayRight);
		RelayPORT &= ~(1 << RelayLeft);
		while(bit_is_clear(ButtonPORT,ButtonPIN) || bit_is_clear(LimitSwitchPORT,LSRight) || bit_is_clear(LimitSwitchPORT,LSLeft));	
		_delay_ms(NoiseTime);										//Operation stops even if shaft is pressing limit switches
		StartOrStop = 0;											//Flag is cleared for future pressings
		TCNT1 = 0;													
		OperationTimer = 0;											//Operation timer is reset
	}*/
	if(bit_is_clear(LimitSwitchPORT,LSRight) && bit_is_clear(LimitSwitchPORT,LSLeft))	//For mechanical interlock
	{
		StartOrStop = 0;						//If the two limit switches are pressed, the motor stops immediately
		TCNT1 = 0;						//The operation timer is reset and the operation does not start again until one or both are not pressed
		RelayPORT &= ~(1 << RelayRight);
		RelayPORT &= ~(1 << RelayLeft);
		while(bit_is_clear(LimitSwitchPORT,LSRight) && bit_is_clear(LimitSwitchPORT,LSLeft));
	}
}

void DelayFunction(void)		//Self-made function for delay time to keep operation time accurate
{
	TCNT0 = 0;								//Delay timer is cleared to start counting from zero when calling function
	while(DelayCounter < DelayPeriod)		//While delay timer is less than intended delay time in micro seconds, it just keeps incrementing
	{
		if(TCNT0 >= 124)
		{
			TCNT0 = 0;
			DelayCounter ++;
		}
		if (TCNT1 >= 15624)					//The AVR does nothing but counting for both operation and delay
		{
			TCNT1 = 0;
			OperationTimer ++;
		}
		InputCombinations();				//If any input is entered while delay, it will be responded to according to the above function
	}
	DelayCounter = 0;
}

int main(void)
{	
	DDRD = 0b01100000;				//Push button on pinD0, limit switches on D1 and D2, relays on D5 and D6
	PORTD = 0b00000111;
	TCCR0 |= (1 << CS01);					//Timer 0 is set on a pre-scaler value of 8 for delay in milliseconds
	TCCR1B |= ((1 << CS11) | (1 << CS10));	//Timer 1 is set on a pre-scaler value of 64 for operation in seconds
	
    while(1)
    {
		InputCombinations();
		if(StartOrStop == 1)
		{	
			while(OperationTimer < OperationTime)
			{
				if(bit_is_clear(LimitSwitchPORT,LSRight))
				{
					RelayPORT &= ~(1 << RelayLeft);
					DelayFunction();
					RelayPORT |= (1 << RelayRight);
				}
				if(bit_is_clear(LimitSwitchPORT,LSLeft))
				{
					RelayPORT &= ~(1 << RelayRight);
					DelayFunction();
					RelayPORT |= (1 << RelayLeft);
				}
				if (TCNT1 >= 15624)
				{
					TCNT1 = 0;
					OperationTimer ++;
				}
				InputCombinations();
			}
			OperationTimer = 0;
			StartOrStop = 0;
			RelayPORT &= ~(1 << RelayRight);
			RelayPORT &= ~(1 << RelayLeft);
		}
    }
}