#pragma once
#include "movesEnum.h"
#include "pch.h"

class Agent
{
private:
	double prevFirstMoveBestqVal;
	double prevSecondMoveBestqVal;
	double prevLastStateqVal;
	double prevPenultimateStateqVal;

protected:
	COORD position;
	COORD startPos;
	int gridSize;
	double alpha, gamma;
	double **qTable;

	//stores the order that the moves are considered
	int moves_order[4];

	//returns a random action
	inline Moves getRandomAction() { return Moves(rand() * 4 + 1); }
	
	//returns the maximum q-value from a given state from the q-table
	double getBestActionQVal(int state);

public:
	//don't allow the use of the default constructor-always use overloaded constructor
	Agent() = delete;

	//overloaded contructor
	Agent(double alpha, double gamma, int initPos, int gridSize);

	//returns the largest q-value amongst all the actions in a specific state
	void printQTable();

	//move the cursor to specific locations to update the display of maximum q values after the agent has moved
	void updateQValueOutput(int state, COORD startingPoint);

	//update the agent's position
	void updatePos(int newState);

	//check if the algorithm has converged
	bool checkForConvergence(int lastState, int penultimateState);

	//return the state from the agent's position
	inline int getState() { return gridSize * position.Y + position.X; }

	//move the agent to the starting position
	inline void goToStart() { position = startPos; }

	//return an action based on the epsilon-greedy policy
	Moves pickAction(int state, double epsilonG);
};