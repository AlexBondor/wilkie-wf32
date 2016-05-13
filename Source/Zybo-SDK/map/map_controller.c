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

int fCosts[NUMBER_OF_NODES];
int gCosts[NUMBER_OF_NODES];
int searchFrontier[NUMBER_OF_NODES];
int shortestPath[NUMBER_OF_NODES];
int path[NUMBER_OF_NODES];
int aStarMap[BLOCKS_COUNT][BLOCKS_COUNT];

typedef struct
{
	int id;
	int cost;
} priorityQueueElement;

int priorityQueueSize = 0;
priorityQueueElement priorityQueue[NUMBER_OF_NODES];

/*
 * Function prototypes
 */
point MapControllerTransformPoint(int originX, int originY, int pointX, int pointY, int heading);
int MapControllerIsTooClose(int originX, int originY, int pointX, int pointY);

void PriorityQueuePrintQueue();
void PriorityQueueAdd(priorityQueueElement element);
priorityQueueElement PriorityQueueRemove();
void PriorityQueueChangePriority(int neigborId, int gCost);
void PriorityQueueResort();

point* AStarGetNeighborBlocks(int blockId, int* size);
int AStarSearch(point from, point to);
void AStarInitializeStructures();
int AStarBuildFinalPath(int sourceId);

/**
 * Does the initialization of the map by computing for each block its position
 */
void MapControllerInitializeMap()
{
	int i, j, id = 0;

	int x, y;
	x = (BLOCKS_COUNT / 2) * -BLOCK_LENGTH;
	y = (BLOCKS_COUNT / 2) * BLOCK_LENGTH;

	for (i = 0; i < BLOCKS_COUNT; i++)
	{
		for (j = 0; j < BLOCKS_COUNT; j++)
		{
			blockMap[i][j].id = id++;
			blockMap[i][j].isObstacle = 0;
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
			if (blockMap[i][j].isObstacle == 0)
			{
				xil_printf("%4d", 0);
			}
			else
			{
				int points = blockMap[i][j].leftEyePoints + blockMap[i][j].frontEyePoints + blockMap[i][j].rightEyePoints;
				xil_printf("%4d", points);
			}
		}
		xil_printf("\n");
	}
}

/**
 * Checks is the count of hits is more than the threshold for a
 * specific block
 */
void MapControllerCheckForObstacle(point myPoint)
{
	if (blockMap[myPoint.x][myPoint.y].leftEyePoints
		+ blockMap[myPoint.x][myPoint.y].frontEyePoints
		+ blockMap[myPoint.x][myPoint.y].rightEyePoints > THRESHOLD)
	{
		blockMap[myPoint.x][myPoint.y].isObstacle = 1;
	}
}

/**
 * Updates the blockMap given the current state of the robot
 * If the sensors of the robot sensed something then the map
 * will be updated accordingly
 */
void MapControllerConsumeCurrentRobotState(robot_state CurrentRobotState)
{
	int lX = CurrentRobotState.leftSensor.x;
	int lY = CurrentRobotState.leftSensor.y;
	int fX = CurrentRobotState.frontSensor.x;
	int fY = CurrentRobotState.frontSensor.y;
	int rX = CurrentRobotState.rightSensor.x;
	int rY = CurrentRobotState.rightSensor.y;
	int pX = CurrentRobotState.position.x;
	int pY = CurrentRobotState.position.y;
	int head = CurrentRobotState.headingAngle;

	point transformedPoint;

	transformedPoint = MapControllerTransformPoint(pX, pY, lX, lY, head);
	if (MapControllerIsTooClose(pX, pY, transformedPoint.x, transformedPoint.y) == 0)
	{
		point blockAt = MapControllerBlockAt(transformedPoint.x, transformedPoint.y);
		blockMap[blockAt.x][blockAt.y].leftEyePoints ++;
		MapControllerCheckForObstacle(blockAt);
	}

	transformedPoint = MapControllerTransformPoint(pX, pY,fX, fY, head);
	if (MapControllerIsTooClose(pX, pY, transformedPoint.x, transformedPoint.y) == 0)
	{
		point blockAt = MapControllerBlockAt(transformedPoint.x, transformedPoint.y);
		blockMap[blockAt.x][blockAt.y].frontEyePoints ++;
		MapControllerCheckForObstacle(blockAt);
	}

	transformedPoint = MapControllerTransformPoint(pX, pY, rX, rY, head);
	if (MapControllerIsTooClose(pX, pY, transformedPoint.x, transformedPoint.y) == 0)
	{
		point blockAt = MapControllerBlockAt(transformedPoint.x, transformedPoint.y);
		blockMap[blockAt.x][blockAt.y].rightEyePoints ++;
		MapControllerCheckForObstacle(blockAt);
	}
}

