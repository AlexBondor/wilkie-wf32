/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * wilkie_main.c: Wilkie autonomous vacuum cleaner Zybo controller logic
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <stdlib.h>
#include "xgpio.h"
#include "xspips.h"		/* SPI device driver */
#include "wilkie_main.h"
#include "util/platform.h"
#include "util/util.h"
#include "iic/iic_slave_polling.h"
#include "map/map_controller.h"


#define GPIO_DEVICE_ID XPAR_GPIO_0_DEVICE_ID
#define SPI_DEVICE_ID XPAR_XSPIPS_0_DEVICE_ID

void printMove(int next)
{
	switch(next)
	{
	case 0:
		xil_printf("MOVE_FORWARD\r\n");
		break;
	case 1:
		xil_printf("TURN_LEFT\r\n");
		break;
	case 2:
		xil_printf("TURN_RIGHT\r\n");
		break;
	case 3:
		xil_printf("TURN_180\r\n");
		break;
	case 4:
		xil_printf("BRAKE\r\n");
		break;
	}
}

int targetIds[BLOCKS_COUNT];
int demoTargets[2];
int currentBlockId, targetBlockId;
int targetStep = 1;
int heading = 90;
int trueHeading;
int offsetAngle;

long counter = 0;

void correctHeading()
{
	xil_printf("correctHeading %d %d\r\n", heading, trueHeading);
	if (trueHeading != heading)
	{
		if (heading == 0)
		{
			if (trueHeading < 45)
			{
				offsetAngle = trueHeading;
			}
			else
			{
				offsetAngle = trueHeading - 360;
			}
		}
		else
		{
			if (heading == 360)
			{
				if (trueHeading < 45)
				{
					offsetAngle = trueHeading;
				}
				else
				{
					offsetAngle = trueHeading - 360;
				}
			}
			else
			{
				offsetAngle = trueHeading - heading;
			}
		}
	}

	if (offsetAngle < 0)
	{
		if (offsetAngle <= -5)
		{
			sendCommand(TURN_LEFT, -offsetAngle);
		}
		xil_printf("Turn left %d degrees\r\n", -offsetAngle);
		usleep(1000000);
	}
	else
	{
		if (offsetAngle >= 5)
		{
			sendCommand(TURN_RIGHT, offsetAngle);
		}
		xil_printf("Turn right %d degrees\r\n", offsetAngle);
		usleep(1000000);
	}
}

