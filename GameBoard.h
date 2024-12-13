#pragma once
#include <vector>
#include <raylib.h>
#include <random>
#include <type_traits>

struct IntVector2
{
	int x, y;

	bool operator==(const IntVector2& other) const {
		return x == other.x && y == other.y;

	}
};

class GameBoard
{
public:
	static struct Square
	{
		IntVector2 dimensions;
		int margin;
		Color colour;

		Square(IntVector2 dimensions_, int margin_, Color colour_)
			: dimensions(dimensions_), margin(margin_), colour(colour_)
		{
		};

	};

	static enum AnchorPoints
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

protected:

	AnchorPoints m_anchorPoint = AnchorPoints::TOP_LEFT;

	typedef std::vector<std::vector<Square>> SquareGrid;
	SquareGrid m_grid;

protected:
	int GenerateRandomInteger(int min, int max) {
		// Create a random device and a Mersenne Twister random number generator
		std::random_device rd;
		std::mt19937 gen(rd());

		// Define a distribution in the specified range [min, max]
		std::uniform_int_distribution<int> dis(min, max);

		// Generate and return the random number
		return dis(gen);
	}

	template <typename T_Grid = SquareGrid, typename T_Square = Square>
	T_Grid GenerateBoard(IntVector2 dimensions, T_Square sampleSquare)
	{
		static_assert(std::is_base_of<Square, T_Square>::value, "T_Square must derive from Grid::Square");

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

		dimensions.x = (m_grid[0].size() - 1) * (tile.dimensions.x + tile.margin);
		dimensions.y = (m_grid.size() - 1) * (tile.dimensions.y + tile.margin);

		dimensions.x += tile.dimensions.x;
		dimensions.y += tile.dimensions.y;

		return dimensions;
	}

	virtual bool ShouldRenderSquare(IntVector2 coords)
	{
		return true;
	}

public:
	GameBoard(IntVector2 dimensions, Square sampleSquare):m_grid(GenerateBoard(dimensions, sampleSquare))
	{
	}

	void SetAnchorPoint(AnchorPoints anchorPoint) {
		m_anchorPoint = anchorPoint;
	}

	void DisplayGrid(IntVector2 position)
	{
		IntVector2 offset = { 0,0 }; // Default for top left anchor point;
		IntVector2 pixelDimensions = GetBoardPixelDimensions();

		switch (m_anchorPoint)
		{
		case TOP_MIDDLE: offset.x = int(pixelDimensions.x / 2); break;
		case TOP_RIGHT: offset.x = pixelDimensions.x; break;
		case MIDDLE_LEFT: offset.y = int(pixelDimensions.y / 2); break;

		case MIDDLE: 
			offset.x = int(pixelDimensions.x / 2); 
			offset.y = int(pixelDimensions.y / 2); 
			break;

		case MIDDLE_RIGHT: 
			offset.x = pixelDimensions.x;
			offset.y = int(pixelDimensions.y / 2);
			break;

		case BOTTOM_LEFT: 
			offset.y = pixelDimensions.y; break;

		case BOTTOM_MIDDLE:
			offset.x = int(pixelDimensions.x / 2); 
			offset.y = pixelDimensions.y; 
			break;

		case BOTTOM_RIGHT:
			offset.x = pixelDimensions.x; 
			offset.y = pixelDimensions.y; 
			break;

		default: 
			break;
		}

		for (int row_index = 0; row_index < m_grid.size(); row_index++)
		{
			for (int column_index = 0; column_index < m_grid[row_index].size(); column_index++)
			{
				if (ShouldRenderSquare(IntVector2{ column_index, row_index }))
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