/**
 * Returns 1 if point coordinates are too close to origin coordinates
 * The bounding box is represented by a square with edge size DOUBLE_BLOCK_LENGTH
 */
int MapControllerIsTooClose(int originX, int originY, int pointX, int pointY)
{
	if (originX - DOUBLE_BLOCK_LENGTH < pointX
		&& originX + DOUBLE_BLOCK_LENGTH > pointX
		&& originY - DOUBLE_BLOCK_LENGTH < pointY
		&& originY + DOUBLE_BLOCK_LENGTH > pointY)
	{
		return 1;
	}
	return 0;
}

/**
 * Returns the coordinates of the block in blockMap matrix
 * given the coordinates of the point in real space
 */
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

/**
 * Translates a point with the origin taking into account the heading
 */
point MapControllerTransformPoint(int originX, int originY, int pointX, int pointY, int heading)
{
	point result;

	result.x = (int)((float)pointX * COS[heading-90] - (float)pointY * SIN[heading - 90] + originX);
	result.y = (int)((float)pointY * COS[heading-90] + (float)pointX * SIN[heading - 90] + originY);

	return result;
}

/**
 * Returns the Manhattan distance between two points
 */
int MapControllerDistanceFromTo(point a, point b)
{
	int xOffset, yOffset;

	if (b.x > a.x)
	{
		xOffset = b.x - a.x;
	}
	else
	{
		xOffset = a.x - b.x;
	}
	if (b.y > a.y)
	{
		yOffset = b.y - a.y;
	}
	else
	{
		yOffset = a.y - b.y;
	}
	return xOffset + yOffset;
}

int MapControllerPointToId(point myPoint)
{
	point blockAt = MapControllerBlockAt(myPoint.x, myPoint.y);

	return blockMap[blockAt.x][blockAt.y].id;
}

point MapControllerIdToPoint(int id)
{
	point result;

	result.x = id / BLOCKS_COUNT;
	result.y = id % BLOCKS_COUNT;

	return result;
}

/**
 * Add an element inside the global priority queue
 * At any moment the queue is sorted non-decreasing taking
 * into account the costs of each element
 */
void PriorityQueueAdd(priorityQueueElement element)
{
	int i, j;
	int elementCost = element.cost;
	for (i = 0; i < priorityQueueSize; i++)
	{
		if (priorityQueue[i].cost > elementCost)
		{
			break;
		}
	}
	if (i == priorityQueueSize)
	{
		priorityQueue[i] = element;
		priorityQueueSize ++;
		return;
	}
	for (j = priorityQueueSize; j > i; j--)
	{
		priorityQueue[j] = priorityQueue[j - 1];
	}
	priorityQueueSize ++;
	priorityQueue[i] = element;
}

/**
 * Returns the first element of the global priority queue in case
 * it is not empty yet
 */
priorityQueueElement PriorityQueueRemove()
{
	priorityQueueElement result;
	result.id = -1;
	result.cost = -1;
	if (priorityQueueSize != 0)
	{
		int i;
		result = priorityQueue[0];
		for (i = 0; i < priorityQueueSize; i++)
		{
			priorityQueue[i] = priorityQueue[i + 1];
		}
		priorityQueueSize --;
	}
	return result;
}

void PriorityQueueChangePriority(int neigborId, int gCost)
{
	int i;
	for (i = 0; i < priorityQueueSize; i++)
	{
		if (priorityQueue[i].id == neigborId)
		{
			priorityQueue[i].cost = gCost;
			break;
		}
	}
	PriorityQueueResort();
}

void PriorityQueueResort()
{
	int temp, i;
	priorityQueueElement swap;
	for (i = 1 ; i < priorityQueueSize; i++)
	{
		temp = i;

		while (temp > 0 && priorityQueue[temp].cost < priorityQueue[temp - 1].cost)
		{
			swap             = priorityQueue[temp];
			priorityQueue[temp]   = priorityQueue[temp - 1];
			priorityQueue[temp - 1] = swap;

			temp--;
		}
	}
}

void PriorityQueuePrintQueue()
{
	int i;
	for (i = 0; i <priorityQueueSize; i++)
	{
		xil_printf("<%d,%d> ", priorityQueue[i].id, priorityQueue[i].cost);
	}
	xil_printf("\n");
}

