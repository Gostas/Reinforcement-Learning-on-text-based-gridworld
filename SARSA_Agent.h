#pragma once
#include "Agent.h"

//inherits class Agent
class SARSA_Agent :
	public Agent
{

public:
	using Agent::Agent; //use constructor from base class
	//destructor-free up memory
	~SARSA_Agent();
	void updateQTable(int state, int newState, Moves action, Moves nextAction, double reward);
};