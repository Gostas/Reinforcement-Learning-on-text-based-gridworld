#include "pch.h"
#include "TextEnvironment.h"
#include "QLearningAgent.h"
#include "SARSA_Agent.h"
#include "ShortestPathFinder.h"

using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.

const int BUFFER_SIZE = 256;
const int MAX_EPISODES = 15000;

struct Steps
{
	unsigned int steps; 
	bool reachedGoal;
};

//Forward function declarations

void outputInfo(TextEnvironment *env, double epsilon, Agent *a, int episodes);

std::vector<Steps> Q_Learning(TextEnvironment *env, QLearningAgent *agent, const double EPSILON, bool outputGrid = true);

std::vector<Steps> SARSA(TextEnvironment *env, SARSA_Agent *agent, const double EPSILON, bool outputGrid = true);

double checkVal(double minm, double maxm, bool isInt);	//check whether input is a number and within specified range

void printToFile(std::vector<Steps> data1, std::vector<Steps>data2, FILE *hFile);

void printToFile(std::vector<int> data1, std::vector<int> data2, double startingValue, double interval, FILE *hFile);  //function overloading

void pause(const char *MSG);

void moveCursorToBtm(int gridSize);

int main()
{
	TextEnvironment *env;	//object which will hold the text environment
	QLearningAgent *qAgent;	//object which will hold agent which runs Q-Learning
	SARSA_Agent *sAgent;	//object which will hold agent which runs SARSA
	ShortestPathFinder *pathChecker;	//object which will be used to perform shortest path finding on grid


	int numberOfObstacles,
		gridSize,
		choice,
		choice2;

	double alpha,
		gamma,
		epsilon;

	bool happy;

	char yesOrNo[BUFFER_SIZE];

	char batchFileName[] = "movesDataBatch.csv";
	char interactiveFileName[] = "movesData.csv";

	std::vector<Steps> movesOutputData1, movesOutputData2;

	FILE *pfile;

	SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE), CONSOLE_FULLSCREEN_MODE, NULL);	//set console to fullscreen mode

	printf("Please select:\n");
	printf("\t1. Interactive mode\n\t2. Batch processing\n");
	choice = static_cast<int>(checkVal(1, 2, true));

	printf("\n\n");

	happy = false;

	printf("Do you want to create a new grid or load one? (1/2)\n");
	choice2 = static_cast<int>(checkVal(1, 2, true));

	printf("\n\n");

	if (choice2 == 1)
	{
		do
		{
			printf("Please enter size of grid: ");
			gridSize = static_cast<int>(checkVal(5, 27, true));

			printf("\n\n");

			printf("Please enter the number of obstacles: ");
			numberOfObstacles = static_cast<int>(checkVal(0, gridSize*gridSize - 2 * gridSize - 2 * (gridSize - 1) - 5, true));

			printf("\n\n");

			try		//defensive programming
			{
				env = new TextEnvironment(numberOfObstacles, gridSize);
			}
			catch (int i)
			{
				fprintf(stderr, "\nSize of grid exceeds limits\n");
				pause("Press enter to continue");
				return -1;
			}


			env->drawGrid();
			printf("Are you happy with this grid? (Y/N)\n");
			fgets(yesOrNo, BUFFER_SIZE - 1, stdin);
			yesOrNo[0] = toupper(yesOrNo[0]);

			while (yesOrNo[0] != 'Y' && yesOrNo[0] != 'N' || strlen(yesOrNo)>2)
			{
				fgets(yesOrNo, BUFFER_SIZE - 1, stdin);
				yesOrNo[0] = toupper(yesOrNo[0]);
			}

			if (yesOrNo[0] == 'Y')
				happy = true;
			else
				delete env;

			printf("\n\n");
		} while (!happy);

		printf("Do you want to save this grid? (Y/N)\n");
		fgets(yesOrNo, BUFFER_SIZE - 1, stdin);
		yesOrNo[0] = toupper(yesOrNo[0]);
		while (yesOrNo[0] != 'Y' && yesOrNo[0] != 'N' || strlen(yesOrNo)>2)
		{
			fgets(yesOrNo, BUFFER_SIZE - 1, stdin);
			yesOrNo[0] = toupper(yesOrNo[0]);
		}

		if (yesOrNo[0] == 'Y')
			env->save();

		printf("\n\n");
	}
	else
	{
		env = new TextEnvironment();

		if (!env->load())
		{
			pause("Press enter to exit");
			return -1;
		}
		gridSize = env->getSize();
		numberOfObstacles = env->getObstacleNum();
	}

	printf("\n\n\n");

	//determine the starting value of epsilon depending on the size of the grid, so that the agent is able to explore the whole grid
	if (gridSize <= 6)
		epsilon = 0.2;

	else if (gridSize <= 8)
		epsilon = 0.6;

	else if (gridSize <= 10)
		epsilon = 0.7;

	else if (gridSize <= 20)
		epsilon = 0.85;

	else
		epsilon = 0.9;

	
	pathChecker = new ShortestPathFinder(env);
	pathChecker->BFS();

	//....................start of interactive mode.................
	if (choice == 1)
	{
		printf("Please enter value of alpha: ");
		alpha = checkVal(0, 1, false);
		printf("\n\n");

		printf("Please enter the value of gamma: ");
		gamma = checkVal(0, 1, false);

		qAgent = new QLearningAgent(alpha, gamma, env->getStartState(), gridSize);
		sAgent = new SARSA_Agent(alpha, gamma, env->getStartState(), gridSize);

		system("cls");

		printf("Q-Learning\nEpisodes completed: 0\nEpsilon = %.2lf\n\n", epsilon);
		env->drawGrid();
		printf("\n");
		qAgent->printQTable();

		/*fopen_s(&pfile, "rewards.txt", "a");
		fprintf(pfile, "Rewards before Q-Learning\n");
		fclose(pfile);
		env->printRewards();*/

		movesOutputData1 = Q_Learning(env, qAgent, epsilon);	//run Q-learning subroutine

		/*fopen_s(&pfile, "rewards.txt", "a");
		fprintf(pfile, "Rewards after Q-Learning\n");
		fclose(pfile);
		env->printRewards();*/

		moveCursorToBtm(gridSize);
		pause("Press enter to continue");

		system("cls");		

		printf("SARSA\nEpisodes completed: 0\nEpsilon = %.2lf\n\n", epsilon);

		env->drawGrid();
		printf("\n");
		sAgent->printQTable();

		movesOutputData2 = SARSA(env, sAgent, epsilon);	//run Sarsa subroutine
		moveCursorToBtm(gridSize);
		pause("Press enter to continue");

		fopen_s(&pfile, interactiveFileName, "w");		//print to file

		fprintf(pfile, "Episodes,Q-learning,Sarsa\n");
		printToFile(movesOutputData1, movesOutputData2, pfile);

		fclose(pfile);

		delete qAgent;
		delete sAgent;
		system("cls");
	//	printf("Number of episodes:	 %d\n", numberOfEpisodes);
		printf("Grid size:   %d\n", env->getSize());
		printf("Number of obstacles:   %d\n\n", numberOfObstacles);
		printf("Parameters for RL algorithms:\n");
		printf("alpha = %.2lf\n", alpha);
		printf("gamma = %.2lf\n\n", gamma);
		printf("Shortest path length using:\n\tBreadth First Search:   %i\n", pathChecker->BFS());

		if (!movesOutputData1[movesOutputData1.size() - 1].reachedGoal)
			printf("\tQ-Learning:  PATH NOT FOUND\n\n");
		else
			printf("\tQ-Learning path:   %i moves (converged after %i episodes)\n\n", movesOutputData1[movesOutputData1.size() - 1].steps, movesOutputData1.size());

		if (!movesOutputData2[movesOutputData2.size() - 1].reachedGoal)
			printf("\tSARSA:  PATH NOT FOUND\n");
		else
			printf("\tSARSA path:  %i moves (converged after %i episodes) \n", movesOutputData2[movesOutputData2.size() - 1].steps, movesOutputData2.size());
		
		system("graphInteractive.py");
	}
	//..................end of interactive mode.....................

	//..............start of batch processing mode..................
	else
	{
		std::vector<int>QLearningEpisodes, SarsaEpisodes;
		double lb, ub, interval;	//lower bound and upper bound for either alpha or gamma

		printf("Perform parameter sweep while changing value of:\n\t1. alpha\n\t2. Gamma\n");
		choice = static_cast<int>(checkVal(1.0, 2.0, true));
		printf("\n\n\n");

		//keep gamma constant
		if (choice == 1)
		{
			printf("Please enter lower bound for alpha:");
			lb = checkVal(0, 1, false);
			printf("\n\n");

			printf("Please enter upper bound for alpha:");
			ub = checkVal(lb, 1, false);
			printf("\n\n");

			printf("Please enter interval:  ");
			interval = checkVal(0.05, 0.9, false);
			printf("\n\n");

			printf("Please enter value of gamma:");
			gamma = checkVal(0, 1, false);
			printf("\n\n\n");

			printf("Shortest path length is: %d\n\n", pathChecker->BFS());

            printf("------Running Tests------\n");
			for (int i = 0; i <= round((ub - lb)/interval); ++i)
			{
				alpha = lb + i * interval;

				printf("Test %d: alpha = %.2lf, gamma = %.2lf .........", i, alpha, gamma);

				qAgent = new QLearningAgent(alpha, gamma, env->getStartState(), gridSize);
				sAgent = new SARSA_Agent(alpha, gamma, env->getStartState(), gridSize);

				movesOutputData1 = Q_Learning(env, qAgent, epsilon, false); //run Q-Learning non interactive
				movesOutputData2 = SARSA(env, sAgent, epsilon, false); //run Sarsa non interactive

				QLearningEpisodes.push_back(movesOutputData1.size());
				SarsaEpisodes.push_back(movesOutputData2.size());

				printf("complete!\n");

				if (!movesOutputData1[movesOutputData1.size() - 1].reachedGoal)
					printf("\tQ-Learning:  PATH NOT FOUND\n");
				else
					printf("\tQ-Learning path:   %i moves (converged after %i episodes)\n", movesOutputData1[movesOutputData1.size() - 1].steps, movesOutputData1.size());

				if (!movesOutputData2[movesOutputData2.size() - 1].reachedGoal)
					printf("\tSARSA:  PATH NOT FOUND\n\n\n");
				else
					printf("\tSARSA path:  %i moves (converged after %i episodes) \n\n\n", movesOutputData2[movesOutputData2.size() - 1].steps, movesOutputData2.size());

				movesOutputData1.clear();
				movesOutputData2.clear();

				delete qAgent;
				delete sAgent;
			}
			
		}
		//keep alpha constant
		else
		{
			printf("Please enter lower bound for gamma:\n");
			lb = checkVal(0, 1, false);
			printf("\n\n");

			printf("Please enter upper bound for gamma:\n");
			ub = checkVal(lb, 1, false);
			printf("\n\n");

			printf("Please enter interval:  ");
			interval = checkVal(0.05, 0.9, false);
			printf("\n\n");

			printf("Please enter value of alpha:\n");
			alpha = checkVal(0, 1, false);
			printf("\n\n\n");

			printf("Shortest path lenght is: %d\n\n", pathChecker->BFS());

			printf("------Running Tests------\n");

			for (int i = 0; i <= round((ub - lb)/interval); ++i)
			{
				gamma = lb + i * interval;

				printf("Test %d: alpha = %.2lf, gamma = %.2lf .........", i, alpha, gamma);

				qAgent = new QLearningAgent(alpha, gamma, env->getStartState(), gridSize);
				sAgent = new SARSA_Agent(alpha, gamma, env->getStartState(), gridSize);

				movesOutputData1 = Q_Learning(env, qAgent, epsilon, false);	//run Q-Learning non interactive
				movesOutputData2 = SARSA(env, sAgent, epsilon, false); //run Sarsa non interactive

				QLearningEpisodes.push_back(movesOutputData1.size());
				SarsaEpisodes.push_back(movesOutputData2.size());

				printf("complete!\n");
				
				if (!movesOutputData1[movesOutputData1.size() - 1].reachedGoal)
					printf("\tQ-Learning:  PATH NOT FOUND\n");
				else
					printf("\tQ-Learning path:   %i moves (converged after %i episodes)\n", movesOutputData1[movesOutputData1.size() - 1].steps, movesOutputData1.size());

				if (!movesOutputData2[movesOutputData2.size() - 1].reachedGoal)
					printf("\tSARSA:  PATH NOT FOUND\n\n\n");
				else
					printf("\tSARSA path:  %i moves (converged after %i episodes) \n\n\n", movesOutputData2[movesOutputData2.size() - 1].steps, movesOutputData2.size());

				movesOutputData1.clear();
				movesOutputData2.clear();

				delete qAgent;
				delete sAgent;
			}
		}

		fopen_s(&pfile, batchFileName, "w");	//open new file
		fprintf(pfile, "Variable,Q-Learning,Sarsa,Average\n");
		printToFile(QLearningEpisodes, SarsaEpisodes, lb, interval, pfile);
		fclose(pfile);

		if (choice == 1)
			system("graphBatchAlpha.py");
		else
			system("graphBatchGamma.py");

	}
	//..............end of batch processing mode..................

	delete env;

	pause("Press enter exit");
	return 0;
}

