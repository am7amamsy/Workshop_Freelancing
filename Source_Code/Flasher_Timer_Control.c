/*
 * Flasher_Timer_Control.c
 *
 * Created: 07/06/2015 08:06:39 PM
 *  Author: ADMIN
 */ 


#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

#define HIGH 1
#define LOW 0
#define Error 0X4F

#define MostSegPORT PORTB
#define LeastSegPORT PORTD

#define ButtonPORT PINC	
#define SettingsB PINC0
#define StartB	PINC1
#define PauseB	PINC2
#define IncB	PINC3
#define DecB	PINC4

#define OutPORT PORTC
#define OutPin PINC5

#define NoiseTime 200

void seven_segment (volatile uint8_t *port, int num)
{
	if (num == 0)
	*port |= 0X7E;
	else if (num == 1)
	*port |= 0X30;
	else if (num == 2)
	*port |= 0X6D;
	else if (num == 3)
	*port |= 0X79;
	else if (num == 4)
	*port |= 0X33;
	else if (num == 5)
	*port |= 0X5B;
	else if (num == 6)
	*port |= 0X5F;
	else if (num == 7)
	*port |= 0X70;
	else if (num == 8)
	*port |= 0X7F;
	else if (num == 9)
	*port |= 0X7B;
	else
	*port |= Error;
}

int WorkTime = 20;
int DelayTime = 1;

int MostDig =2;
int LeastDig =0;

int i=0 ;
int j=0;

//flags
int SettingsFlag = LOW; // HIGH Work time  -- LOW Delay time

int main(void)
{	//PORT SETUP 
	
	DDRB = 0xFF;
	DDRD = 0xFF;
	DDRC = 0x00;
    PORTC =0xFF;
	
	DDRC |=(1<<OutPin);
	OutPORT &=~(1<<OutPin);

	MostDig = 0 ;
	LeastDig = DelayTime;
	seven_segment(&MostSegPORT,MostDig);
	seven_segment(&LeastSegPORT,LeastDig);
	while(1)
    {	
		// application code
		
		
		if (bit_is_clear(ButtonPORT,SettingsB))                  //    Edit Selection
		{	
			
			if (SettingsFlag==HIGH)
			{
				SettingsFlag=LOW;
				MostDig = 0 ;
				LeastDig = DelayTime;
				seven_segment(&MostSegPORT,MostDig);
				seven_segment(&LeastSegPORT,LeastDig);	 
			}
			if (SettingsFlag==LOW) 
			{
				SettingsFlag=HIGH;
				MostDig = WorkTime/10 ;
				LeastDig = WorkTime-MostDig*10;
				seven_segment(&MostSegPORT,MostDig);
				seven_segment(&LeastSegPORT,LeastDig);
			}
			_delay_ms(NoiseTime);
		}//Edit Selection
		
		if (bit_is_clear(ButtonPORT,IncB))     // Increase button pressed
		{
			if (SettingsFlag==HIGH)  //WORK TIME EDIT
			{
				WorkTime++;
				
				MostDig = WorkTime/10 ;
				LeastDig = WorkTime-MostDig*10;
				seven_segment(&MostSegPORT,MostDig);
				seven_segment(&LeastSegPORT,LeastDig);
			}//WORK TIME EDIT
			
			else if (SettingsFlag==LOW) //Delay TIME edit
			{
				DelayTime++;
				
				MostDig = 0 ;
				LeastDig = DelayTime;
				seven_segment(&MostSegPORT,MostDig);
				seven_segment(&LeastSegPORT,LeastDig);
			}//Delay TIME edit
			
			else			//ERROR 
			{
				seven_segment(&MostSegPORT,Error);
				seven_segment(&LeastSegPORT,Error);
			}//ERROR
			
			_delay_ms(NoiseTime); //debouncing effect
			
		}// Increase button pressed
		
		if (bit_is_clear(ButtonPORT,DecB))      //decrease button pressed
		{
			if (SettingsFlag==HIGH)  //WORK TIME EDIT
			{
				WorkTime--;
				
				MostDig = WorkTime/10 ;
				LeastDig = WorkTime-MostDig*10;
				seven_segment(&MostSegPORT,MostDig);
				seven_segment(&LeastSegPORT,LeastDig);
			}//WORK TIME EDIT
			
			else if (SettingsFlag==LOW) //Delay TIME edit
			{
				DelayTime--;
				
				MostDig = 0 ;
				LeastDig = DelayTime;
				seven_segment(&MostSegPORT,MostDig);
				seven_segment(&LeastSegPORT,LeastDig);
			}//Delay TIME edit
			
			else			//ERROR
			{
				seven_segment(&MostSegPORT,Error);
				seven_segment(&LeastSegPORT,Error);
			}//ERROR
			
			_delay_ms(NoiseTime); //debouncing effect
						
		}// decrease Button pressed
		
		if (bit_is_clear(ButtonPORT,StartB))    //start button press
		{
			for (i = 1; i<=(WorkTime*60/2/DelayTime);i++)
			{
				OutPORT |=(1<<OutPin);
				for (j=1;j<=DelayTime;j++)_delay_ms(1000);
				
				OutPORT &=~(1<<OutPin);
				for (j=1;j<=DelayTime;j++)_delay_ms(1000);
				if (bit_is_clear(ButtonPORT,PauseB))break ;
			}
				
		}//end start button 
		
		
    }
}