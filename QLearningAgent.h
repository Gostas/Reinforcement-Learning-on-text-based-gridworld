#pragma once
#include "Agent.h"

//inherits class Agents
class QLearningAgent :
	public Agent
{
public:
	//use constructor from base class
	using Agent::Agent;

	//destructor-free up memory
	~QLearningAgent();

	void updateQTable(int state, int newState, Moves action, double reward);
};