#pragma once
#include "movesEnum.h"

class TextEnvironment
{
	int gridSize;
	int obstacleCount;
	COORD goal;
	COORD start;
	COORD prevAgentPos;
	double **rewards;
	char **grid;
	bool saved;	//indicates if grid has been saved
	bool loaded;	//indicates if grid has been loaded
	//save obstacle coordinates
	std::vector<COORD> obstCoord;

	//allocates memory for the grid and rewards array
	void memoryAllocator();

	// create the actual grid and rewards table: assign values to the arrays
	void createGrid();

	//write specific details about the current grid to text file so it can be retrieved later
	void writeGridToFile();

	//returns true of it was able to open the text file containing the saved grid info
	//prints basic info about the grids saved
	bool printSavedGridInfo();

	//returns a pair of random coordinates within the grid
	COORD GetRandomCoordinates();

	//returns the coordinates that correspond to the given state
	COORD getCoordFromState(int state);

public:
	//default constructor
	//initialize member variables
	TextEnvironment();

	//constructor-initialize values (polymorphism)
	TextEnvironment(int obstacleCount, int gridSize);

	//destructor-free memory from grid and reward array
	~TextEnvironment();

	//print the whole grid
	void drawGrid();

	//change the output of the grid where the agent has moved
	void updateGridOutput(int state, COORD startingPoint);

	//acts as an interface to save the current grid details to file
	void save();

	//returns true if load was successful, false otherwise
	bool load();

	//return the new state that occurs after the agent executes action
	int getNewState(int state, Moves action);

	//returns size of grid
	inline int getSize() { return gridSize; }

	//returns number of obstacles
	inline int getObstacleNum() { return obstacleCount; }

	//returns the starting state
	inline int getStartState() { return (start.X + gridSize * start.Y); }

	//returns the associated reward at a specific state
	inline double getRewardAt(int state) { return rewards[getCoordFromState(state).X][getCoordFromState(state).Y]; }

	//returns true if state is equal to the goal state
	inline bool reachedGoal(int state) { return state == (goal.X + gridSize * goal.Y); }

	//returns true if given position is an obstacle
	inline bool isObstacle(int state) {return grid[getCoordFromState(state).X][getCoordFromState(state).Y] == '*' ? true : false; }

	//returns a copy of the grid; restrict access to member variables
	char **getGrid();

	//void printRewards();
};