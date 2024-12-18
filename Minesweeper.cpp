#include "raylib.h"
#include "GameBoard.h"
#include <vector>
#include <random>
#include <set>
#include <iostream>

class Minesweeper
{

public:

	enum class Contents : int
	{
		BOMB = -1,
		EMPTY = 0,
		ONE = 1,
		TWO = 2,
		THREE = 3,
		FOUR = 4,
		FIVE = 5,
		SIX = 6,
		SEVEN = 7,
		EIGHT = 8
	};

	struct Tile : GameBoard::Square
	{
		Contents tileContent = Contents::EMPTY;
		bool isCovered = true;

		Tile(const Square& square)
			: Square(square), tileContent(Contents::EMPTY), isCovered(true) {
		}
	};
	
	class MinesweeperGrid : public GameBoard::Grid<Tile> 
	{
		typedef std::vector<std::vector<Tile>> TileGrid;

	private:
		int GetNumberOfBombsAroundPoint(TileGrid board, IntVector2 point)
		{
			int count = 0;

			if (point.y - 1 >= 0 && point.x - 1 >= 0 && board[point.y - 1][point.x - 1].tileContent == Contents::BOMB) count++; // Top Left

			if (point.y - 1 >= 0 && board[point.y - 1][point.x].tileContent == Contents::BOMB) count++; // Top Middle

			if (point.y - 1 >= 0 && point.x + 1 < board[point.y].size() && board[point.y - 1][point.x + 1].tileContent == Contents::BOMB) count++; // Top Right


			if (point.x - 1 >= 0 && board[point.y][point.x - 1].tileContent == Contents::BOMB) count++; // Middle Left

			if (point.x + 1 < board[point.y].size() && board[point.y][point.x + 1].tileContent == Contents::BOMB) count++; // Middle Right


			if (point.y + 1 < board.size() && point.x - 1 >= 0 && board[point.y + 1][point.x - 1].tileContent == Contents::BOMB) count++; // Bottem Left

			if (point.y + 1 < board.size() && board[point.y + 1][point.x].tileContent == Contents::BOMB) count++; // Bottem Middle

			if (point.y + 1 < board.size() && point.x + 1 < board[point.y].size() && board[point.y + 1][point.x + 1].tileContent == Contents::BOMB) count++; // Bottem Right


			return count;
		}

		void PlaceBombsOnBoard(TileGrid& board) {
			const float BOMB_DENSITY = 0.15f; // 15% of tiles are bombs
			const int numberOfBombs = static_cast<int>(board.size() * board[0].size() * BOMB_DENSITY);

			std::set<std::pair<int, int>> bombCoordinates;
			while (bombCoordinates.size() < numberOfBombs) {
				bombCoordinates.insert({
					GenerateRandomInteger(0, board[0].size() - 1),
					GenerateRandomInteger(0, board.size() - 1)
					});
			}

			for (const auto& [x, y] : bombCoordinates) {
				board[y][x].tileContent = Contents::BOMB;
			}

			// Simplified number assignment
			for (int y = 0; y < board.size(); y++) {
				for (int x = 0; x < board[0].size(); x++) {
					if (board[y][x].tileContent != Contents::BOMB) {
						int bombCount = GetNumberOfBombsAroundPoint(board, IntVector2{ x,y });
						board[y][x].tileContent = static_cast<Contents>(bombCount);
					}
				}
			}
		}

	public:
		MinesweeperGrid(IntVector2 dimensions, Minesweeper::Tile sampleTile, GameBoard::Grid<Tile>::AnchorPoints anchorPoint, IntVector2 position)
			: Grid(dimensions, sampleTile, anchorPoint, position)
		{
		}

		void ProcessMouseInput()
		{
			if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;


			Vector2 mousePosition = GetMousePosition();

			for (int y = 0; y < m_grid.size(); y++)
			{
				for (int x = 0; x < m_grid[y].size(); x++)
				{
					Tile tile = m_grid[y][x];
					if (tile.positionOnScreen.x - tile.dimensions.x < mousePosition.x && tile.positionOnScreen.x + tile.dimensions.x > mousePosition.x
						&& tile.positionOnScreen.y - tile.dimensions.y < mousePosition.y && tile.positionOnScreen.y + tile.dimensions.y > mousePosition.y)
					{
						tile.colour = PURPLE;
					}
				}
			}
		}
	};
};

int main()
{
	InitWindow(800, 600, "Minesweeper");
	SetTargetFPS(60);

	GameBoard::Square sampleTile({ 40,40 }, 10, RED);

	Minesweeper::MinesweeperGrid board(
		IntVector2{ 9,9 }, 
		sampleTile, 
		GameBoard::Grid<Minesweeper::Tile>::AnchorPoints::MIDDLE,
		IntVector2{ GetScreenWidth() / 2,GetScreenHeight() / 2 }
	);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);

		board.DisplayGrid();
		board.ProcessMouseInput();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
