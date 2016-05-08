/*
 * iic_slave_polling.h
 *
 *  Created on: May 2, 2016
 *      Author: Alex
 */

#ifndef IIC_SLAVE_POLLING_H_
#define IIC_SLAVE_POLLING_H_

int IicInit();
robot_state IicGetRobotState();
robot_state IicGetInvalidRobotState();
void IicPrintRobotState(robot_state RobotState);
int IicSendCommand(u8* message, int messageSize);

#endif /* IIC_SLAVE_POLLING_H_ */
