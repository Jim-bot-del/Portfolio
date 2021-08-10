#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define NUM_ROWS 8
#define NUM_COLS 8

#define SPACE 0
#define WALL -1
#define EXIT -2

void InitialiseRoad(int road[NUM_ROWS][NUM_COLS], char side, int pos)
{
	for (int i = 0; i < NUM_ROWS; ++i) {
		for (int j = 0; j < NUM_COLS; ++j) {

			// Cycles through the entire array, setting outer rows and columns (ie 'walls') to -1, and the rest to zero.
			if (i == 0) {
				road[i][j] =  -1;
			} else if (i == (NUM_ROWS - 1)) {
				road[i][j] =  -1;
			} else if (j == 0) {
				road[i][j] =  -1;
			} else if (j == (NUM_COLS - 1)) {
				road[i][j] =  -1;
			} else {
				road[i][j] = 0;
			}

		}
	}

	// This section reads which wall the exit is on and sets that position to -2
	if (side == 'N') {
		road[0][pos] = -2;		
	} else if (side == 'E') {
		road[pos][NUM_COLS-1] = -2;
	} else if (side == 'S') {
		road[NUM_ROWS-1][pos] = -2;
	} else if (side == 'W') {
		road[pos][0] = -2;
	}
}

void PrintRoad(int road[NUM_ROWS][NUM_COLS])
{
	// char alphabet[50] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	// My first time through this, I was setting the cars to be values from 1-26 
	// (in the AddCars function) and then automatically converting that into letters when I printed.

	for (int i = 0; i < NUM_ROWS; i++) {
		for (int j = 0; j < NUM_COLS; j++) {

			// There are three possible basic characters to print, plus cars.
			if (road[i][j] == -1) {
				printf("#");
			} else if (road[i][j] == 0) {
				printf(" ");
			} else if (road[i][j] == -2) {
				printf("O");
			} else {
				// printf("%c",alphabet[road[i][j] - 1]); Obselete now
				// Instead I directly convert the car number in the array from ASCII to character, and print it
				printf("%c",road[i][j]);
			}
		}
		printf("\n");
	}


}

double PercentUsed(int road[NUM_ROWS][NUM_COLS])
{
	// OutputPercent is the number of cars, and spaces the total number of spaces
	double OutputPercent = 0;
	double spaces;

	spaces = (NUM_COLS - 2) * (NUM_ROWS - 2);

	for (int i = 1; i < (NUM_ROWS - 1); i++) {
		for (int j = 1; j < (NUM_COLS - 1); j++) {

			if (road[i][j] != 0) {
				OutputPercent++;
			}

		}
	}

	// Whenever a car is found, I increase OutputPercent. Then I return it as a percentage of the total
	OutputPercent = (OutputPercent / spaces) * 100;

	return OutputPercent;
}

void AddCar(int road[NUM_ROWS][NUM_COLS], int row, int col, int size)
{

	// I wanted to create an array which could store one grid position in each cell.
	// This requires a custom structure
	typedef struct  {
		int x;
		int y;
	} NewCar;

	// Searching the array for the largest car present. This lets me create a bigger car each time.
	int LargestCar = 'A';
	for (int i = 0; i < NUM_ROWS; i++) {
		for (int j = 0; j < NUM_COLS; j++) {

			if (road[i][j] >= LargestCar) {
				LargestCar = road[i][j] + 1;
			}
		}
	}


	// There are two basic cases - if the car is to be generated horizontally, or vertically.

	if (size > 0) {
		// The car should be horizontal

		// Storing positions of the car
		NewCar positions[100];
		for (int i = 0; i < size; i++) {
			positions[i].x = col;
			positions[i].y = row;
			col++;
		}

		// Checking for collision
		for (int j = 0; j < size; j++) {


			if (road[positions[j].y][positions[j].x] != 0) {
				return;
			}
		}

		// Writing the car by reading the structure array of positions
		for (int k = 0; k < size; k++) {
			road[positions[k].y][positions[k].x] = LargestCar;
		}

	}

	if (size < 0) {
		// The car should be vertical
		size = -1 * size;


		// Storing positions of the car
		NewCar positions[100];
		for (int i = 0; i < size; i++) {
			positions[i].x = col;
			positions[i].y = row;
			row++;
		}

		// Checking for collision
		for (int j = 0; j < size; j++) {
			
			if (road[positions[j].y][positions[j].x] != 0) {
				return;
			}
		}

		// Writing the car by reading the structure array of positions
		for (int k = 0; k < size; k++) {
			road[positions[k].y][positions[k].x] = LargestCar;
		}
	}

}