void outputInfo(TextEnvironment *env, double epsilon, Agent *a, int episodes)
{
	COORD dest;
	HANDLE hStdout2;
	hStdout2 = GetStdHandle(STD_OUTPUT_HANDLE);

	dest.Y = 1;
	dest.X = 20;
	SetConsoleCursorPosition(hStdout2, dest);

	printf("%d", episodes);

	dest.Y = 2;
	dest.X = 10;
	SetConsoleCursorPosition(hStdout2, dest);

	printf("%0.2lf", epsilon);

	env->updateGridOutput(a->getState(), COORD{ 0, 4 });

	sleep_for(70ms);
}

std::vector<Steps> Q_Learning(TextEnvironment *env, QLearningAgent *agent, const double EPSILON, bool outputGrid)
{
	int episodeCounter = 0,
		curr_state,
		newState;

	int lastState,
		penultimateState;

	double reward,
		epsilon = EPSILON;

	bool converged = false;
	Steps episode;
	Moves action;
	std::vector<Steps> movesOutputData;

	while (episodeCounter < MAX_EPISODES && !converged)
	{
		agent->goToStart();

		curr_state = agent->getState();

		if (outputGrid)	outputInfo(env, epsilon, agent, episodeCounter);

		lastState = curr_state;

		episode.steps = 0;
		episode.reachedGoal = false;

		do
		{	
			action = agent->pickAction(curr_state, epsilon);

			newState = env->getNewState(curr_state, action);

			reward = env->getRewardAt(newState);

			agent->updatePos(newState);
	
			agent->updateQTable(curr_state, newState, action, reward);

			if (outputGrid)
			{
				agent->updateQValueOutput(curr_state, COORD{ 0, static_cast<SHORT>(5 + env->getSize()) });
				outputInfo(env, epsilon, agent, episodeCounter);
			}

			curr_state = newState;

			penultimateState = lastState;

			lastState = curr_state;

			episode.steps++;
		} while (!env->isObstacle(newState) && !env->reachedGoal(newState) && episode.steps < 1000);

		episodeCounter++;
		
		epsilon = EPSILON * exp(-0.025 * episodeCounter);

		if (env->reachedGoal(newState))
			episode.reachedGoal = true;

		movesOutputData.push_back(episode);

		if (epsilon < 0.1)
		{
			if (agent->checkForConvergence(penultimateState, lastState) && episode.reachedGoal == true && movesOutputData[movesOutputData.size() - 1].steps == movesOutputData[movesOutputData.size() - 2].steps)
				converged = true;
		}

	}

	return movesOutputData;
}

