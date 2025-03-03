#include "raylib.h"
#include "minesweeper.h"


int main()
{
	InitWindow(800, 600, "Minesweeper");
	SetTargetFPS(60);

	Minesweeper::texturesHandler.LoadTextures();

	Minesweeper::Tile sampleTile(IntVector2{ 40,40 }, IntVector2{ 10,10 });

	Gameboard::Text flagsLeft("Flags Left: 0", 20, RED);
	flagsLeft.SetPositionOnScreen(GetScreenWidth() - 170, 80);

	Gameboard::Text winText("You found all the bombs!", 50, BLUE);
	winText.SetPositionOnScreen(10, 10);

	Gameboard::Text loseText("You triggered a bomb!", 50, BLUE);
	loseText.SetPositionOnScreen(10, 10);

	Gameboard::Text playAgainText("Press ENTER to play again or ESC to exit", 30, BLUE);
	playAgainText.SetPositionOnScreen(10, GetScreenHeight() - 50);

	bool shouldPlayAgain = true;

	while (!WindowShouldClose() || shouldPlayAgain)
	{
		Minesweeper::MinesweeperGrid game(
			IntVector2{ 9,9 },
			sampleTile,
			Gameboard::AnchorPoints::MIDDLE,
			IntVector2{ GetScreenWidth() / 2,GetScreenHeight() / 2 }
		);

		// MAIN GAME LOOP
		while (true)
		{
			if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose())
			{
				shouldPlayAgain = false;
				break;
			}

			BeginDrawing();
			ClearBackground(RAYWHITE);

			game.DisplayGrid();

			const int numberOfFlagsLeft = game.GetNumberOfFlagsLeft();
			flagsLeft.SetText(TextFormat("Flags Left: %d", numberOfFlagsLeft));
			flagsLeft.Render();


			if (!game.IsBombTriggered() && game.GetNumberOfBombsLeft() != 0)
			{
				game.ProcessMouseInput();
			}
			else
			{
				playAgainText.Render();
				if (IsKeyPressed(KEY_ENTER))
				{
					break;
				}

				if (game.GetNumberOfBombsLeft() == 0)
				{
					winText.Render();

				}
				else
				{
					loseText.Render();
					game.DisplayBombs();

				}
			}

			EndDrawing();
		}
	}

	CloseWindow();
	return 0;
}
