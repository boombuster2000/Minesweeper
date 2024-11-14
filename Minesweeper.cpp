#include <iostream>
#include "raylib.h"

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