/**
 * Returns a list of neighbor nodes given the id of the current block
 * A neighbor will be included in the list only if it is not an obstacle
 * and if none of the next 3 neighbors of it are an obstacle
 * ex. of not a neighbor:
 *
 * 0 X 0 0 0
 * 0 0 1 0 0
 * X 4 C 2 0
 * 0 0 3 0 0
 * 0 X X X 0
 *
 * In the above example "X" are denoted the obstacles and the only possible
 * neighbor to "C" is the node denoted with "2"
 */
point* AStarGetNeighborBlocks(int blockId, int* size)
{
	int temp = 0, flag;
	point* result = (point*)malloc(4 * sizeof(point));
	point myBlock = MapControllerIdToPoint(blockId);

	// Check for left neighbor
	flag = 1;
	if (myBlock.y - 1 >= 0 && blockMap[myBlock.x][myBlock.y - 1].isObstacle == 0)
	{
		if (myBlock.y - 2 >= 0)
		{
			if (blockMap[myBlock.x][myBlock.y - 2].isObstacle == 1)
			{
				flag = 0;
			}
			else
			{
				if(myBlock.x - 1 >= 0 && blockMap[myBlock.x - 1][myBlock.y - 2].isObstacle == 1)
				{
					flag = 0;
				}
				else
				{
					if(myBlock.x + 1 >= 0 && blockMap[myBlock.x + 1][myBlock.y - 2].isObstacle == 1)
					{
						flag = 0;
					}
				}
			}
		}
		if (flag == 1)
		{
			result[temp].x = myBlock.x;
			result[temp++].y = myBlock.y - 1;
		}
	}
	// Check for top neighbor
	flag = 1;
	if (myBlock.x - 1 >= 0 && blockMap[myBlock.x - 1][myBlock.y].isObstacle == 0)
	{
		if (myBlock.x - 2 >= 0)
		{
			if (blockMap[myBlock.x - 2][myBlock.y].isObstacle == 1)
			{
				flag = 0;
			}
			else
			{
				if(myBlock.y - 1 >= 0 && blockMap[myBlock.x - 2][myBlock.y - 1].isObstacle == 1)
				{
					flag = 0;
				}
				else
				{
					if(myBlock.y + 1 >= 0 && blockMap[myBlock.x - 2][myBlock.y + 1].isObstacle == 1)
					{
						flag = 0;
					}
				}
			}
		}
		if (flag == 1)
		{
			result[temp].x = myBlock.x - 1;
			result[temp++].y = myBlock.y;
		}
	}
	// Check for right neighbor
	flag = 1;
	if (myBlock.y + 1 < BLOCKS_COUNT && blockMap[myBlock.x][myBlock.y + 1].isObstacle == 0)
	{
		if (myBlock.y + 2 >= 0)
		{
			if (blockMap[myBlock.x][myBlock.y + 2].isObstacle == 1)
			{
				flag = 0;
			}
			else
			{
				if(myBlock.x - 1 >= 0 && blockMap[myBlock.x - 1][myBlock.y + 2].isObstacle == 1)
				{
					flag = 0;
				}
				else
				{
					if(myBlock.x + 1 >= 0 && blockMap[myBlock.x + 1][myBlock.y + 2].isObstacle == 1)
					{
						flag = 0;
					}
				}
			}
		}
		if (flag == 1)
		{
			result[temp].x = myBlock.x;
			result[temp++].y = myBlock.y + 1;
		}
	}
	// Check for bottom neighbor
	flag = 1;
	if (myBlock.x + 1 < BLOCKS_COUNT && blockMap[myBlock.x + 1][myBlock.y].isObstacle == 0)
	{
		if (myBlock.x + 2 >= 0)
		{
			if (blockMap[myBlock.x + 2][myBlock.y].isObstacle == 1)
			{
				flag = 0;
			}
			else
			{
				if(myBlock.y - 1 >= 0 && blockMap[myBlock.x + 2][myBlock.y - 1].isObstacle == 1)
				{
					flag = 0;
				}
				else
				{
					if(myBlock.y + 1 >= 0 && blockMap[myBlock.x + 2][myBlock.y + 1].isObstacle == 1)
					{
						flag = 0;
					}
				}
			}
		}
		if (flag == 1)
		{
			result[temp].x = myBlock.x + 1;
			result[temp++].y = myBlock.y;
		}
	}
	*size = temp;
	return result;
}

void AStarInitializeStructures()
{
	int i;
	for (i = 0; i < NUMBER_OF_NODES; i++)
	{
		fCosts[i] = 0;
		gCosts[i] = 0;
		searchFrontier[i] = -1;
		shortestPath[i] = -1;
		path[i] = -1;
	}
	while (priorityQueueSize != 0)
	{
		PriorityQueueRemove();
	}
}