std::vector<Steps> SARSA(TextEnvironment *env, SARSA_Agent *agent, const double EPSILON, bool outputGrid)
{
	int episodeCounter = 0,
		curr_state,
		newState;

	int lastState,
		penultimateState;

	double reward,
		epsilon;

	bool converged = false;
	Steps episode;

	Moves action1,
		action2;

	std::vector<Steps> movesOutputData;

	epsilon = EPSILON;

	while (episodeCounter < MAX_EPISODES && !converged)
	{
		agent->goToStart();

		if (outputGrid)	outputInfo(env, epsilon, agent, episodeCounter);

		curr_state = agent->getState();

		action1 = agent->pickAction(curr_state, epsilon);

		lastState = curr_state;

		episode.steps = 0;
		episode.reachedGoal = false;

		do
		{
			newState = env->getNewState(curr_state, action1);

			reward = env->getRewardAt(newState);

			agent->updatePos(newState);

			action2 = agent->pickAction(newState, epsilon);

			agent->updateQTable(curr_state, newState, action1, action2, reward);

			if (outputGrid == true)
			{
				agent->updateQValueOutput(curr_state, COORD{ 0, static_cast<SHORT>(5 + env->getSize()) });
				outputInfo(env, epsilon, agent, episodeCounter);
			}
			curr_state = newState;

			penultimateState = lastState;

			lastState = curr_state;

			action1 = action2;

			episode.steps++;

		} while (!env->isObstacle(newState) && !env->reachedGoal(newState) && episode.steps < 1000);

		episodeCounter++;

		epsilon = EPSILON * exp(-0.025 * episodeCounter);

		if (env->reachedGoal(newState))
			episode.reachedGoal = true;

		movesOutputData.push_back(episode);

		if(epsilon < 0.1)
		{
			if (agent->checkForConvergence(penultimateState, lastState) && episode.reachedGoal == true && movesOutputData[movesOutputData.size() - 1].steps == movesOutputData[movesOutputData.size() - 2].steps)
				converged = true;
		}

	}

	return movesOutputData;
}

