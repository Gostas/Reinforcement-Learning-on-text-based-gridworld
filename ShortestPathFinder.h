#pragma once
#include "TextEnvironment.h"
class ShortestPathFinder
{
	TextEnvironment *env; // aggregation: object from other class is member of this class
	char **grid;
	COORD start;
	bool run;
	int spPathCost;

public:
	//don't allow the use of the default constructor-the constructor should always take arguments
	ShortestPathFinder() = delete;

	//overloaded contructor
	ShortestPathFinder(TextEnvironment *p_env);

	//destructor-free up memory
	~ShortestPathFinder();

	//returns shortest path cost using Breadth-First search
	int BFS();
};