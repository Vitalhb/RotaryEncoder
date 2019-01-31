/*
	 Rotary Encoder - Interrupt Example

	 The circuit:
	 * encoder pin A to Arduino pin 3
	 * encoder pin B to Arduino pin 4
	 * encoder ground pin to ground (GND)
*/
#include <Arduino.h>
#include <buttonsTemplate.h>
#include <Port_IO.h>
#include <Rotary.h>

#define USE_DISPLAY // Comment this line if you don't have a TM1637 display

#ifdef USE_DISPLAY

#include <TM1637Display.h>
#define DISPLAY_CLK 7
#define DISPLAY_DIO 8
TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);

#endif //USE_DISPLAY

#define ENCODER_BUTTON	0
#define MAX_POSITION 999

Rotary<(uint16_t)&PORTD, BIT3, BIT4, MAX_POSITION> r;
const uint8_t button_pins[] = { 2 };
using buttons = Buttons<1>;

int16_t lastPosition, currentPosition;

void setup()
{
	Serial.begin(2000000);
	r.begin();
	buttons::begin(button_pins);
#ifdef USE_DISPLAY
	display.setBrightness(5);
	display.showNumberDec(8888);
#endif //USE_DISPLAY
	lastPosition = 0;
	sei(); // Enable global interrupts
	delay(1000);
	Serial.print("Position, ");
	Serial.println(currentPosition);
}

void loop()
{
	currentPosition = r.getPosition();
	if (currentPosition != lastPosition)
	{
		lastPosition = currentPosition;
		if (r.getDirection() == Rotation_Dir::DIR_CCW)
			Serial.print("Counter");

		Serial.print("Clockwise, ");
		Serial.println(currentPosition);
#ifdef USE_DISPLAY
		display.showNumberDec(currentPosition);
#endif //USE_DISPLAY
	}
	if (buttons::clicked(ENCODER_BUTTON))
	{
		currentPosition = 0;
		Serial.print("Position, ");
		Serial.println(currentPosition);
#ifdef USE_DISPLAY
		display.showNumberDec(currentPosition);
#endif //USE_DISPLAY
	}
}

ISR(PCINT2_vect)
{
	r.process();
}
