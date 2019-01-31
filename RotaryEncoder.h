/*
 * Rotary encoder template library for Arduino.
 * Based on the work of Brian Low: https://github.com/brianlow/Rotary
 * Depends on Port_IO library. https://github.com/Vitalhb/VHB_Utils
 * You can use the library templateButtons to manage the button of the encoder and any other buttons in your project.
 *
 * Declare the interrupt handler
 * ISR(PCINTn_vect) // PCINT0_vect, PCINT1_vect or PCINT2_vect for PORTB, PORTC and PORTD.
 * {
 *    r.process(); // call the process() method from the interrupt routine
 * }
 *
 * Vital H. B 2019
 *
 */

#pragma once

#include "Arduino.h"
#include <Port_IO.h>

#define IS_BIT_SET(BYTE, BIT) ((BYTE & BIT) ? 1:0)

#define R_START 0x0

#ifdef HALF_STEP
 // Use the half-step state table (emits a code at 00 and 11)
#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5
#else
 // Use the full-step state table (emits a code at 00 only)
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6
#endif



 // Enable this to emit codes twice per step.
 // #define HALF_STEP

// Values returned by 'getDirection'
// DIR_NONE: No complete step yet.
// DIR_CW: Clockwise step.
// DIR_CCW: Counter-clockwise step.
enum class Rotation_Dir : uint8_t { DIR_NONE = 0x0, DIR_CW = 0x10, DIR_CCW = 0x20 };

extern const uint8_t ttable[7][4];

// Rotary<(uint16_t)&PORTN, PIN1, PIN2, MAX_POSITION, bSATURATE> rotaryEncoder;
// PORTN can be PORTB, PORTC, PORTD
// MAX_POSITION (default 0) is the maximum number that position can reach. If Zero, there is no maximum position, besides the int16 overflow/underflow.
// bSATURATE (default true). If true, position will stop at MAX_POSITION. 
// If false, position will go to zero at MAX_POSITION and turns will increment/decrement, depending on the direction of turn.
template <uint16_t port, uint8_t pin1_Bit, uint8_t pin2_Bit, int16_t maxPosition = 0, bool saturate = true>
class Rotary
{
public:
	Rotary() __attribute__((always_inline)) : encoder_port(pin1_Bit | pin2_Bit, 0) {}

	void begin(bool pullup = true) __attribute__((always_inline))
	{
		state = R_START;
		position = 0;
		turns = 0;

		if (pullup)
		{
			// Enable weak pullups
			encoder_port.SetBits(pin1_Bit | pin2_Bit);
		}

		if (port == (uint16_t)&PORTB)
		{
			PCICR |= (1 << PCIE0); // Enable Pin Change Interrupts for Port B
			PCMSK0 |= pin1_Bit | pin2_Bit; // Enable PCI for pins 1 and 2
		}
		if (port == (uint16_t)&PORTC)
		{
			PCICR |= (1 << PCIE1); // Enable Pin Change Interrupts for Port C
			PCMSK1 |= pin1_Bit | pin2_Bit; // Enable PCI for pins 1 and 2
		}
		if (port == (uint16_t)&PORTD)
		{
			PCICR |= (1 << PCIE2); // Enable Pin Change Interrupts for Port D
			PCMSK2 |= pin1_Bit | pin2_Bit; // Enable PCI for pins 1 and 2
		}

	}

	// Call this method inside the interrupt routine or in a fast loop.
	void process();

	void setPosition(int16_t newPosition = 0, int8_t newTurns = 0)  __attribute__((always_inline)) { position = newPosition; turns = newTurns; }
	int16_t getPosition() __attribute__((always_inline)) { return position; }
	int8_t getTurns() __attribute__((always_inline)) { return turns; }
	Rotation_Dir getDirection()  __attribute__((always_inline)) { return rotationDir; }

private:
	TPort_IO<port> encoder_port;

	uint8_t state;
	int8_t turns;
	int16_t position;
	Rotation_Dir rotationDir;
};

template <uint16_t port, uint8_t pin1_Bit, uint8_t pin2_Bit, int16_t maxPosition, bool saturate>
void Rotary<port, pin1_Bit, pin2_Bit, maxPosition, saturate>::process()
{
	// Grab state of input pins.
	uint8_t portValue = encoder_port.Read();
	uint8_t pinstate = (IS_BIT_SET(portValue, pin2_Bit) << 1) | IS_BIT_SET(portValue, pin1_Bit);
	// Determine new state from the pins and state table.
	state = ttable[state & 0xf][pinstate];
	// Return emit bits, ie the generated event.
	rotationDir = (Rotation_Dir)(state & 0x30);
	if (rotationDir == Rotation_Dir::DIR_CW)
	{
		position++;
		if (maxPosition)
		{
			if (position > maxPosition)
			{
				if (saturate)
					position = maxPosition;
				else
				{
					position = 0;
					turns++;
				}
			}
		}
	}
	else if (rotationDir == Rotation_Dir::DIR_CCW)
	{
		position--;
		if (maxPosition)
		{
			if (position < -maxPosition)
			{
				if (saturate)
					position = -maxPosition;
				else
				{
					position = 0;
					turns--;
				}
			}
		}
	}
}
