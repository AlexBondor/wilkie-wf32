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

#define MAX_ARRAY_SIZE 10000

point SensorsRawData[MAX_ARRAY_SIZE * 3];
point RobotPosition[MAX_ARRAY_SIZE];
int RobotHeading[MAX_ARRAY_SIZE];

int IicReadingsCount = 0;

int main()
{
	xil_printf("Hello\r\n");

//	MapControllerInitializeMap();
//	int i;
//	for (i = 0; i < 20; i++)
//	{
//		MapControllerPrintMap();
//		sleep(2);
//	}

//	if (IicInit() != XST_SUCCESS)
//	{
//		xil_printf("Failed to initialize iic component\r\n");
//		return -1;
//	}
//
//	robot_state CurrentRobotState;
//
//	while(1)
//	{
//		CurrentRobotState = IicGetRobotState();
//
//		RobotPosition[IicReadingsCount] = CurrentRobotState.position;
//		RobotHeading[IicReadingsCount] = CurrentRobotState.headingAngle;
//		SensorsRawData[IicReadingsCount * 3] = CurrentRobotState.leftSensor;
//		SensorsRawData[IicReadingsCount * 3 + 1] = CurrentRobotState.frontSensor;
//		SensorsRawData[IicReadingsCount * 3 + 2] = CurrentRobotState.rightSensor;
//
//		IicPrintRobotState(CurrentRobotState);
//
//		IicReadingsCount++;
//
//		if (IicReadingsCount == 20)
//		{
//			sendCommand(TOGGLE_PUMP, 0);
//			break;
//		}
//		usleep(100000);
//	}

//	xil_printf("Hello World\n\r");
//	int status;
//	int i;
//
//	XGpio xGpio;
//	XGpio_Config *xGpioConfig;
//
//    init_platform();
//
//    xGpioConfig = XGpio_LookupConfig(GPIO_DEVICE_ID);
//	if (NULL == xGpioConfig) {
//		return XST_FAILURE;
//	}
//
//    status = XGpio_CfgInitialize(&xGpio, xGpioConfig, xGpioConfig->BaseAddress);
//	if (status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}
//    xil_printf("Gpio init successful\n\r");
//
//
//    xil_printf("Initialize OLED pins\n\r");
//
//	XSpiPs xSpi;
//	XSpiPs_Config *xSpiConfig;
//
//	xSpiConfig = XSpiPs_LookupConfig(SPI_DEVICE_ID);
//	if (NULL == xSpiConfig) {
//		return XST_FAILURE;
//	}
//
//	status = XSpiPs_CfgInitialize((&xSpi), xSpiConfig,
//			xSpiConfig->BaseAddress);
//	if (status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}
//
//	status = XSpiPs_SetOptions((&xSpi), (XSPIPS_CR_CPHA_MASK) | \
//			(XSPIPS_CR_CPOL_MASK));
//	if (status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}
//	XSpiPs_Enable((&xSpi));
//
//    xil_printf("Spi init successful\n\r");
//
//
//
//    XGpio_SetDataDirection(&xGpio, 1, SW_3 | SW_2 | SW_1 | SW_0);
//    while(1)
//    {
//    	XGpio_DiscreteWrite(&xGpio, 1, XGpio_DiscreteRead(&xGpio, 1) << 4 | OLED_VDD);
//    }
////    XGpio_DiscreteWrite(&xGpio, 1, OLED_VDD);
////
////    u8 init_vector[23] = {0xae, 0xd5, 0x80, 0xa8, 0x1f, 0xd3, 0x00, 0x40, 0x8d, 0x14, 0xa1, 0xc8, 0xda, 0x02, 0x81, 0x8f, 0xd9, 0xf1, 0xdb, 0x40, 0xa4, 0xa6, 0xaf};
////    for (i = 0; i < 23; i++)
////    {
////    	u8 byte = init_vector[i];
////		XGpio_DiscreteWrite(&xGpio, 1, OLED_VDD); // set DC to command
////		XSpiPs_SetSlaveSelect(&xSpi, 0x0); //select the slave
////		XSpiPs_PolledTransfer(&xSpi, byte, NULL, 1); //write the word
////		XGpio_DiscreteWrite(&xGpio, 1, OLED_VDD | OLED_DC); // set DC to data
////    }
//
//    getchar();
//
//    cleanup_platform();
//    return 0;
}

void sendCommand(int command, int amount)
{
	u8 cmd[6];
	sprintf(cmd, "n");
	IicSendCommand(cmd, strlen(cmd));

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
	IicSendCommand(cmd, strlen(cmd));

	sprintf(cmd, "n");
	IicSendCommand(cmd, strlen(cmd));

	robot_state CurrentRobotState = IicGetRobotState();
}
