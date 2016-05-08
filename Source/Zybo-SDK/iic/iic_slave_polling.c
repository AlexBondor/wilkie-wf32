/***************************** Include Files **********************************/
#include "xparameters.h"
#include "xiicps.h"
#include "xil_printf.h"
#include "../util/util.h"
#include "iic_slave_polling.h"

/************************** Constant Definitions ******************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define IIC_DEVICE_ID		XPAR_XIICPS_0_DEVICE_ID

/*
 * The slave address to send to and receive from.
 */
#define IIC_SLAVE_ADDR		0x08
#define IIC_SCLK_RATE		100000

/*
 * The following constant controls the length of the buffers to be sent
 * and received with the IIC.
 */
#define RECV_BUFFER_SIZE	32

/************************** Variable Definitions ******************************/

XIicPs Iic;		/**< Instance of the IIC Device */

/*
 * The following buffers are used in this example to send and receive data
 * with the IIC.
 */
u8 RecvBuffer[RECV_BUFFER_SIZE];    /**< Buffer for Receiving Data */

/*****************************************************************************/
/**
*
* This function simply initializes the iic component of the board
*
*******************************************************************************/
int IicInit()
{
	xil_printf("Initialize iic component\r\n");

	int Status;
	XIicPs_Config *Config;
	int Index;

	/*
	 * Initialize the IIC driver so that it's ready to use
	 * Look up the configuration in the config table,
	 * then initialize it.
	 */
	Config = XIicPs_LookupConfig(IIC_DEVICE_ID);
	if (NULL == Config)
	{
		return XST_FAILURE;
	}

	Status = XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XIicPs_SelfTest(&Iic);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	/*
	 * Set the IIC serial clock rate.
	 */
	XIicPs_SetSClk(&Iic, IIC_SCLK_RATE);

	/*
	 * Initialize the send buffer bytes with a pattern to send and the
	 * the receive buffer bytes to zero to allow the receive data to be
	 * verified.
	 */
	for (Index = 0; Index < RECV_BUFFER_SIZE; Index++)
	{
		RecvBuffer[Index] = 0;
	}

	/*
	 * Wait until bus is idle to start another transfer.
	 */
	while (XIicPs_BusIsBusy(&Iic)) {
		/* NOP */
	}
	// Wait until slave is ready to communicate
	Status = XIicPs_MasterRecvPolled(&Iic, RecvBuffer, RECV_BUFFER_SIZE, IIC_SLAVE_ADDR);
	while (Status != XST_SUCCESS)
	{
		Status = XIicPs_MasterRecvPolled(&Iic, RecvBuffer, RECV_BUFFER_SIZE, IIC_SLAVE_ADDR);
		usleep(100000);
	}

	xil_printf("iic component initialized and ready to request data!\r\n");

	return XST_SUCCESS;
}

/**
 * Returns a structure containing the position, heading and data read from sensors
 * of the robot. The structure contains the parsed data requested by IIC from the
 * slave board.
 */
