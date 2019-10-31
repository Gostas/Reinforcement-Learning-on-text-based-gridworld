#include "pch.h"
#include "TextEnvironment.h"
#include <ctime>
#include <cstdio>

const int CONSOLE_BUFFER_SIZE = 256;
const int FILE_BUFFER_SIZE = 512;

TextEnvironment::TextEnvironment() 
{ 
	saved = true;	//prevent the loaded grid from being saved, since this constructor is only called when
	loaded = true;  //you want to load a grid
	gridSize = 0;
	obstacleCount = -1;
	start = COORD({ 0,0 });
	prevAgentPos = start;
	goal = start;
	rewards = nullptr;
	grid = nullptr;
}

TextEnvironment::TextEnvironment(int obstacleCount, int gridSize)
{
	COORD obstacle;
	int minDistance;
	int i;

	std::random_device rd;	//to generate random seed
	std::mt19937 rng(rd());  //mersenne - twister engine with random seed
	std::uniform_int_distribution<int> uni(1, gridSize - 2); //function to make each random number have an equal oppurtunity of being selected

	this->gridSize = gridSize;
	this->obstacleCount = obstacleCount;

	saved = false;
	loaded = false;

	memoryAllocator();

	if (gridSize == 5)
		minDistance = 2;
	else if (gridSize == 6)
		minDistance = 3;
	else if (gridSize <= 8)
		minDistance = 4;
	else if (gridSize <= 12)
		minDistance = 6;
	else
		minDistance = 9;

	start.X = uni(rng);
	goal.X = uni(rng);

	do
	{
		start.Y = uni(rng);
		goal.Y = uni(rng);
	} while (sqrt(pow(start.X - goal.X, 2) + pow(start.Y - goal.Y, 2)) < minDistance); 	//ensure that goal and start are a fair distance apart r = sqrt[(x1-x2)^2+(y1-y2)^2]

	grid[goal.X][goal.Y] = 'G';		//set symbol for goal and start
	grid[start.X][start.Y] = 'S';

	for (i = 0; i < obstacleCount; ++i)		//create random positions for obstacles
	{
		do
		{
			obstacle = GetRandomCoordinates();
		} while (grid[obstacle.X][obstacle.Y] == 'G' || grid[obstacle.X][obstacle.Y] == 'S' || grid[obstacle.X][obstacle.Y] == '*');
		obstCoord.push_back(obstacle);
		grid[obstacle.X][obstacle.Y] = '*';
	}
	
	createGrid();
}    

TextEnvironment::~TextEnvironment()	//free up memory
{
	for (int i = 0; i < gridSize; ++i)
	{
		delete[] grid[i];
		delete[] rewards[i];
	}

	delete[] grid;
	delete[] rewards;
}

void TextEnvironment::memoryAllocator()
{
	register int i;
	prevAgentPos = start;

	try
	{
		grid = new char *[gridSize];		//allocate space for grid
		rewards = new double *[gridSize];
	}
	catch (const std::bad_alloc& ba)
	{
		printf("Error allocating memory\n");
		throw 6;	//this will trow an exception in main.cpp
	}

	for (i = 0; i < gridSize; ++i)	//create the dynamic 2D arrays that will contain the rewards and the grid
	{
		try
		{
			grid[i] = new char[gridSize];
			rewards[i] = new double[gridSize];
		}
		catch (const std::bad_alloc& ba)
		{
			printf("Error allocating memory for second dimension\n");
			throw 7;	//this will trow an exception in main.cpp
		}
	}

	for (i = 0; i < gridSize; ++i)
	{
		for (int j = 0; j < gridSize; ++j)
			grid[j][i] = 0;
	}
}

void TextEnvironment::createGrid()
{
	register int i;

	rewards[goal.X][goal.Y] = 1;	//set reward value for goal and start positions
	rewards[start.X][start.Y] = -0.025;

	grid[goal.X][goal.Y] = 'G';		//set symbol for goal and start
	grid[start.X][start.Y] = 'S';

	for (i = 0; i < gridSize; ++i)
	{
		grid[0][i] = '*';	 //create left wall
		rewards[0][i] = -1;

		grid[gridSize - 1][i] = '*';	//create right wall
		rewards[gridSize - 1][i] = -1;

		grid[i][0] = '*';	//create top wall
		rewards[i][0] = -1;

		grid[i][gridSize - 1] = '*'; //create bottom wall
		rewards[i][gridSize - 1] = -1;
	}

	for (std::vector<COORD>::iterator it = obstCoord.begin(); it != obstCoord.end(); ++it)	//put obstacles into grid and assign the correct reward value
	{
		grid[it->X][it->Y] = '*';
		rewards[it->X][it->Y] = -1;
	}

	for (i = 1; i < gridSize; ++i)		//add a '-' where there is a free space
	{
		for (int j = 1; j < gridSize; ++j)
		{
			if (grid[j][i] != 'S' && grid[j][i] != 'G' && grid[j][i]!='*')
			{
				rewards[j][i] = -0.025;
				grid[j][i] = '-';
			}	
		}
	}
}

