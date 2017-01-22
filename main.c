/*
 * SeniorDesign.c
 *
 * Created: 9/25/2016 9:17:25 PM
 * Author : aviles
 */ 

#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);  // Fast I2C / TWI

#include <avr/io.h>
#include <avr/interrupt.h>
//Hardware pin definitions
int UVOUT = A2; //Output from the sensor
int REF_3V3 = A1; //3.3V power on the Arduino board
int LDROUT = A3;

//other global variables
char charBuff[50];

float outputVoltage = 0.0;
int uvLevel = 0;
int refLevel = 0;
float uvIntensity = 0.0;
int main(void)

{
	 
	EIMSK |= (1 << INT0); // enables interrupt 1 and 0
	PCICR |= (1 << PCIE0); //Pin change interrupt control register is sent to one
	PCMSK0 |= (1<<PCINT0);
	
	//SPCR |= (1 << SPE)|(1<<MSTR); // enables spi interface and master
	
	//TWCR |= (1<<TWEN); //Enables i2c mode
	DDRC = 0xFF; //Port used to count how many sodis cycles have occurred
	DDRE = 0xFF; //PORT USED FOR AVG
	DDRF = 0x00; //input for uv sensor
	DDRD = 0x00;//INPUT VOLTAGE
	DDRB = 0xFF; //Outputs for  drinking water
	
	 #if defined(ARDUINO)
	 pinMode(13, OUTPUT);
	 digitalWrite(13, HIGH);
	 #endif
	 
	TIMSK0 = ((1<<OCIE0A) || (1<<TOIE0));
	TCCR0A = 0x02;//Again dummy value but his will control what mode and prescaler we are using
	sei();// enable global interrupt
	while(1){
}

}
int Turbidiy = 0; // This will be the light range read in from the UV sensor for Turbidity

int IdealTurb = 2; //Dummy value for ideal turb level
//Ideal Turbidity needs to be researched

int count; // used to count how many iterations are done

int def = 1000; // default timer value dummy variable
int Timer = 0; //dummy variables for timer
int cycleCount = 0; //This variable will count the number of sodis cycles
double movAverage = 20; // dummy variable for average


//This interrupt will handle the global reset 
ISR(PCINT0_vect, ISR_BLOCK) {
Timer = def;
PORTE = 0; // can use port as a global dynamic variable
count = 0;  // can use port as a global dynamic variable
PORTF = 0x00; // can use port as a global dynamic variable
PORTC = 0x00;
TCNT0 = 160; //This is a test value till we decide what clock we are using

	return; 
}

int uvValue = 0;

//Handle the timer and moving average 
//For the dummy i will use a 4 sec delay as reading average
ISR(INT0_vect,ISR_BLOCK)
{
	
	//using 100 and 50 as a dummy number for acceptable range
	PORTE = PORTE/count; //AVERAGES MOVAVERAGE
movAverage = PORTE;
if(((movAverage <= 100) && movAverage >= 50) && Turbidiy< IdealTurb){
		Timer = def; // sets defaults
		PORTE= 0; //sets reset
		count = 0;
					//drink water
					// essentially turning on a green led
					PORTB = 0x0F;
					return;
											}
		
		else{
				Timer = def; // sets defaults
				PORTE= 0; //sets reset FOR MOVE AVERAGE
				count = 0;
				//dont drink water
				//essential turns on a red dont drink led
				PORTB=0xF0;
				PORTC= PORTC+1;
	if(PORTC == 5){
		//TELL USER TO RUN WATER THROUGH FITLERS AGAIN and to hit global reset
	    PORTB=0xFF; //All lights are on
			    }
			return;
		}


}

void T0Delay(){
	while((TIFR &(1<<TOV0)==0)); //WAIT FOR DELAY TO FINISH
	
	TCCR0A=0; //turn off timer0
	TIFR = 0x1;//CLEAR TOV0
	movAverage= movAverage + uvIntensity // READ IN UV INTEN AND COMPUTES AVERAGE
}

ISR(TIMER0_COMPA_vect){
//tHIS CAN BE REPLACED USING BUILT IN DELAYS IE DELAY(29) OR WHAT EVER THE CODE IS
//checks to see if 4 minutes have passed
PORTE = averageAnalogRead(DDRF);
	count++;

}




/* 
 The ML8511 UV Sensor outputs an analog signal in relation to the amount of UV light it detects.
 This sensor detects 280-390nm light most effectively. This is categorized as part of the UVB (burning rays)
 spectrum and most of the UVA (tanning rays) spectrum.
*/


void drawUV()
{
  uvLevel = averageAnalogRead(UVOUT);
  refLevel = averageAnalogRead(REF_3V3);
  
  //Use the 3.3V power pin as a reference to get a very accurate output value from sensor
  outputVoltage = 3.3 / refLevel * uvLevel;
  
  uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0); //Convert the voltage to a UV intensity level
  // graphic commands to redraw the complete screen should be placed here  
  
  u8g.setFont(u8g_font_unifont);
  //
  //WHY IS THIS  HERE TWICE
  //
  uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0); //Convert the voltage to a UV intensity level
}


//Takes an average of readings on a given pin
//Returns the average
int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);  
}



	

