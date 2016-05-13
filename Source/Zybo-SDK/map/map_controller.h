/*
 * map_controller.h
 *
 *  Created on: May 6, 2016
 *      Author: Alex
 */

#ifndef MAP_CONTROLLER_H_
#define MAP_CONTROLLER_H_

#include "../util/util.h"

// The map is a 2 by 2 matrix of BLOCKS_COUNT by BLOCKS_COUNT
// squares and the length of each square is BLOCK_LENGTH.
// Thus, the size of the map in reality is BLOCKS_COUNT * BLOCK_LENGTH (CM) by
// the same size and the origin is represented in the middle of the map

// Number of blocks the map has
#define BLOCKS_COUNT 40
// Length of a block
#define BLOCK_LENGTH 5
#define DOUBLE_BLOCK_LENGTH BLOCK_LENGTH * 2
// Used for computing the Manhattan distance between two points
#define OFFSET (BLOCKS_COUNT / 2) * BLOCK_LENGTH

#define NUMBER_OF_NODES BLOCKS_COUNT * BLOCKS_COUNT

// Number of points that a sensor needs to 'see' inside a block so that
// it considers the block an obstacle
#define THRESHOLD 10

typedef struct
{
	int id;
	int isObstacle;
	point position;
	int leftEyePoints, frontEyePoints, rightEyePoints;
}block;

block blockMap[BLOCKS_COUNT][BLOCKS_COUNT];

void MapControllerInitializeMap();
void MapControllerPrintMap();
void MapControllerConsumeCurrentRobotState(robot_state CurrentRobotState);
int MapControllerGetNextMove(point from, point to, int heading);
point MapControllerIdToPoint(int id);
point MapControllerBlockAt(int x, int y);

#endif /* MAP_CONTROLLER_H_ */
