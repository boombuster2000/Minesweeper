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

	struct Square
	{
		IntVector2 dimensions;
		int margin;
		Color colour;
		
		Square(IntVector2 dimensions_, int margin_, Color colour_) 
			: dimensions(dimensions_), margin(margin_), colour(colour_){};

	};

protected:

	typedef std::vector<std::vector<Square>> SquareGrid;
	SquareGrid m_grid;
	AnchorPoints m_anchorPoint = AnchorPoints::TOP_LEFT;

protected:
	int GenerateRandomNumber(int min, int max) {
		// Create a random device and a Mersenne Twister random number generator
		std::random_device rd;
		std::mt19937 gen(rd());

		// Define a distribution in the specified range [min, max]
		std::uniform_int_distribution<int> dis(min, max);

		// Generate and return the random number
		return dis(gen);
	}

	template <typename T_Grid, typename T_Square> 
	T_Grid GenerateBoard(IntVector2 dimensions,T_Square sampleSquare)
	{
		T_Grid board;

		// Populate board with tiles.
		for (int rows_index = 0; rows_index < dimensions.y; rows_index++)
		{
			std::vector<T_Square> row;

			for (int columns_index = 0; columns_index < dimensions.x; columns_index++)
			{
				row.push_back(sampleSquare);
			}

			board.push_back(row);
		}

		return board;
	}

	IntVector2 GetBoardPixelDimensions()
	{
		IntVector2 dimensions;
		Square tile = m_grid[0][0];

		dimensions.x = (m_grid[0].size()-1) * (tile.dimensions.x + tile.margin);
		dimensions.y = (m_grid.size()-1) * (tile.dimensions.y + tile.margin);

		dimensions.x += tile.dimensions.x;
		dimensions.y += tile.dimensions.y;
		
		return dimensions;
	}

	virtual bool ShouldRenderSquare(IntVector2 coords)
	{
		return true;
	}

public:
	Board(IntVector2 dimensions, Square sampleSquare) {
		m_grid = GenerateBoard<SquareGrid, Square>(dimensions, sampleSquare);
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
				if (ShouldRenderSquare(IntVector2{column_index, row_index}))
				{
					Square tile = m_grid[row_index][column_index];
					int xPos = (column_index * (tile.margin + tile.dimensions.x)) + position.x - offset.x;
					int yPos = (row_index * (tile.margin + tile.dimensions.y)) + position.y - offset.y;

					DrawRectangle(xPos, yPos, tile.dimensions.x, tile.dimensions.y, tile.colour);
				}
			}
		}
	}
};

class Minesweeper : public Board {

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

		Tile(const Board::Square& square)
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
			IntVector2 bombCoordinateToAdd = { GenerateRandomNumber(0, board[0].size() - 1), GenerateRandomNumber(0, board.size() - 1)};

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
	Minesweeper(IntVector2 dimensions, Board::Square sampleTile) : Board(dimensions, sampleTile)
	{
		m_grid = GenerateBoard<TileGrid, Tile>(IntVector2{ 9,9 }, sampleTile);
	} 
};

int main()
{
	InitWindow(800, 600, "Minesweeper");
	SetTargetFPS(60);

	Board::Square sampleTile({ 40,40 }, 10, RED);

	Minesweeper board(IntVector2{ 9,9 }, sampleTile);
	board.SetAnchorPoint(Board::AnchorPoints::MIDDLE);

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
