/*
 * Sewing_Machine_Control.c
 *
 * Created: 05/02/2015 01:07:17 PM
 *  Author: ADMIN
 */ 

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

#define PressPORT PIND
#define PressPIN PIND0	

#define SensorPORT PIND
#define SensorUPIN PIND1
#define SensorLPIN PIND2

#define RelayPORT PORTD
#define Relay1PIN PIND5
#define Relay2PIN PIND6

#define NoiseTime 200
#define CaptureTime 2000


int main(void)
{
	DDRD = 0b01100000;
	PORTD = 0b00000111;
    while(1)
    {
        // application code 
		while(bit_is_set(PressPORT,PressPIN) );		//if button pressed	
		_delay_ms(NoiseTime);
		while (bit_is_set(PressPORT,PressPIN) );	//if button Pressed again
		_delay_ms(NoiseTime);

				RelayPORT|=(1<<Relay1PIN);					//relay 1 on
				while(bit_is_set(SensorPORT,SensorUPIN));	//while upper sensor is high 
				_delay_ms(NoiseTime);
				RelayPORT|=(1<<Relay2PIN);					//relay 2 on
				_delay_ms(CaptureTime);						//delay 2sec
				RelayPORT &=~(1<<Relay1PIN);				//relay1 off
				while(bit_is_set(SensorPORT,SensorLPIN));	//while lower sensor is high
				_delay_ms(NoiseTime);
				RelayPORT &=~(1<<Relay2PIN);				//relay 2 off
				_delay_ms(CaptureTime);						//delay 2sec
			
		
    }
}