COORD TextEnvironment::GetRandomCoordinates()
{
	std::random_device rd;	//to generate random seed
	std::mt19937 rng(rd());  //mersenne - twister engine with random seed
	std::uniform_int_distribution<int> uni(1, gridSize-2); //function to make each random number have an equal oppurtunity of being selected
	COORD randCoord;

	randCoord.X = uni(rng);
	randCoord.Y = uni(rng);

	return randCoord;
}

void TextEnvironment::drawGrid()
{
	int i,
		j;

	for (i = 0; i < gridSize; ++i)	//Y COORD
	{
		for (j = 0; j < gridSize; ++j)	//X COORD
		{
			if (j == start.X && i == start.Y)
				printf("%c ", 'R');
			else
				printf("%c ", grid[j][i]);
		}
		printf("\n");
	}
}

void TextEnvironment::updateGridOutput(int state, COORD startingPoint)
{
	COORD dest;
	HANDLE hStdout;

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	dest.X = 2 * prevAgentPos.X + startingPoint.X;	//account for the spaces between the characters in the grid
	dest.Y = prevAgentPos.Y + startingPoint.Y;

	SetConsoleCursorPosition(hStdout, dest);

	printf("%c", grid[prevAgentPos.X][prevAgentPos.Y]);

	prevAgentPos = getCoordFromState(state);

	dest.X = 2 * prevAgentPos.X;
	dest.Y = prevAgentPos.Y + 4;

	SetConsoleCursorPosition(hStdout, dest);

	printf("%c", 'R');
}

int TextEnvironment::getNewState(int state, Moves action)
{
	COORD pos;

	pos = getCoordFromState(state);

	switch (action)
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

	return (gridSize * pos.Y + pos.X);
}

COORD TextEnvironment::getCoordFromState(int state)
{
	COORD temp;

	temp.X = state % gridSize;
	temp.Y = state / gridSize;

	return temp;
}

char **TextEnvironment::getGrid()
{
	char **tempArray = new char *[gridSize];

	for (int i = 0; i < gridSize; ++i)
	{
		tempArray[i] = new char[gridSize];
		for (int j = 0; j < gridSize; ++j)
			tempArray[i][j] = grid[i][j];
	}

	return tempArray;
}

void TextEnvironment::writeGridToFile()
{
	FILE *pfile;
	time_t now = time(0);
	tm t;
	char fileName[] = "grids.txt";
	localtime_s(&t, &now);

	fopen_s(&pfile, fileName, "a");	// Open file for output at the end of a file

	fprintf(pfile, "%i/%i/%i %i:%i:%i ", t.tm_mday, t.tm_mon + 1, 1900 + t.tm_year, t.tm_hour, t.tm_min, t.tm_sec);

	fprintf(pfile, "%i %i ", gridSize, obstacleCount);

	for (std::vector<COORD>::iterator i = obstCoord.begin(); i != obstCoord.end(); ++i)		//Example line: 3/12/2018 13:29:8 10 7 3,2 8,6 x,y x1,y1 ...
		fprintf(pfile, "%i,%i ", i->X, i->Y);

	fprintf(pfile, "%i,%i %i,%i\n", start.X, start.Y, goal.X, goal.Y);

	fflush(pfile);	//write any unwritten data in the stream's output buffer to the file.
	fclose(pfile);
}

void TextEnvironment::save()
{
	if (!saved)	//prevent the same grid from being saved multiple times to the text file
	{
		writeGridToFile();
		saved = true;
	}

}

bool TextEnvironment::printSavedGridInfo()
{
	FILE *pFile;
	char line[FILE_BUFFER_SIZE];
	int count,
		whiteSpaceCount;
	bool moreSpace = false;
	char fileName[] = "grids.txt";
	
	fopen_s(&pFile, fileName, "r");

	if (pFile == NULL)
	{
		printf("Error opening file\n");
		return false;
	}

	fgets(line, FILE_BUFFER_SIZE - 1, pFile);

	printf("  Date   |  Time  | Grid size | No. of obstacles\n");

	while (!feof(pFile))	//while end-of-file has not been reached
	{
		count = 0;
		whiteSpaceCount = 0;
		while (whiteSpaceCount<4 && count < 24)
		{
			if (line[count] == ' ')
			{
				whiteSpaceCount++;
				printf(" ");
				if (whiteSpaceCount > 1)
				{
					printf("        ");
					moreSpace = true;
				}
			}
			else
			{
				printf("%c", line[count]);
			}
			++count;
		}
		printf("\n");
		fgets(line, FILE_BUFFER_SIZE - 1, pFile);
	}

	fclose(pFile);
	return true;
}

