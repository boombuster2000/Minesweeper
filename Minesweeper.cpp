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

class Board 
{
public:

	enum AnchorPoints 
	{
		TOP_LEFT,
		TOP_MIDDLE,
		TOP_RIGHT,

		MIDDLE_LEFT,
		MIDDLE,
		MIDDLE_RIGHT,

		BOTTOM_LEFT,
		BOTTOM_MIDDLE,
		BOTTOM_RIGHT,	
	};

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
		IntVector2 dimensions = { 50,50 };
		int margin = 10;

		Color colour = RED;
	};

	typedef std::vector<std::vector<m_Tile>> TileGrid;

	TileGrid m_grid;

	AnchorPoints m_anchorPoint = AnchorPoints::TOP_LEFT;


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
	
	TileGrid GenerateBoard(IntVector2 dimensions)
	{
		TileGrid board;

		// Populate board with tiles.
		for (int rows_index = 0; rows_index < dimensions.y; rows_index++)
		{
			std::vector<m_Tile> row;

			for (int columns_index = 0; columns_index < dimensions.x; columns_index++)
			{
				row.push_back(m_Tile{});
			}

			board.push_back(row);
		}

		// Place bombs on board.

		std::vector<IntVector2> bombCoordinates;

		while (bombCoordinates.size() != dimensions.y)
		{
			bool coordExists = false;
			IntVector2 bombCoordinateToAdd = { GenerateRandomNumber(0, dimensions.x - 1), GenerateRandomNumber(0, dimensions.y - 1) };

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
				int numberOfBombs = GetNumberOfBombsAroundPoint(board, IntVector2{ x,y });

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

	IntVector2 GetBoardPixelDimensions()
	{
		IntVector2 dimensions;
		m_Tile tile = m_grid[0][0];

		dimensions.x = (m_grid[0].size()-1) * (tile.dimensions.x + tile.margin);
		dimensions.y = (m_grid.size()-1) * (tile.dimensions.y + tile.margin);

		dimensions.x += tile.dimensions.x;
		dimensions.y += tile.dimensions.y;
		
		return dimensions;
	}

public:
	Board(IntVector2 dimensions) {
		m_grid = GenerateBoard(dimensions);
	}

	void SetAnchorPoint(AnchorPoints anchorPoint) {
		m_anchorPoint = anchorPoint;
	}

	void DisplayGrid(IntVector2 position) 
	{
		IntVector2 offset = { 0,0 }; // Default for top left anchor point;
		IntVector2 pixelDimensions = GetBoardPixelDimensions();

		if (m_anchorPoint == TOP_MIDDLE) offset.x = int(pixelDimensions.x / 2);
		else if (m_anchorPoint == TOP_RIGHT) offset.x = pixelDimensions.x;

		else if (m_anchorPoint == MIDDLE_LEFT) offset.y = int(pixelDimensions.y / 2);
		else if (m_anchorPoint == MIDDLE) {
			offset.x = int(pixelDimensions.x / 2);
			offset.y = int(pixelDimensions.y / 2);
		}
		else if (m_anchorPoint == MIDDLE_RIGHT) {
			offset.x = pixelDimensions.x;
			offset.y = int(pixelDimensions.y / 2);
		}

		else if (m_anchorPoint == BOTTOM_LEFT) offset.y = pixelDimensions.y;
		else if (m_anchorPoint == BOTTOM_MIDDLE)
		{
			offset.x = int(pixelDimensions.x / 2);
			offset.y = pixelDimensions.y;
		}
		else if (m_anchorPoint == BOTTOM_RIGHT) {
			offset.x = pixelDimensions.x;
			offset.y = pixelDimensions.y;
		}


		for (int row_index = 0; row_index < m_grid.size(); row_index++) 
		{
			for (int column_index = 0; column_index < m_grid[row_index].size(); column_index++)
			{
				if (!m_grid[row_index][column_index].isCovered)
				{
					m_Tile tile = m_grid[row_index][column_index];
					int xPos = (column_index * (tile.margin + tile.dimensions.x)) + position.x - offset.x;
					int yPos = (row_index * (tile.margin + tile.dimensions.y)) + position.y - offset.y;

					DrawRectangle(xPos, yPos, tile.dimensions.x, tile.dimensions.y, RED);
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
	board.SetAnchorPoint(Board::AnchorPoints::MIDDLE);

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
		ClearBackground(RAYWHITE);

		board.DisplayGrid(IntVector2{GetScreenWidth()/2,GetScreenHeight()/2});


		EndDrawing();
	}

	CloseWindow();
	return 0;
}
