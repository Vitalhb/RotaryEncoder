# RotaryEncoder
 * Rotary encoder template library for Arduino.
 * Vital HB Software - 2019
 * Based on the work of Brian Low: https://github.com/brianlow/Rotary
 * Depends on Port_IO library.
 * You can use the library templateButtons to manage the button of the encoder and any other buttons in your project.
 *
 * Declare the interrupt handler
 * ISR(PCINTn_vect) // PCINT0_vect, PCINT1_vect or PCINT2_vect for PORTB, PORTC and PORTD.
 * {
 *    r.process(); // call the process() method from the interrupt routine
 * }
 *
