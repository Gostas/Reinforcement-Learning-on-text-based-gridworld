#include "pch.h"
#include "Agent.h"
#include "movesEnum.h"

Agent::Agent(double alpha, double gamma, int initPos, int gridSize)
{
	COORD start;

	moves_order[0] = 1;
	moves_order[1] = 2;
	moves_order[2] = 3;
	moves_order[3] = 4;

	start.X = initPos % gridSize;
	start.Y = initPos / gridSize;

	this->alpha = alpha;
	this->gamma = gamma;

	position = start;
	startPos = start;

	this->gridSize = gridSize;

	qTable = new double*[gridSize*gridSize];

	prevFirstMoveBestqVal = 0;
	prevSecondMoveBestqVal = 0;

	prevLastStateqVal = 0;
	prevPenultimateStateqVal = 0;

	for (int i = 0; i < gridSize*gridSize; ++i)
	{
		qTable[i] = new double[4];
		for (int j = 0; j < 4; ++j)
			qTable[i][j] = 0;
	}
}

Moves Agent::pickAction(int state, double epsilon)
{
	double testVal = 0;
	double maxVal=-1e9;
	Moves action = Moves(0);
	int intAction = 0;

	testVal = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);

	if (testVal < epsilon)
		action = Moves(1 + rand() % 4);	//pick random action if value is less than epsilon

	else //otherwise choose a move depending on the value of the qTable
	{
		for (int i = 0; i < 4; ++i)
		{
			if (qTable[state][moves_order[i]-1] > maxVal)
			{
				maxVal = qTable[state][moves_order[i]-1];
				intAction = moves_order[i];
			}
		}
		action = Moves(intAction);
		std::next_permutation(moves_order, moves_order + 4); //consider the moves in different order every time
	}
	return action;
}

void Agent::updatePos(int newState)
{
	position.X = newState % gridSize;
	position.Y = newState / gridSize;
}

double Agent::getBestActionQVal(int state)
{
	double maxVal = -1e9;
	for (int i = 0; i < 4; ++i)
	{
		if (position.X == 0 && i == 2)		//only look at the actions that will result in a position inside the grid
			continue;
		else if (position.Y == 0 && i == 0)
			continue;
		else if (position.X == gridSize - 1 && i == 3)
			continue;
		else if (position.Y == gridSize - 1 && i == 1)
			continue;
		if (qTable[state][i] > maxVal)
			maxVal = qTable[state][i];
	}

	return maxVal;
}

void Agent::printQTable()
{
	for (int i = 0; i < gridSize; ++i)
	{
		for (int j = 0; j < gridSize; ++j)
			printf("%.2lf  ", getBestActionQVal(i*gridSize + j));

		printf("\n");
	}
}

void Agent::updateQValueOutput(int state, COORD startingPoint)
{
	COORD dest;
	HANDLE hStdout;
	COORD stateCoord;

	stateCoord.X = state % gridSize;
	stateCoord.Y = state / gridSize;

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	dest.X = 6 * stateCoord.X + startingPoint.X;	//account for the spaces between the characters in the grid
	dest.Y = stateCoord.Y + startingPoint.Y;

	SetConsoleCursorPosition(hStdout, dest);	//move the cursor to the correct location

	printf("%.2lf", getBestActionQVal(state));
}

bool Agent::checkForConvergence(int lastState, int penultimateState)
{
	double currFirstMoveBestqVal,
		currSecondMoveBestqVal;

	double lastStateqVal,
		penultimateStateqVal;

	bool converged;
	Moves action;
	COORD pos;

	pos = startPos;

	currFirstMoveBestqVal = getBestActionQVal(startPos.X + gridSize * startPos.Y);

	action = pickAction(startPos.X + gridSize * startPos.Y, 0.f);

	switch (action)	//use it to find the action that will lead to the 2nd state
	{
	case up:
		if (pos.Y - 1 < 0)
			break;
		pos.Y -= 1;
		break;

	case down:
		if (pos.Y + 1 > gridSize - 1)
			break;
		pos.Y += 1;
		break;

	case left:
		if (pos.X - 1 < 0)
			break;
		pos.X -= 1;
		break;

	case right:
		if (pos.X + 1 > gridSize - 1)
			break;
		pos.X += 1;
		break;

	default:
		break;
	}

	currSecondMoveBestqVal = getBestActionQVal(pos.X + gridSize * pos.Y);

	penultimateStateqVal = getBestActionQVal(penultimateState);
	lastStateqVal = getBestActionQVal(lastState);

	if (abs(currFirstMoveBestqVal - prevFirstMoveBestqVal) < 0.01 && abs(currSecondMoveBestqVal - prevSecondMoveBestqVal) < 0.01 && abs(lastStateqVal - prevLastStateqVal) < 0.01 && abs(penultimateStateqVal - prevPenultimateStateqVal) < 0.01)
		converged = true;
	else
		converged = false;

	prevFirstMoveBestqVal = currFirstMoveBestqVal;
	prevSecondMoveBestqVal = currSecondMoveBestqVal;

	prevLastStateqVal = lastStateqVal;
	prevPenultimateStateqVal = penultimateStateqVal;

	return converged;
}