bool TextEnvironment::load()
{
	char sDate[CONSOLE_BUFFER_SIZE], sTime[CONSOLE_BUFFER_SIZE];	//data entered by user
	char fDate[50], fTime[50], fGridSize[50], fObstacleCount[50], fXcoord[50], fYcoord[50]; //data read from file
	char line[FILE_BUFFER_SIZE];
	char fileName[] = "grids.txt";

	int count, //holds the current position on the line read from the text file
		count2,	//holds the number we need to subtract from count in order to write the data in the correct position in each string
		coordCount; //counts the number of coordinates written from file

	bool valid,
		hitComma;	//keeps track whether a comma has been encountered

	FILE *pFile;

	COORD temp;		//will hold the coordinates read from file before they are added to obstCoord

	printf("Grid layouts saved are the following:\n");
	if (!printSavedGridInfo())
		return false;

	printf("Please type in the date and time in separate lines\n");
	fgets(sDate, CONSOLE_BUFFER_SIZE - 1, stdin);
	fgets(sTime, CONSOLE_BUFFER_SIZE - 1, stdin);

	count = 0;
	while (sDate[count] != '\n') ++count;
	sDate[count] = '\0';		//end the string

	count = 0;
	while (sTime[count] != '\n') ++count;
	sTime[count] = '\0';

	fopen_s(&pFile, fileName, "r");

	if (pFile == NULL)
	{
		printf("Error opening file\n");
		return false;
	}

	valid = false;
	while (!feof(pFile) && !valid)
	{
		fgets(line, FILE_BUFFER_SIZE - 1, pFile);	//read line from text file
		count = 0;
		while (line[count] != ' ')	//discern the date from the line
		{
			fDate[count] = line[count];
			++count;
		}

		fDate[count] = '\0';	//terminate the string
		count2 = ++count;
		while (line[count] != ' ')	//discern the time from the line
		{
			fTime[count-count2] = line[count];
			++count;
		}

		fTime[count - count2] = '\0';
		
		if (!strcmp(fDate, sDate) && !strcmp(fTime, sTime))  //compare date and time read from file with
			valid = true;									 //with date and time entered by user
	}
	fclose(pFile);

	if (!valid)
	{
		printf("Entry not found\n");
		return false;
	}

	count2 = ++count;
	while (line[count] != ' ')	//read grid size from text file
	{
		fGridSize[count - count2] = line[count];
		++count;
	}

	fGridSize[count - count2] = '\0';
	count2 = ++count;
	while (line[count] != ' ') //read obstacle number from text file
	{
		fObstacleCount[count - count2] = line[count];
		++count;
	}

	fObstacleCount[count - count2] = '\0';

	this->gridSize = atoi(fGridSize);

	if (this->gridSize <5)
		return false;

	this->obstacleCount = atoi(fObstacleCount);

	coordCount = 0;

	hitComma = false;

	count2 = 0;
	while (coordCount < this->obstacleCount + 2 && line[count]) //make sure coordinates are read even if text file is not in required format
	{																  //e.g. is modified by user - defensive programming
		if (line[count] == ' ' && !hitComma)	//ignore whitespaces between two coordinates or between two numbers
			++count;
		else if ((line[count] == ' ' || line[count]=='\n') && hitComma) //if while reading y-coordinate (hitComma is true) a whitespace is read,
		{																//then we have finished reading a pair of coordinates
			fYcoord[count2] = '\0';

			temp.X = atoi(fXcoord);
			temp.Y = atoi(fYcoord);

			obstCoord.push_back(temp);

			hitComma = false;
			count2 = 0;
			++count;
			++coordCount;
		}
		else if (isdigit(line[count]) && !hitComma)
		{
			fXcoord[count2] = line[count];
			++count;
			++count2;
		}
		else if (isdigit(line[count]) && hitComma)
		{
			fYcoord[count2] = line[count];
			++count;
			++count2;
		}
		else if (line[count] == ',')
		{
			fXcoord[count2] = '\0';
			count2 = 0;
			++count;
			hitComma = true;
		}
		else	//ignore any unrecognized characters
			++count;
	}

	goal = obstCoord.back();
	obstCoord.pop_back();

	start = obstCoord.back();
	obstCoord.pop_back();

	memoryAllocator();
	createGrid();
	//now grid is loaded and created
	//text environment is ready!!

	return true;	//success!!
}

//void TextEnvironment::printRewards()
//{
//	FILE* pfile;
//	fopen_s(&pfile, "rewards.txt", "a");
//
//	for (int i = 0; i < gridSize; ++i)
//	{
//		for (int j = 0; j < gridSize; ++j)
//			fprintf(pfile, "%.2lf ",rewards[i][j]);
//
//		fprintf(pfile, "\n");
//	}
//
//	fclose(pfile);
//}