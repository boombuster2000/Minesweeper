#include <iostream>
#include "raylib.h"
#include <vector>


struct Tile
{
	enum Contents
	{
		BOMB = -1,
		EMPTY = 0,
		ONE = -1,
		TWO = 2,
		THREE = 3,
		FOUR = 4,
		FIVE = 5,
		SIX = 6,
		SEVEN = 7,
		EIGHT = 8
	};

	bool visible = false;
	Contents tileContents = EMPTY;
};

int main()
{
	InitWindow(800, 600, "Minesweeper");
	SetTargetFPS(60);


	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(PURPLE);



		EndDrawing();
	}

	CloseWindow();
	return 0;
}