double checkVal(double minm, double maxm, bool isInt)
{
	bool isValid;
	double val;
	char num[BUFFER_SIZE];

	do
	{
		isValid = true;

		fgets(num, BUFFER_SIZE - 1, stdin);

		val = atof(num);

		if (val == 0)
		{
			printf("Please enter a number: ");
			isValid = false;
			continue;
		}
		
		if (val < minm || val > maxm)
		{
			if (isInt)
			{
				if (maxm == 1e9) printf("Please enter an integer greater than %.0lf: ", minm);
				else printf("Please enter an integer between %.0lf and %.0lf: ", minm, maxm);
			}
			else
			{
				if (maxm == 1e9) printf("Please enter a floating point number greater than %.2lf: ", minm);
				else printf("Please enter a floating point number between %.2lf and %.2lf: ", minm, maxm);
			}
			isValid = false;
		}
	} while (!isValid);

	return val;
}

void printToFile(std::vector<Steps> data1, std::vector<Steps> data2, FILE *hFile)
{
	register unsigned int i;
	if (data1.size() <= data2.size())
	{
		for (i = 0; i < data1.size(); ++i)
		{
			fprintf(hFile, "%d,", i + 1);	//episode number
			fprintf(hFile, "%d,", data1[i].steps);
			fprintf(hFile, "%d\n", data2[i].steps);
		}

		for (i = data1.size(); i < data2.size(); ++i)	//now output data 2 only
		{
			fprintf(hFile, "%d,,", i + 1);	//episode number
			fprintf(hFile, "%d\n", data2[i].steps);
		}
	}
	else
	{
		for (i = 0; i < data2.size(); ++i)
		{
			fprintf(hFile, "%d,", i + 1);	//episode number
			fprintf(hFile, "%d,", data1[i].steps);
			fprintf(hFile, "%d\n", data2[i].steps);
		}

		for (i = data2.size(); i < data1.size(); ++i)
		{
			fprintf(hFile, "%d,", i + 1);	//episode number
			fprintf(hFile, "%d,\n", data1[i].steps);
		}
	}
	
}

void printToFile(std::vector<int> data1, std::vector<int> data2, double startingValue, double interval, FILE *hFile)
{
	for (unsigned int i = 0; i < data1.size(); ++i)
	{
		fprintf(hFile, "%lf,", startingValue + i * interval);
		fprintf(hFile, "%d,", data1[i]);
		fprintf(hFile, "%d,", data2[i]);
		fprintf(hFile, "%d\n", (data1[i] + data2[i]) / 2);
	}
}

void pause(const char *MSG)
{
	char temp[BUFFER_SIZE];
	printf("%s\n", MSG);
	gets_s(temp);
}

void moveCursorToBtm(int gridSize)
{
	COORD dest;
	HANDLE hStdout;

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	dest.X = 0;
	dest.Y = 6 + 2 * gridSize;

	SetConsoleCursorPosition(hStdout, dest);
}