void FindCar(int road[NUM_ROWS][NUM_COLS], char move, int *rowStart, int *colStart, int *rowEnd, int *colEnd)
{
	// Searches top to bottom, left to right, meaning it will encounter the upper-most/left-most part of the car first.
	for (int i = 0; i < NUM_ROWS; i++) {
		for (int j = 0; j < NUM_COLS; j++) {
			if (road[i][j] == move) {
				*rowStart = i;
				*colStart = j;
				goto found;
			}
		}
	}

	// This 'goto found' command lets me break out of the loop early, and avoid wasting time and processing power
	// The second loop searches in the opposite direction, meaning it will find the 'end' of the car.
	found:
	for (int i = NUM_ROWS-1; i >=0; i--) {
		for (int j = NUM_COLS-1; j >= 0; j--) {
			if (road[i][j] == move) {
				*rowEnd = i;
				*colEnd = j;
				return;
			}
		}
	}

}
int MoveCar(int road[NUM_ROWS][NUM_COLS], int r0, int c0, int r1, int c1)
{
	//This function is responsible for updating the position of a car that moves
	//Inputs:
	// - The two-dimensional array representing the road 
	// - Four integer values representing the position of the car to move
	//Outputs:
	//  - none

	//initialising variables
	int i;
	int j;
	int count = 0;
	int addPos = 0;
	int temp;
	//CHECK IF CAR IS HORIZONTALLY PLACED
	if (r0 == r1)
	{

		//CHECK FOR SPACE ON THE LEFT
		if (road[r0][c0 - 1] == SPACE)
		{
			temp = road[r0][c0];
			//initialise j to be the first column the  car is  found in
			j = c0;
			while (road[r0][j - 1] == SPACE )
			{
				road[r0][j - 1] = temp;
				j--;
				count++; // counts how many spaces the first column moved before hitting something else
				addPos++; 
			}
			//reinitialise j to be the last column
			j = c1;
			while (count > 0)
			{
				road[r0][j] = SPACE;
				j--;
				count--; // changes the length of the car to match the original length
			}
			// if car has reached the exit return true else return false
			if (road[r0][c0 - addPos - 1] == EXIT)
			{
				return 1;
			} 

		//CHECK FOR SPACE ON THE RIGHT
		} else if (road[r0][c1 + 1] == SPACE)
		{
			temp = road[r0][c1];
			//initialise j to be the first column the  car is  found in
			j = c1;
			while (road[r0][j + 1] == SPACE )
			{					
				road[r0][j + 1] = temp;
				j++;
				count++; // counts how many spaces the last column moved before hitting something else
				addPos++;
			}
			//reinitialise j to be the first column				
			j = c0;
			while (count > 0)
			{
				road[r0][j] = SPACE;
				j++;
				count--; // changes the length of the car to match the original length				
			}
			// if car has reached the exit return true else return false
			if (road[r0][c1 + addPos + 1] == EXIT)
			{
				return 1;
			} 
		}
	//CHECK IF CAR IS PLACED VERTICALLY 
	}else if (c0 == c1)
	{
		//CHECK IF NO SPACES BUT TOUCHING EXIT WHEN MOVED
		if (road[r0-1][c0] == EXIT || road[r1+1][c0] == EXIT )
		{
			return 1;
		}
		
		//CHECK IF THERE IS SPACE ABOVE THE CAR
		if (road[r0 - 1][c0] == SPACE)
		{
			temp = road[r0][c0];
			//initialise i to be the first row the  car is  found in
			i = r0;
			while (road[i - 1][c0] == SPACE)
			{					
				road[i - 1][c0] = temp;
				i--;
				count++; // counts how many spaces the first row moved before hitting something else
				addPos++;
			}
			//reinitialise i to be the last row
			i = r1;
			while (count > 0)
			{
				road[i][c1] = SPACE;
				i--;
				count--; // changes the length of the car to match the original length
			}
			// if car has reached the exit return true else return false
			if (road[r0 - addPos - 1][c0] == EXIT)
			{
				return 1;
			} 
		//CHECK IF THERE IS  SPACE BELOW THE CAR
		}else if (road[r1 + 1][c0] == SPACE) 
		{
			//initialise i to be the first row the car is found in
			temp = road[r1][c0];
			i = r1;
			while (road[i + 1][c0] == SPACE )
			{
				road[i + 1][c0] = temp;
				i++;
				count++; // counts how many spaces the last row moved before hitting something else
				addPos++;
			}
			//reinitialise j to be the first column
			i = r0;
			while (count > 0)
			{
				road[i][c0] = SPACE;
				i++;
				count--; // changes the length of the car to match the original length
			}
			// if car has reached the exit return true else return false
			if (road[r1 + addPos + 1][c0] == EXIT)
			{
				return 1;
			} 
		}
	} 
	return 0;
}
	


