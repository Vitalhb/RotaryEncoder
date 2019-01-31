/* Rotary encoder handler for arduino.
 *
 * Copyright 2011 Ben Buxton. Licenced under the GNU GPL Version 3.
 * Contact: bb@cactii.net
 *
 */

#include "Arduino.h"
#include "Rotary.h"

 /*
  * The below state table has, for each state (row), the new state
  * to set based on the next encoder output. From left to right in,
  * the table, the encoder outputs are 00, 01, 10, 11, and the value
  * in that position is the new state to set.
  */

#ifdef HALF_STEP
  // Use the half-step state table (emits a code at 00 and 11)
const uint8_t ttable[6][4] = {
	// R_START (00)
	{R_START_M,            R_CW_BEGIN,     R_CCW_BEGIN,  R_START},
	// R_CCW_BEGIN
	{R_START_M | DIR_CCW, R_START,        R_CCW_BEGIN,  R_START},
	// R_CW_BEGIN
	{R_START_M | DIR_CW,  R_CW_BEGIN,     R_START,      R_START},
	// R_START_M (11)
	{R_START_M,            R_CCW_BEGIN_M,  R_CW_BEGIN_M, R_START},
	// R_CW_BEGIN_M
	{R_START_M,            R_START_M,      R_CW_BEGIN_M, R_START | (uint8_t)Rotation_Dir::DIR_CW},
	// R_CCW_BEGIN_M
	{R_START_M,            R_CCW_BEGIN_M,  R_START_M,    R_START | (uint8_t)Rotation_Dir::DIR_CCW},
};
#else
  // Use the full-step state table (emits a code at 00 only)
const uint8_t ttable[7][4] = {
	// R_START
	{R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},
	// R_CW_FINAL
	{R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | (uint8_t)Rotation_Dir::DIR_CW},
	// R_CW_BEGIN
	{R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},
	// R_CW_NEXT
	{R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},
	// R_CCW_BEGIN
	{R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},
	// R_CCW_FINAL
	{R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | (uint8_t)Rotation_Dir::DIR_CCW},
	// R_CCW_NEXT
	{R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
};
#endif