int main()
{
	xil_printf("hello\r\n");

	XGpio_Config * xGpioConfig;
	XGpio xGpio;

	xGpioConfig = XGpio_LookupConfig(GPIO_DEVICE_ID);

	int status = XGpio_CfgInitialize(&xGpio, xGpioConfig, xGpioConfig->BaseAddress);
	if (status != XST_SUCCESS)
	{
		xil_printf("Failed to initialize GPIO\r\n");
	}

	XGpio_SetDataDirection(&xGpio, 1, 0);
	XGpio_DiscreteWrite(&xGpio, 1, LED_0);

	int j, toggle = 1;
//	for (j = 0; j < 15; j++)
//	{
//		if (toggle == 1)
//		{
//			toggle = 0;
//			XGpio_DiscreteWrite(&xGpio, 1, LED_0);
//		}
//		else
//		{
//			toggle = 1;
//			XGpio_DiscreteWrite(&xGpio, 1, 0);
//		}
//		usleep(1000000);
//	}

	if (IicInit() != XST_SUCCESS)
	{
		xil_printf("Failed to initialize iic component\r\n");
		return -1;
	}
	XGpio_DiscreteWrite(&xGpio, 1, LED_1);

	MapControllerInitializeMap();

	int i, blockX, blockY, asc = 1;
	for (i = 0; i < BLOCKS_COUNT; i += 2)
	{
		blockX = i + 4;
		if (asc == 1)
		{
			blockY = 4;
		}
		else
		{
			blockY = BLOCKS_COUNT - 5;
		}
		targetIds[i] = blockMap[blockX][blockY].id;
		if (asc == 1)
		{
			blockY = BLOCKS_COUNT - 5;
			asc = 0;
		}
		else
		{
			blockY = 4;
			asc = 1;
		}
		targetIds[i + 1] = blockMap[blockX][blockY].id;
	}

	for (i = 0; i < BLOCKS_COUNT; i++)
	{
		xil_printf("<%d, %d> ", i, targetIds[i]);
	}

	robot_state CurrentRobotState;

	xil_printf("ready\r\n");

	demoTargets[0] = blockMap[20][20].id;
	demoTargets[1] = blockMap[6][20].id;

	targetStep = 0;

	while(targetStep != 2)
	{
		if (targetStep == 0)
		{
			currentBlockId = demoTargets[0];
			targetBlockId = demoTargets[1];
		}
		else
		{
			currentBlockId = demoTargets[1];
			targetBlockId = demoTargets[0];
		}
		point targetBlockCoordinates = MapControllerIdToPoint(targetBlockId);
		point targetBlockPosition = blockMap[targetBlockCoordinates.x][targetBlockCoordinates.y].position;

		while (1)
		{
			CurrentRobotState = IicGetRobotState();

			trueHeading = CurrentRobotState.headingAngle;

			MapControllerConsumeCurrentRobotState(CurrentRobotState);

			if (CurrentRobotState.numberOfCommands == 0 && currentBlockId == targetBlockId)
			{
				xil_printf("next one\r\n");
				targetStep ++;
				break;
			}

			counter ++;
			if (counter >= 30)
			{
				correctHeading();

				point positionBlock = MapControllerBlockAt(CurrentRobotState.position.x, CurrentRobotState.position.y);
				currentBlockId = blockMap[positionBlock.x][positionBlock.y].id;
				int nextMove = MapControllerGetNextMove(CurrentRobotState.position, targetBlockPosition, CurrentRobotState.headingAngle);

				if (CurrentRobotState.numberOfCommands == 0)
				{
					switch(nextMove)
					{
						case 0:
							sendCommand(MOVE_FORWARD, 5);
							break;
						case 1:
							sendCommand(TURN_LEFT, 90);
							heading += 90;
							break;
						case 2:
							sendCommand(TURN_RIGHT, 90);
							heading -= 90;
							break;
						case 3:
							sendCommand(TURN_LEFT, 180);
							heading += 180;
							break;
						case 4:
							sendCommand(STOP_ROBOT, 0);
							break;
					}
					heading = heading < 0 ? 360 + heading : heading;
					heading = heading > 360 ? heading - 360 : heading;
					printMove(nextMove);
				}
				counter = 0;
			}
			usleep(100000);
		}
	}

	targetStep = 0;

	while(targetStep <= BLOCKS_COUNT)
	{
		currentBlockId = targetIds[targetStep - 1];
		targetBlockId = targetIds[targetStep];
		point targetBlockCoordinates = MapControllerIdToPoint(targetBlockId);
		point targetBlockPosition = blockMap[targetBlockCoordinates.x][targetBlockCoordinates.y].position;

		while (1)
		{
			CurrentRobotState = IicGetRobotState();

			trueHeading = CurrentRobotState.headingAngle;

			MapControllerConsumeCurrentRobotState(CurrentRobotState);

			if (CurrentRobotState.numberOfCommands == 0 && currentBlockId == targetBlockId)
			{
				xil_printf("next one\r\n");
				targetStep ++;
				break;
			}

			counter ++;
			if (counter >= 30)
			{
				correctHeading();

				point positionBlock = MapControllerBlockAt(CurrentRobotState.position.x, CurrentRobotState.position.y);
				currentBlockId = blockMap[positionBlock.x][positionBlock.y].id;
				int nextMove = MapControllerGetNextMove(CurrentRobotState.position, targetBlockPosition, CurrentRobotState.headingAngle);

				if (CurrentRobotState.numberOfCommands == 0)
				{
					switch(nextMove)
					{
						case 0:
							sendCommand(MOVE_FORWARD, 5);
							break;
						case 1:
							sendCommand(TURN_LEFT, 90);
							heading += 90;
							break;
						case 2:
							sendCommand(TURN_RIGHT, 90);
							heading -= 90;
							break;
						case 3:
							sendCommand(TURN_LEFT, 180);
							heading += 180;
							break;
						case 4:
							sendCommand(STOP_ROBOT, 0);
							break;
					}
					heading = heading < 0 ? 360 + heading : heading;
					heading = heading > 360 ? heading - 360 : heading;
					printMove(nextMove);
				}
				counter = 0;
			}
			usleep(100000);
		}
	}
	return 0;
}

int c = 0;

void sendCommand(int command, int amount)
{
	u8 cmd[6];
//	sprintf(cmd, "n");
//	IicSendCommand(cmd, strlen(cmd));

	switch(command)
	{
	case MOVE_FORWARD:
		sprintf(cmd, "mf%d", amount);
		break;
	case MOVE_BACKWARD:
		sprintf(cmd, "mb%d", amount);
		break;
	case TURN_LEFT:
		sprintf(cmd, "tl%d", amount);
		break;
	case TURN_RIGHT:
		sprintf(cmd, "tr%d", amount);
		break;
	case TOGGLE_PUMP:
		sprintf(cmd, "pv");
		break;
	case TOGGLE_SERVO:
		sprintf(cmd, "po");
		break;
	case STOP_ROBOT:
		sprintf(cmd, "s");
		break;
	default:
		break;
	}
	xil_printf("%s %d\r\n", cmd, c++);
	IicSendCommand(cmd, strlen(cmd));

	sprintf(cmd, "n");
	IicSendCommand(cmd, strlen(cmd));

	robot_state CurrentRobotState = IicGetRobotState();
}
