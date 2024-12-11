#include "raylib.h"
#include "GameBoard.h"
#include <vector>
#include <random>

class Minesweeper : public GameBoard {

private:
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

	struct Tile : Square
	{
		Contents tileContent = Contents::EMPTY;
		bool isCovered = false;

		Tile(const GameBoard::Square& square)
			: Square(square), tileContent(Contents::EMPTY), isCovered(false) {
		}
	};
	
	typedef std::vector<std::vector<Tile>> TileGrid;

	TileGrid m_grid;

private:
	int GetNumberOfBombsAroundPoint(TileGrid board, IntVector2 point)
	{
		int count = 0;

		if (point.y - 1 > 0 && point.x - 1 > 0 && board[point.y - 1][point.x - 1].tileContent == Contents::BOMB) count++; // Top Left

		if (point.y - 1 > 0 && board[point.y - 1][point.x].tileContent == Contents::BOMB) count++; // Top Middle

		if (point.y - 1 > 0 && point.x + 1 < board[point.y].size() && board[point.y - 1][point.x + 1].tileContent == Contents::BOMB) count++; // Top Right


		if (point.x - 1 > 0 && board[point.y][point.x - 1].tileContent == Contents::BOMB) count++; // Middle Left

		if (point.x + 1 < board[point.y].size() && board[point.y][point.x + 1].tileContent == Contents::BOMB) count++; // Middle Right


		if (point.y + 1 < board.size() && point.x - 1 > 0 && board[point.y + 1][point.x - 1].tileContent == Contents::BOMB) count++; // Bottem Left

		if (point.y + 1 < board.size() && board[point.y + 1][point.x].tileContent == Contents::BOMB) count++; // Bottem Middle

		if (point.y + 1 < board.size() && point.x + 1 < board[point.y].size() && board[point.y + 1][point.x + 1].tileContent == Contents::BOMB) count++; // Bottem Right


		return count;
	}

	void PlaceBombsOnBoard(TileGrid board) {
		// Place bombs on board.

		std::vector<IntVector2> bombCoordinates;

		while (bombCoordinates.size() != board.size())
		{
			bool coordExists = false;
			IntVector2 bombCoordinateToAdd = { GenerateRandomInteger(0, board[0].size() - 1), GenerateRandomInteger(0, board.size() - 1)};

			for (IntVector2 bombCoordinate : bombCoordinates) {
				coordExists = bombCoordinate == bombCoordinateToAdd;
				if (coordExists) break;
			}

			if (!coordExists) bombCoordinates.push_back(bombCoordinateToAdd);
		}

		for (IntVector2 coordinates : bombCoordinates)
		{
			board[coordinates.y][coordinates.x].tileContent = Contents::BOMB;
		}


		// Place numbers
		for (int y = 0; y < board.size(); y++)
		{
			for (int x = 0; x < board[0].size(); x++)
			{
				int numberOfBombs = GetNumberOfBombsAroundPoint(board, IntVector2{ x,y });

				if (numberOfBombs == 1) board[y][x].tileContent = Contents::ONE;
				else if (numberOfBombs == 2) board[y][x].tileContent = Contents::TWO;
				else if (numberOfBombs == 3) board[y][x].tileContent = Contents::THREE;
				else if (numberOfBombs == 4) board[y][x].tileContent = Contents::FOUR;
				else if (numberOfBombs == 5) board[y][x].tileContent = Contents::FIVE;
				else if (numberOfBombs == 6) board[y][x].tileContent = Contents::SIX;
				else if (numberOfBombs == 7) board[y][x].tileContent = Contents::SEVEN;
				else if (numberOfBombs == 8) board[y][x].tileContent = Contents::EIGHT;
			}
		}
	}

	bool ShouldRenderSquare(IntVector2 coords) override
	{
		return !m_grid[coords.y][coords.x].isCovered;
	}

public:
	Minesweeper(IntVector2 dimensions, GameBoard::Square sampleTile) : GameBoard(dimensions, sampleTile)
	{
		m_grid = GenerateBoard<TileGrid, Tile>(IntVector2{ 9,9 }, sampleTile);
	} 
};

int main()
{
	InitWindow(800, 600, "Minesweeper");
	SetTargetFPS(60);

	GameBoard::Square sampleTile({ 40,40 }, 10, RED);

	Minesweeper board(IntVector2{ 9,9 }, sampleTile);
	board.SetAnchorPoint(GameBoard::AnchorPoints::MIDDLE);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);

		board.DisplayGrid(IntVector2{GetScreenWidth()/2,GetScreenHeight()/2});


		EndDrawing();
	}

	CloseWindow();
	return 0;
}
