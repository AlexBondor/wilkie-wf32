/*
 * util.h
 *
 *  Created on: May 2, 2016
 *      Author: Alex
 */

#ifndef UTIL_H_
#define UTIL_H_

#define MOVE_FORWARD  0
#define MOVE_BACKWARD 1
#define TURN_LEFT 	  2
#define TURN_RIGHT 	  3
#define STOP_ROBOT    4
#define TOGGLE_PUMP   5
#define TOGGLE_SERVO  6

typedef struct
{
	int x, y;
} point;

typedef struct
{
	int numberOfCommands;
	int vacuumStatus;
	int sensorsServoStatus;
	point position;
	int headingAngle;
	point leftSensor;
	point frontSensor;
	point rightSensor;
} robot_state;

#define OLED_OUTPUTS 0xF
#define OLED_DC   0x1
#define OLED_RES  0x2
#define OLED_VBAT 0x4
#define OLED_VDD  0x8

#define LED_3 1 << 23
#define LED_2 1 << 22
#define LED_1 1 << 21
#define LED_0 1 << 20

#define SW_3 1 << 19
#define SW_2 1 << 18
#define SW_1 1 << 17
#define SW_0 1 << 16

#endif /* UTIL_H_ */
