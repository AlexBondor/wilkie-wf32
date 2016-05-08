/*
 * map_controller.c
 *
 *  Created on: May 6, 2016
 *      Author: Alex
 */

#include "stdlib.h"
#include "xil_printf.h"
#include "map_controller.h"
#include "../util/consts.h"

// The map is a 2 by 2 matrix of BLOCKS_COUNT by BLOCKS_COUNT
// squares and the length of each square is BLOCK_LENGTH.
// Thus, the size of the map in reality is BLOCKS_COUNT * BLOCK_LENGTH (CM) by
// the same size and the origin is represented in the middle of the map

// Number of blocks the map has
#define BLOCKS_COUNT 20
// Length of a block
#define BLOCK_LENGTH 5
// Number of points that a sensor needs to 'see' inside a block so that
// it considers the block an obstacle
#define THRESHOLD 10

typedef struct
{
	point position;
	int leftEyePoints, frontEyePoints, rightEyePoints;
}block;

block blockMap[BLOCKS_COUNT][BLOCKS_COUNT];

// 'private' function prototypes
point MapControllerBlockAt(int x, int y);
point MapControllerTransformPoint(int originX, int originY, int pointX, int pointY, int heading);

void MapControllerInitializeMap()
{
	int i, j;

	int x, y;
	x = (BLOCKS_COUNT / 2) * -BLOCK_LENGTH;
	y = (BLOCKS_COUNT / 2) * BLOCK_LENGTH;

	for (i = 0; i < BLOCKS_COUNT; i++)
	{
		for (j = 0; j < BLOCKS_COUNT; j++)
		{
			blockMap[i][j].position.x = x;
			blockMap[i][j].position.y = y;
			blockMap[i][j].leftEyePoints = 0;
			blockMap[i][j].frontEyePoints = 0;
			blockMap[i][j].rightEyePoints = 0;
			x += BLOCK_LENGTH;
		}
		x = (BLOCKS_COUNT / 2) * -BLOCK_LENGTH;
		y -= BLOCK_LENGTH;
	}

	for (i = 0; i < BLOCKS_COUNT; i++)
	{
		for (j = 0; j < BLOCKS_COUNT; j++)
		{
			xil_printf("<%d %d> ", blockMap[i][j].position.x, blockMap[i][j].position.y);
		}
		xil_printf("\r\n");
	}
}

void MapControllerPrintMap()
{
	int i, j;

	for (i = 0; i < BLOCKS_COUNT; i++)
	{
		xil_printf("\r\n");
	}

	for (i = 0; i < BLOCKS_COUNT; i++)
	{
		for (j = 0; j < BLOCKS_COUNT; j++)
		{
			int points = blockMap[i][j].leftEyePoints + blockMap[i][j].frontEyePoints + blockMap[i][j].rightEyePoints;
			xil_printf("%4d", points);
		}
		xil_printf("\n");
	}
}

// Updates the blockMap given the current state of the robot
// If the sensors of the robot sensed something then the map
// will be updated accordingly
void MapControllerConsumeCurrentRobotState(robot_state CurrentRobotState)
{
//	point transformedPoint = MapControllerTransformPoint(
//					CurrentRobotState.position.x,
//					CurrentRobotState.position.y,
//					CurrentRobotState.leftSensor.x,)
}

// Returns the coordinates of the block in blockMap matrix
// given the coordinates of the point in real space
point MapControllerBlockAt(int x, int y)
{
	int min = (BLOCKS_COUNT / 2) * -BLOCK_LENGTH;
	int max = (BLOCKS_COUNT / 2) * BLOCK_LENGTH;

	point result;

	x = x < min ? -1 : x > max ? -1 : x;
	y = y < min ? -1 : y > max ? -1 : y;

	if (x == -1 || y == -1)
	{
		result.x = -1;
		result.y = -1;

		return result;
	}

	result.x = x - ((x % BLOCK_LENGTH) + BLOCK_LENGTH) % BLOCK_LENGTH;
	result.y = y - ((y % BLOCK_LENGTH) - BLOCK_LENGTH) % BLOCK_LENGTH;

	x = result.x;
	y = result.y;

	result.x = BLOCKS_COUNT / 2 - y / BLOCK_LENGTH;
	result.y = x / BLOCK_LENGTH + BLOCKS_COUNT / 2;

	return result;
}

// Translates a point with the origin taking into account the heading
point MapControllerTransformPoint(int originX, int originY, int pointX, int pointY, int heading)
{
	point result;

	result.x = (int)((float)pointX * COS[heading-90] - (float)pointY * SIN[heading - 90] + originX);
	result.y = (int)((float)pointY * COS[heading-90] + (float)pointX * SIN[heading - 90] + originY);

	return result;
}
