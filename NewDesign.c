/*
TEST DESIGN
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
	
	
	//These will be used as outputs/IN
	DDRC = 0xFF; //Port used to count how many sodis cycles have occurred
	DDRE = 0xFF; //PORT USED FOR AVG
	DDRF = 0x00; //input for uv sensor
	DDRD = 0x00;//INPUT VOLTAGE
	DDRB = 0xFF; //Outputs for  drinking water
	//END OF VARIALBE OUT/IN
	
	
	 #if defined(ARDUINO)  ///NEED TO FIX THIS FOR AVR
	 pinMode(13, OUTPUT);
	 digitalWrite(13, HIGH);
	 #endif 
	 //Following code for reset button and turbidity meter int
	 EIMSK |= (1<<INT0) | (1<<INT1); 
	 
	//Following code for timer 0
	TCNT0 = 0xFF; //Timer value for .06528 sec
	TCCR0A = 0x05; //Normal mode,int clk, 1024 prescaler
	TIMSK0 = (1<<TOIE0);//Enables timer int 0 
	

	sei();// enable global interrupt
	while(1){
		     
			T2Delay();
			canDrink();
}

void T2Delay(){  //6 hour delay
	int i = 0;
	while(i<21600){ // that value will get 6hr
		T1Delay();
	}
}

//This interrupt will handle the global reset 
ISR(INT0_vect) {
uvIntensity = 0; // UV inten 
movAverage = 0; // Moving average
PORTE = 0; // can use port as a global dynamic variable
PORTF = 0x00; //Use for UV
PORTC = 0x00; // count the times SODIS occurs
count = 0; // count the number of readings
	return; 
}

//Handle the timer and moving average 
//For the dummy i will use a 4 sec delay as reading average
void canDrink()
{
	//using 100 and 50 as a dummy number for acceptable range
movAverage = movAverage/count; //AVERAGES MOVAVERAGE

//value for UV needs to be found
if(((movAverage <= 100) && Turbidiy < IdealTurb){
					//drink water
					// essentially turning on a green led
					PORTB = 0x0F;
					return;
											}
		
		else{
				//dont drink water
				//essential turns on a red dont drink led
				PORTB=0xF0;
				PORTC= PORTC+1; //Used to count the number of interations SODIS OCCURS
	
	if(PORTC == 5){
		//TELL USER TO RUN WATER THROUGH FITLERS AGAIN and to hit global reset
	    PORTB=0xFF; //All lights are on
			    }
			return;
		}


}


ISR(TIMER0_OVF_vect){
uvIntensity = averageAnalogRead(DDRF);
movAverage= movAverage + uvIntensity // READ IN UV INTEN AND COMPUTES AVERAGE
	count++; //cOUNT NUMBER OF READINGS

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


//
void T1Delay(){ //function for 10 sec delay. Can be reused
TCNT1H = 0x67; //lOAD VALUES
TCNT1L = 0X69;

TCCR1A = 0x00; //Normal mode
TCCR0B = 0x05;  // normal mode, 1024 prescaler

	while((TIFR1 &(1<<TOV1)==0)); //WAIT FOR DELAY TO FINISH
	
	TCCR1B= 0; //turn off timer0
	TIFR1 = 0x1<<TOV1;//CLEAR TOV1
}

	

