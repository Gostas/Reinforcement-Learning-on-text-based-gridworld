#include "pch.h"
#include "ShortestPathFinder.h"
#include "Coordinate.h"

using namespace std;

const int MAX_SIZE = 40;

ShortestPathFinder::ShortestPathFinder(TextEnvironment *p_env)
{
	this->env = p_env;
	grid = env->getGrid();

	start.X = env->getStartState() % env->getSize();
	start.Y = env->getStartState() / env->getSize();

	spPathCost = -1;

	run = false;
}

ShortestPathFinder::~ShortestPathFinder()
{
	for (int i = 0; i < env->getSize(); ++i)
	{
		for (int j = 0; j < env->getSize(); ++j)
			delete[] grid[i];
	}	

	delete[] grid;

	delete env;
}

int ShortestPathFinder::BFS()
{
	if (run)	//if BFS has already been performed, don't run it again-return the shortest path length already found
		return spPathCost;

	int pathCost = -1;

	bool isFound[MAX_SIZE][MAX_SIZE];
	int dist[MAX_SIZE][MAX_SIZE];

	int dx[] = { 0, 0, 1, -1 };		//up, down, left, right
	int dy[] = { 1, -1, 0, 0};

	COORD currNode, newNode;

	int curr_state;

	bool reachedGoal = false;

	queue<COORD> myQ;

	for (int i = 0; i < MAX_SIZE; ++i)
		memset(isFound[i], false, MAX_SIZE);
	

	myQ.push(start);

	dist[start.X][start.Y] = 0;
	isFound[start.X][start.Y] = true;

	while (!myQ.empty() && !reachedGoal)
	{
		currNode = myQ.front();
		myQ.pop();

		for (int i = 0; i < 4; ++i)
		{
			newNode.X = currNode.X + dx[i];
			newNode.Y = currNode.Y + dy[i];

			if (!isFound[newNode.X][newNode.Y] && (grid[newNode.X][newNode.Y]=='-' || grid[newNode.X][newNode.Y] == 'G') && newNode.X > 0 && newNode.Y > 0 && newNode.X < env->getSize() && newNode.Y < env->getSize())
			{
				isFound[newNode.X][newNode.Y] = true;
				dist[newNode.X][newNode.Y] = dist[currNode.X][currNode.Y] + 1;
				myQ.push(newNode);

				curr_state = newNode.X + env->getSize()*newNode.Y;
				if (env->reachedGoal(curr_state))
				{
					reachedGoal = true;
					pathCost = dist[newNode.X][newNode.Y];
					break;
				}
			}
		}
	}
	run = true;
	spPathCost = pathCost;
	return pathCost;
}