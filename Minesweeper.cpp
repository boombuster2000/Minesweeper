#include <iostream>
#include "raylib.h"
#include <vector>
#include <random>

struct IntVector2
{
	int x, y;

	bool operator==(const IntVector2& other) const {
		return x == other.x && y == other.y;
		
	}
};

class Board {
private:
	struct m_Tile
	{
		enum Contents
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

		bool isCovered = false;
		Contents tileContent = EMPTY;
		IntVector2 dimensions = { 20,20 };
		int margin = 40;

		Color colour = RED;
	};

	typedef std::vector<std::vector<m_Tile>> TileGrid;

	TileGrid m_grid;

private:
	int GenerateRandomNumber(int min, int max) {
		// Create a random device and a Mersenne Twister random number generator
		std::random_device rd;
		std::mt19937 gen(rd());

		// Define a distribution in the specified range [min, max]
		std::uniform_int_distribution<int> dis(min, max);

		// Generate and return the random number
		return dis(gen);
	}

	int GetNumberOfBombsAroundPoint(TileGrid board, IntVector2 point)
	{
		int count = 0;

		if (point.y - 1 > 0 && point.x - 1 > 0 && board[point.y - 1][point.x - 1].tileContent == m_Tile::Contents::BOMB) count++; // Top Left

		if (point.y - 1 > 0 && board[point.y - 1][point.x].tileContent == m_Tile::Contents::BOMB) count++; // Top Middle

		if (point.y - 1 > 0 && point.x + 1 < board[point.y].size() && board[point.y - 1][point.x + 1].tileContent == m_Tile::Contents::BOMB) count++; // Top Right


		if (point.x - 1 > 0 && board[point.y][point.x - 1].tileContent == m_Tile::Contents::BOMB) count++; // Middle Left

		if (point.x + 1 < board[point.y].size() && board[point.y][point.x + 1].tileContent == m_Tile::Contents::BOMB) count++; // Middle Right


		if (point.y + 1 < board.size() && point.x - 1 > 0 && board[point.y + 1][point.x - 1].tileContent == m_Tile::Contents::BOMB) count++; // Bottem Left

		if (point.y + 1 < board.size() && board[point.y + 1][point.x].tileContent == m_Tile::Contents::BOMB) count++; // Bottem Middle

		if (point.y + 1 < board.size() && point.x + 1 < board[point.y].size() && board[point.y + 1][point.x + 1].tileContent == m_Tile::Contents::BOMB) count++; // Bottem Right


		return count;
	}
	
	TileGrid GenerateBoard(int rows, int columns)
	{
		TileGrid board;

		// Populate board with tiles.
		for (int rows_index = 0; rows_index < rows; rows_index++)
		{
			std::vector<m_Tile> row;

			for (int columns_index = 0; columns_index < columns; columns_index++)
			{
				row.push_back(m_Tile{});
			}

			board.push_back(row);
		}

		// Place bombs on board.

		std::vector<IntVector2> bombCoordinates;

		while (bombCoordinates.size() != rows)
		{
			bool coordExists = false;
			IntVector2 bombCoordinateToAdd = { generateRandomNumber(0, columns - 1), generateRandomNumber(0, rows - 1) };

			for (IntVector2 bombCoordinate : bombCoordinates) {
				coordExists = bombCoordinate == bombCoordinateToAdd;
				if (coordExists) break;
			}

			if (!coordExists) bombCoordinates.push_back(bombCoordinateToAdd);
		}

		for (IntVector2 coordinates : bombCoordinates)
		{
			board[coordinates.y][coordinates.x].tileContent = m_Tile::Contents::BOMB;
		}


		// Place numbers
		for (int y = 0; y < board.size(); y++)
		{
			for (int x = 0; x < board[0].size(); x++)
			{
				int numberOfBombs = getNumberOfBombsAroundPoint(board, IntVector2{ x,y });

				if (numberOfBombs == 1) board[y][x].tileContent = m_Tile::Contents::ONE;
				else if (numberOfBombs == 2) board[y][x].tileContent = m_Tile::Contents::TWO;
				else if (numberOfBombs == 3) board[y][x].tileContent = m_Tile::Contents::THREE;
				else if (numberOfBombs == 4) board[y][x].tileContent = m_Tile::Contents::FOUR;
				else if (numberOfBombs == 5) board[y][x].tileContent = m_Tile::Contents::FIVE;
				else if (numberOfBombs == 6) board[y][x].tileContent = m_Tile::Contents::SIX;
				else if (numberOfBombs == 7) board[y][x].tileContent = m_Tile::Contents::SEVEN;
				else if (numberOfBombs == 8) board[y][x].tileContent = m_Tile::Contents::EIGHT;
			}
		}

		return board;
	}

public:
	Board(IntVector2 dimensions) {
		m_grid = GenerateBoard(dimensions.y, dimensions.x);
	}

	void DisplayGrid() {

		for (int row = 0; row < m_grid.size(); row++) {
			for (int column = 0; column < m_grid[row].size(); column++) {
				if (!m_grid[row][column].isCovered) {
					m_Tile tile = m_grid[row][column];
					DrawRectangle(column * tile.margin, row * tile.margin, tile.dimensions.x, tile.dimensions.y, RED);
				}
			}
		}
	}
};





int main()
{
	InitWindow(800, 600, "Minesweeper");
	SetTargetFPS(60);

	Board board(IntVector2{9,9});

	/*for (int y = 0; y < board.size(); y++)
	{
		for (int x = 0; x < board[y].size(); x++)
		{
			std::cout << board[y][x].tileContent;
		}

		std::cout << "\n";
	}*/

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(PURPLE);

		board.DisplayGrid();


		EndDrawing();
	}

	CloseWindow();
	return 0;
}
