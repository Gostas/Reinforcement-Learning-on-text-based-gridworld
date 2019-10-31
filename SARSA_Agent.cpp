#include "pch.h"
#include "SARSA_Agent.h"
//#include <cstdio>

//the SARSA equation
void SARSA_Agent::updateQTable(int state, int newState, Moves action, Moves nextAction, double reward)
{
	//FILE* pFile;
	//fopen_s(&pFile, "debugging.txt", "a");
	//fprintf(pFile, "State: %d, new state: %d, action: %d, nextAction: %d, reward: %.2lf\n", state, newState, action, nextAction, reward);
	qTable[state][action - 1] += alpha * (reward + gamma*qTable[newState][nextAction - 1] - qTable[state][action - 1]);

	/*fprintf(pFile, "State %d, q val: %.2lf\n", state, qTable[state][action - 1]);
	fflush(pFile);
	fclose(pFile);*/
}

SARSA_Agent::~SARSA_Agent()
{
	for (int i = 0; i < gridSize*gridSize; ++i)
		delete[] qTable[i];

	delete qTable;
}