char GetMove(void)
{
	char move;
	printf("\nMove car: ");
	scanf("%c", &move);
	// Ignore non-capital letter inputs
	while ((move < 'A') || (move > 'Z')) {
		scanf("%c", &move);
	}
	return move;
}

/* The main Traffic Jam simulation */
int main(void)
{
	int gameOver = 0;
	int road[NUM_ROWS][NUM_COLS];
	int rowStart, colStart, rowEnd, colEnd;
	char input;

	/* Print banner */
	printf(" _____           __  __ _            _                                        \n");
	printf("|_   _| __ __ _ / _|/ _(_) ___      | | __ _ _ __ ___           ____          \n");
	printf("  | || '__/ _` | |_| |_| |/ __|  _  | |/ _` | '_ ` _ \\  --   __/  |_\\_      \n");
	printf("  | || | | (_| |  _|  _| | (__  | |_| | (_| | | | | | | --- |  _     _``-.    \n");
	printf("  |_||_|  \\__,_|_| |_| |_|\\___|  \\___/ \\__,_|_| |_| |_| --  '-(_)---(_)--'\n\n");

	/* Initialise the road and add cars */
	InitialiseRoad(road, 'E', 3);
	AddCar(road, 3, 3, 2);
AddCar(road, 1, 1, 2);
AddCar(road, 2, 1, 3);
AddCar(road, 3, 2, -2);
AddCar(road, 5, 2, -2);
AddCar(road, 4, 4, -2);
AddCar(road, 6, 3, 3);
AddCar(road, 1, 5, -3);
AddCar(road, 2, 6, -2);

	/* Print status */
	printf("ENGGEN131 2020 - C Project\nTraffic Jam!  There is a lot of traffic on the road!\n");
	printf("In fact, %.2f%% of the road is cars!\n\n", PercentUsed(road));

	/* Main simulation loop */
	while (!gameOver) {
		PrintRoad(road);
		input = GetMove();
		FindCar(road, input, &rowStart, &colStart, &rowEnd, &colEnd);
		gameOver = MoveCar(road, rowStart, colStart, rowEnd, colEnd);
	}

	/* A car has exited - the simulation is over */
	PrintRoad(road);
	printf("\nCongratulations, you're on your way again!");

	return 0;
}
