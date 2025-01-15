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
	private:
		Texture2D m_renderedTexture;
		IntVector2 m_margin = { 0,0 };
		IntVector2 m_dimensions = { 0,0 };
		IntVector2 m_positionOnScreen = { 0,0 };
	
	public:
		Drawable(const char* textureFilePath, IntVector2 dimensions, IntVector2 margin)
		{
			m_renderedTexture = LoadTexture(textureFilePath);
			m_dimensions = dimensions;
			m_margin = margin;
		}

		virtual Texture2D GetTexutre() const
		{
			return m_renderedTexture;
		}

		virtual void SetTexture(const char* textureFilePath)
		{
			m_renderedTexture = LoadTexture(textureFilePath);
		}

		virtual void Render() const
		{
			IntVector2 positionOnScreen = GetPositionOnScreen();
			float scale = m_dimensions.y / m_renderedTexture.height;

			DrawTextureEx(m_renderedTexture, {(float)positionOnScreen.x, (float)positionOnScreen.y}, 0, scale, WHITE);
		};

		virtual int GetWidth() const
		{
			return m_dimensions.x;
		}

		virtual void SetWidth(int width)
		{
			m_dimensions.x = width;
		};

		virtual int GetHeight()
		{
			return m_dimensions.y;
		}
		virtual void SetHeight(int height)
		{
			m_dimensions.y = height;
		}

		virtual int GetMarginWidth() const
		{
			return m_margin.x;
		}

		virtual void SetMarginWidth(int marginWidth)
		{
			m_margin.x = marginWidth;
		}

		virtual int GetMarginHeight() const
		{
			return m_margin.y;
		}

		virtual void SetMarginHeight(int marginHeight)
		{
			m_margin.y = marginHeight;
		}

		virtual void SetPositionOnScreen(int y, int x)
		{
			m_positionOnScreen = { x,y };
		};

		virtual IntVector2 GetPositionOnScreen() const
		{
			return m_positionOnScreen;
		}
	};

	template <typename T_Entity = Drawable >
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
		typedef std::vector<std::vector<T_Entity>> T_Grid;
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

		T_Grid GenerateBoard(IntVector2 dimensions, T_Entity sampleSquare)
		{
			static_assert(std::is_base_of<Drawable, T_Entity>::value, "T_Entity must derive from Grid::Drawable");

			if (dimensions.x <= 0 || dimensions.y <= 0) {
				throw std::invalid_argument("Board dimensions must be positive");
			}

			T_Grid board;

			// Populate board with tiles.
			for (int rows_index = 0; rows_index < dimensions.y; rows_index++)
			{
				std::vector<T_Entity> row;

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
			T_Entity square = m_grid[0][0];

			dimensions.x = (m_grid[0].size() - 1) * (square.GetWidth() + square.GetMarginWidth());
			dimensions.y = (m_grid.size() - 1) * (square.GetWidth() + square.GetMarginHeight());

			dimensions.x += square.GetWidth();
			dimensions.y += square.GetHeight();

			return dimensions;
		}

		virtual bool ShouldRenderEntity(IntVector2 coords)
		{
			return true;
		}

	public:
		Grid(IntVector2 dimensions, T_Entity sampleEntity, AnchorPoints anchorPoint, IntVector2 position)
		{
			m_grid = GenerateBoard(dimensions, sampleEntity);
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
					T_Entity entity = m_grid[y][x];
					int xPos = (x * (entity.GetMarginWidth() + entity.GetWidth())) + position.x - offset.x;
					int yPos = (y * (entity.GetMarginHeight() + entity.GetHeight())) + position.y - offset.y;

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
					if (ShouldRenderEntity(IntVector2{ x, y }))
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