/**
 * Searches a path between point "from" and point "to" in the blockMap.
 * If a path is found then 1 is returned and the shortestPath vector
 * contains data about the path. The algorithm used for finding the path
 * is A*. If there is no path between the 2 points then 0 is returned.
 */
int AStarSearch(point from, point to)
{
	int targetId = MapControllerPointToId(to);
	point targetBlock = MapControllerIdToPoint(targetId);
	int i, hCost, gCost;

	AStarInitializeStructures();

	priorityQueueElement myPriorityQueueElement, temp;

	// Initialize source node
	myPriorityQueueElement.id = MapControllerPointToId(from);
	myPriorityQueueElement.cost = 0;

	int lastNodeId = myPriorityQueueElement.id;

	PriorityQueueAdd(myPriorityQueueElement);

	searchFrontier[myPriorityQueueElement.id] = myPriorityQueueElement.id;

	while (priorityQueueSize > 0)
	{
		priorityQueueElement nextClosestNode = PriorityQueueRemove();

		shortestPath[lastNodeId] = nextClosestNode.id;
		lastNodeId = nextClosestNode.id;

		if (nextClosestNode.id == targetId)
		{
			shortestPath[nextClosestNode.id] = targetId;
			return AStarBuildFinalPath(MapControllerPointToId(from));
		}

		int noOfNeighbors;
		point* neighbors = AStarGetNeighborBlocks(nextClosestNode.id, &noOfNeighbors);

		// Test all nodes neighbor to this one
		for (i = 0; i < noOfNeighbors; i++)
		{
			int neighborId = blockMap[neighbors[i].x][neighbors[i].y].id;

			// Calculate the heuristic cost from this node to target
			hCost = MapControllerDistanceFromTo(neighbors[i], targetBlock);

			// Calculate the "real" cost to this node from source
			gCost = gCosts[nextClosestNode.id] + 1; // 1 is the cost for all edges

			temp.id = neighborId;
			temp.cost = hCost;

			// If the node has not been added to the frontier, add it and update
			// the G and F costs
			if (searchFrontier[neighborId] == -1)
			{
				fCosts[neighborId] = gCost + hCost;
				gCosts[neighborId] = gCost;

				PriorityQueueAdd(temp);

				searchFrontier[neighborId] = neighborId;
			}
			else
			{
				if (gCost < gCosts[neighborId] && shortestPath[neighborId] == -1)
				{
					fCosts[neighborId] = gCost + hCost;
					gCosts[neighborId] = gCost;

					PriorityQueueChangePriority(neighborId, gCost);

					searchFrontier[neighborId] = neighborId;
				}
			}
		}
	}
	return 0;
}

/**
 * Builds the final path based on the A* algorithm result
 */
int AStarBuildFinalPath(int sourceId)
{
	int i, j, id = 0;
	for (i = 0; i < BLOCKS_COUNT; i++)
	{
		for (j = 0; j < BLOCKS_COUNT; j++)
		{
			if (shortestPath[id++] != -1)
			{
				aStarMap[i][j] = 1;
			}
			else
			{
				aStarMap[i][j] = 0;
			}
		}
	}

	int currentId = sourceId;
	int nextId = sourceId;
	int lastDirection = 0;

	while (shortestPath[currentId] != currentId)
	{
		point currentPoint = MapControllerIdToPoint(currentId);
		point nextPoint;

		int stepPerformed = 0;
		while (stepPerformed == 0)
		{
			switch(lastDirection)
			{
				case 0:
					nextPoint.x = currentPoint.x + 1;
					nextPoint.y = currentPoint.y;
					nextId = blockMap[nextPoint.x][nextPoint.y].id;
					if (aStarMap[nextPoint.x][nextPoint.y] != 0 && path[nextId] == -1)
					{
						path[currentId] = nextId;
						currentId = nextId;
						lastDirection = 0;
						stepPerformed = 1;
					}
					break;
				case 1:
					nextPoint.x = currentPoint.x;
					nextPoint.y = currentPoint.y + 1;
					nextId = blockMap[nextPoint.x][nextPoint.y].id;
					if (aStarMap[nextPoint.x][nextPoint.y] != 0 && path[nextId] == -1)
					{
						path[currentId] = nextId;
						currentId = nextId;
						lastDirection = 1;
						stepPerformed = 1;
					}
					break;
				case 2:
					nextPoint.x = currentPoint.x - 1;
					nextPoint.y = currentPoint.y;
					nextId = blockMap[nextPoint.x][nextPoint.y].id;
					if (aStarMap[nextPoint.x][nextPoint.y] != 0 && path[nextId] == -1)
					{
						path[currentId] = nextId;
						currentId = nextId;
						lastDirection = 2;
						stepPerformed = 1;
					}
					break;
				case 3:
					nextPoint.x = currentPoint.x;
					nextPoint.y = currentPoint.y - 1;
					nextId = blockMap[nextPoint.x][nextPoint.y].id;
					if (aStarMap[nextPoint.x][nextPoint.y] != 0 && path[nextId] == -1)
					{
						path[currentId] = nextId;
						currentId = nextId;
						lastDirection = 3;
						stepPerformed = 1;
					}
					break;
				default:
					lastDirection = 0;
					break;
			}
			if (stepPerformed == 0)
			{
				lastDirection += 1;
				lastDirection %= 4;
			}
		}
		path[currentId] = currentId;
	}
	return 1;
}

