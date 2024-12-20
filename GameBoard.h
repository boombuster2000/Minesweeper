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

	class Drawable
	{
	protected:
		virtual void Render() const = 0;
		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;
		virtual int GetMarginWidth() const = 0;
		virtual int GetMarginHeight() const = 0;
		virtual void SetColour(Color colour) = 0;
		virtual Color GetColour() const = 0;
		virtual void SetPositionOnScreen(int y, int x) = 0;
		virtual IntVector2 GetPositionOnScreen() const = 0;
	};

	class Square : Drawable
	{
	private:
		IntVector2 m_dimensions;
		IntVector2 m_positionOnScreen = { 0,0 };
		int m_margin;
		Color m_colour;


	public:
		Square(IntVector2 dimensions, int margin, Color colour)
			: m_dimensions(dimensions), m_margin(margin), m_colour(colour)
		{
		};

		int GetWidth() const override
		{
			return m_dimensions.x;
		}
		int GetHeight() const override
		{
			return m_dimensions.y;
		}
		int GetMarginWidth() const override
		{
			return m_margin;
		}
		int GetMarginHeight() const override
		{
			return m_margin;
		}
		void SetColour(Color colour) override
		{
			m_colour = colour;
		}
		Color GetColour() const override
		{
			return m_colour;
		}
		IntVector2 GetPositionOnScreen() const override
		{
			return m_positionOnScreen;
		}
		void SetPositionOnScreen(int y, int x) override
		{
			m_positionOnScreen.x = x;
			m_positionOnScreen.y = y;
		}
		void Render() const override
		{
			DrawRectangle(GetPositionOnScreen().x, GetPositionOnScreen().y, GetWidth(), GetHeight(), GetColour());
		}
		
	};

	template <typename T_Square=Square>
	class Grid
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

	protected:

		AnchorPoints m_anchorPoint = AnchorPoints::TOP_LEFT;

		typedef std::vector<std::vector<T_Square>> T_Grid;
		T_Grid m_grid;

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

		T_Grid GenerateBoard(IntVector2 dimensions, T_Square sampleSquare)
		{
			static_assert(std::is_base_of<Square, T_Square>::value, "T_Square must derive from Grid::Square");

			if (dimensions.x <= 0 || dimensions.y <= 0) {
				throw std::invalid_argument("Board dimensions must be positive");
			}

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
			if (m_grid.empty() || m_grid[0].empty()) {
				throw std::runtime_error("Empty grid");
			}

			IntVector2 dimensions;
			T_Square square = m_grid[0][0];

			dimensions.x = (m_grid[0].size() - 1) * (square.GetWidth() + square.GetMarginWidth());
			dimensions.y = (m_grid.size() - 1) * (square.GetWidth() + square.GetMarginHeight());

			dimensions.x += square.GetWidth();
			dimensions.y += square.GetHeight();

			return dimensions;
		}

		virtual bool ShouldRenderSquare(IntVector2 coords)
		{
			return true;
		}

	public:
		Grid(IntVector2 dimensions, T_Square sampleSquare, AnchorPoints anchorPoint, IntVector2 position)
		{
			m_grid = GenerateBoard(dimensions, sampleSquare);
			SetAnchorPoint(anchorPoint);
			SetPositionsOnScreen(position);
		}

		void SetAnchorPoint(AnchorPoints anchorPoint) {
			m_anchorPoint = anchorPoint;
		}

		void SetPositionsOnScreen(IntVector2 position)
			// Function adds position of each square on the screen to it's attributes so other methods can access it.
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

			for (int y = 0; y < m_grid.size(); y++)
			{
				for (int x = 0; x < m_grid[y].size(); x++)
				{
					T_Square tile = m_grid[y][x];
					int xPos = (x * (tile.GetMarginWidth() + tile.GetWidth())) + position.x - offset.x;
					int yPos = (y * (tile.GetMarginHeight() + tile.GetHeight())) + position.y - offset.y;

					m_grid[y][x].SetPositionOnScreen(yPos, xPos);
				}
			}
		}

		void DisplayGrid()
		{

			for (int y = 0; y < m_grid.size(); y++)
			{
				for (int x = 0; x < m_grid[y].size(); x++)
				{
					if (ShouldRenderSquare(IntVector2{ x, y }))
					{
						m_grid[y][x].Render();
					}
				}
			}
		}

		virtual void ProcessMouseInput()
		{

		}
	};
};