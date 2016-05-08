/*
 * map_controller.h
 *
 *  Created on: May 6, 2016
 *      Author: Alex
 */

#ifndef MAP_CONTROLLER_H_
#define MAP_CONTROLLER_H_

#include "../util/util.h"

void MapControllerInitializeMap();
void MapControllerPrintMap();
void MapControllerConsumeCurrentRobotState(robot_state CurrentRobotState);

#endif /* MAP_CONTROLLER_H_ */