#define HEADING_TOLERANCE 10
#define HEADING_UP 90
#define HEADING_DOWN 270
#define HEADING_LEFT 180
#define HEADING_RIGHT 360

int IsHeadingUp(int heading)
{
	if (heading > HEADING_UP - HEADING_TOLERANCE && heading < HEADING_UP + HEADING_TOLERANCE)
	{
		return 1;
	}
	return 0;
}

int IsHeadingDown(int heading)
{
	if (heading > HEADING_DOWN - HEADING_TOLERANCE && heading < HEADING_DOWN + HEADING_TOLERANCE)
	{
		return 1;
	}
	return 0;
}

int IsHeadingLeft(int heading)
{
	if (heading > HEADING_LEFT - HEADING_TOLERANCE && heading < HEADING_LEFT + HEADING_TOLERANCE)
	{
		return 1;
	}
	return 0;
}

int IsHeadingRight(int heading)
{
	if (heading > HEADING_RIGHT - HEADING_TOLERANCE || heading < HEADING_TOLERANCE)
	{
		return 1;
	}
	return 0;
}

/**
 * Return codes:
 * 0 - move forward
 * 1 - turn left once
 * 2 - turn right once
 * 3 - turn 180 degree
 */
int MapControllerGetNextMove(point from, point to, int heading)
{
	point fromBlockPoint = MapControllerBlockAt(from.x, from.y);
	int fromId = blockMap[fromBlockPoint.x][fromBlockPoint.y].id;
	int nextStepId;
	point nextStepPoint;
	int i, j, index = 0;
	if (AStarSearch(from, to) == 1)
	{
		for (i = 0; i < BLOCKS_COUNT; i++)
		{
			for (j = 0; j < BLOCKS_COUNT; j++)
			{
				xil_printf("%4d ", path[index++]);
			}
			xil_printf("\r\n");
		}
		nextStepId = path[fromId];
		nextStepPoint = MapControllerIdToPoint(nextStepId);
		if(fromBlockPoint.x == nextStepPoint.x)
		{
			// to left
			if (fromBlockPoint.y - 1 == nextStepPoint.y)
			{
				if (IsHeadingLeft(heading) == 1)
				{
					return 0;
				}
				else
				{
					if (IsHeadingUp(heading) == 1)
					{
						return 1;
					}
					else
					{
						if (IsHeadingDown(heading) == 1)
						{
							return 2;
						}
						else
						{
							return 3;
						}
					}
				}
			}
			else
			{
				// to right
				if (fromBlockPoint.y + 1 == nextStepPoint.y)
				{
					if (IsHeadingRight(heading) == 1)
					{
						return 0;
					}
					else
					{
						if (IsHeadingUp(heading) == 1)
						{
							return 2;
						}
						else
						{
							if (IsHeadingDown(heading) == 1)
							{
								return 1;
							}
							else
							{
								return 3;
							}
						}
					}
				}
			}
		}
		else
		{
			if(fromBlockPoint.y == nextStepPoint.y)
			{
				// up
				if (fromBlockPoint.x - 1 == nextStepPoint.x)
				{
					if (IsHeadingUp(heading) == 1)
					{
						return 0;
					}
					else
					{
						if (IsHeadingLeft(heading) == 1)
						{
							return 2;
						}
						else
						{
							if (IsHeadingRight(heading) == 1)
							{
								return 1;
							}
							else
							{
								return 3;
							}
						}
					}
				}
				else
				{
					// down
					if (fromBlockPoint.x + 1 == nextStepPoint.x)
					{
						if (IsHeadingDown(heading) == 1)
						{
							return 0;
						}
						else
						{
							if (IsHeadingLeft(heading) == 1)
							{
								return 1;
							}
							else
							{
								if (IsHeadingRight(heading) == 1)
								{
									return 2;
								}
								else
								{
									return 3;
								}
							}
						}
					}
				}
			}
		}
	}
	return -1;
}