robot_state IicGetRobotState()
{
	int Status, Index;
	// phase used for parsing the input taken from iic buffer
	// 0 - positionX
	// 1 - positionY
	// 2 - headingAngle
	// 3 - leftSensorX
	// 4 - leftSensorY
	// 5 - frontSensorX
	// 6 - frontSensorY
	// 7 - rightSensorX
	// 8 - rightSensorY
	int Phase = 0;
	int Sign = 1;

	robot_state Result = IicGetInvalidRobotState();

	while (XIicPs_BusIsBusy(&Iic)) {
		/* NOP */
	}

	Status = XIicPs_MasterRecvPolled(&Iic, RecvBuffer, RECV_BUFFER_SIZE, IIC_SLAVE_ADDR);
	if (Status != XST_SUCCESS) {
		// failed at this point..
		// do something about it
		return Result;
	}

	/*
	 * Verify received data is correct.
	 */
	char CurrentChar;
	int CurrentNumber = 0;
	int StoreData = 0;
	for (Index = 0; Index < RECV_BUFFER_SIZE; Index++)
	{
		StoreData = 0;
		CurrentChar = RecvBuffer[Index];
		switch (CurrentChar)
		{
			case '-':
				Sign = -1;
				break;
			case ',':
				CurrentNumber *= Sign;
				StoreData = 1;
				break;
			default:
				CurrentNumber = CurrentNumber * 10 + (CurrentChar - '0');
				break;
		}
		if (StoreData == 1)
		{
			switch(Phase)
			{
				case 0:
					Result.numberOfCommands = CurrentNumber;
					break;
				case 1:
					Result.vacuumStatus = CurrentNumber;
					break;
				case 2:
					Result.sensorsServoStatus = CurrentNumber;
					break;
				case 3:
					Result.position.x = CurrentNumber;
					break;
				case 4:
					Result.position.y = CurrentNumber;
					break;
				case 5:
					Result.headingAngle = CurrentNumber;
					break;
				case 6:
					Result.leftSensor.x = CurrentNumber;
					break;
				case 7:
					Result.leftSensor.y = CurrentNumber;
					break;
				case 8:
					Result.frontSensor.x = CurrentNumber;
					break;
				case 9:
					Result.frontSensor.y = CurrentNumber;
					break;
				case 10:
					Result.rightSensor.x = CurrentNumber;
					break;
				case 11:
					Result.rightSensor.y = CurrentNumber;
					return Result;
					break;
				default:
					break;
			}
			CurrentNumber = 0;
			Phase ++;
			Sign = 1;
		}
	}
	return Result;
}

int IicSendCommand(u8* message, int messageSize)
{
	while (XIicPs_BusIsBusy(&Iic)) {
		/* NOP */
	}

	int Status, i;
	Status = XIicPs_MasterSendPolled(&Iic, message, messageSize, IIC_SLAVE_ADDR);
	if (Status != XST_SUCCESS)
	{
		// failed at this point..
		// do something about it
		return -1;
	}

	return 0;
}

robot_state IicGetInvalidRobotState()
{
	robot_state InvalidRobotState;

	InvalidRobotState.numberOfCommands = -1;
	InvalidRobotState.vacuumStatus = -1;
	InvalidRobotState.sensorsServoStatus = -1;
	InvalidRobotState.position.x = -1;
	InvalidRobotState.position.y = -1;
	InvalidRobotState.headingAngle = -1;
	InvalidRobotState.leftSensor.x = -1;
	InvalidRobotState.leftSensor.y = -1;
	InvalidRobotState.frontSensor.x = -1;
	InvalidRobotState.frontSensor.y = -1;
	InvalidRobotState.rightSensor.x = -1;
	InvalidRobotState.rightSensor.y = -1;

	return InvalidRobotState;
}

void IicPrintRobotState(robot_state RobotState)
{
	xil_printf("Robot state: \n");
	xil_printf("\t Number of commands: %d\n", RobotState.numberOfCommands);
	xil_printf("\t Vacuum status: %d\n", RobotState.vacuumStatus);
	xil_printf("\t Sensors servo status: %d\n", RobotState.sensorsServoStatus);
	xil_printf("\tPosition <x, y>: <%d, %d>\n", RobotState.position.x, RobotState.position.y);
	xil_printf("\tHeading angle: %d\n", RobotState.headingAngle);
	xil_printf("\tLeft sensor <x, y>: <%d, %d>\n", RobotState.leftSensor.x, RobotState.leftSensor.y);
	xil_printf("\tFront sensor <x, y>: <%d, %d>\n", RobotState.frontSensor.x, RobotState.frontSensor.y);
	xil_printf("\tRight sensor <x, y>: <%d, %d>\n", RobotState.rightSensor.x, RobotState.rightSensor.y);
	xil_printf("\r\n");
}
