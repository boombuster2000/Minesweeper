#include "raylib.h"
#include "minesweeper.h"


int main()
{
	InitWindow(800, 600, "Minesweeper");
	InitAudioDevice(); 
	SetTargetFPS(60);

	//Loading assets
	Minesweeper::assets.textures.LoadAll(LoadDirectoryFiles("./assets/textures"));
	Minesweeper::assets.fonts.LoadAll(LoadDirectoryFiles("./assets/fonts"));
	Minesweeper::assets.sounds.LoadAll(LoadDirectoryFiles("./assets/sounds"));

	// Creating Text Instances
	RaylibFramework::Text flagsLeft("Flags Left: 0", 20, RED, Minesweeper::assets.fonts.Get("arialroundedmtbold"));
	flagsLeft.SetPositionOnScreen(GetScreenWidth() - 170, 80);

	RaylibFramework::Text winText("You found all the bombs!", 50, BLUE, Minesweeper::assets.fonts.Get("arialroundedmtbold"));
	winText.SetPositionOnScreen(10, 10);

	RaylibFramework::Text loseText("You triggered a bomb!", 50, BLUE, Minesweeper::assets.fonts.Get("arialroundedmtbold"));
	loseText.SetPositionOnScreen(10, 10);

	RaylibFramework::Text playAgainText("Press ENTER to play again or ESC to exit", 30, BLUE, Minesweeper::assets.fonts.Get("arialroundedmtbold"));
	playAgainText.SetPositionOnScreen(10, GetScreenHeight() - 50);

	Minesweeper::Tile sampleTile(IntVector2{ 40,40 }, IntVector2{ 10,10 });
	bool shouldPlayAgain = true;

	while (!WindowShouldClose() || shouldPlayAgain)
	{
		Minesweeper::MinesweeperGrid game(
			IntVector2{ 9,9 },
			sampleTile,
			RaylibFramework::AnchorPoints::MIDDLE,
			IntVector2{ GetScreenWidth() / 2, GetScreenHeight() / 2 }
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
	
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
