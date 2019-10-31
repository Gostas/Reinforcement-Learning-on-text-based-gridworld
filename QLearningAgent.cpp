#include "pch.h"
#include "QLearningAgent.h"

//the Q-Learning equation
void QLearningAgent::updateQTable(int state, int newState, Moves action, double reward)
{
	qTable[state][action - 1] += alpha * (reward + gamma * getBestActionQVal(newState) - qTable[state][action - 1]);
}

QLearningAgent::~QLearningAgent()
{
	for (int i = 0; i < gridSize*gridSize; ++i)
		delete[] qTable[i];

	delete[] qTable;
}