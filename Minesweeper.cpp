#include "raylib.h"
#include "GameBoard.h"
#include <vector>
#include <random>
#include <set>

class Minesweeper
{
	

public:
	class Tile : public GameBoard::Drawable
	{

	public:

		enum class ContentOption : int
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
		
		struct ContentTexturesFilePaths
		{
			const char* bomb = "./resources/textures/bomb.png";
			const char* coveredTile = "./resources/textures/covered-tile.png";
			const char* emptyTile = "./resources/textures/empty-tile.png";
			const char* flag = "./resources/textures/flag.png";
		};
	
	private:
		ContentOption m_entityOption = ContentOption::EMPTY;

	public:
		bool m_isCovered = true;
		const char* m_contentTexture = "./resources/textures/empty-tile.png";
		
	public:
		Tile(IntVector2 dimensions, IntVector2 margin)
			:Drawable("./resources/textures/covered-tile.png", dimensions, margin)
		{
		}

		ContentOption GetContentOption() const
		{
			return m_entityOption;
		}
		void SetContentOption(ContentOption contentOption)
		{
			m_entityOption = contentOption;
			
			switch (m_entityOption)
			{
			case ContentOption::BOMB:
				m_contentTexture = "./resources/textures/bomb.png";
				break;
			case ContentOption::ONE:
				m_contentTexture = "./resources/textures/one.png";
				break;
			case ContentOption::TWO:
				m_contentTexture = "./resources/textures/two.png";
				break;
			case ContentOption::THREE:
				m_contentTexture = "./resources/textures/three.png";
				break;
			case ContentOption::FOUR:
				m_contentTexture = "./resources/textures/four.png";
				break;
			case ContentOption::FIVE:
				m_contentTexture = "./resources/textures/five.png";
				break;
			case ContentOption::SIX:
				m_contentTexture = "./resources/textures/six.png";
				break;
			case ContentOption::SEVEN:
				m_contentTexture = "./resources/textures/seven.png";
				break;
			case ContentOption::EIGHT:
				m_contentTexture = "./resources/textures/eight.png";
				break;
			default:
				break;
			}
		}
};
	
	class MinesweeperGrid : public GameBoard::Grid<Tile> 
	{
		typedef std::vector<std::vector<Tile>> TileGrid;

	private:
		int GetNumberOfBombsAroundPoint(TileGrid board, IntVector2 point) const
		{
			int count = 0;

			if (point.y - 1 >= 0 && point.x - 1 >= 0 && board[point.y - 1][point.x - 1].GetContentOption() == Tile::ContentOption::BOMB) count++; // Top Left

			if (point.y - 1 >= 0 && board[point.y - 1][point.x].GetContentOption() == Tile::ContentOption::BOMB) count++; // Top Middle

			if (point.y - 1 >= 0 && point.x + 1 < board[point.y].size() && board[point.y - 1][point.x + 1].GetContentOption() == Tile::ContentOption::BOMB) count++; // Top Right


			if (point.x - 1 >= 0 && board[point.y][point.x - 1].GetContentOption() == Tile::ContentOption::BOMB) count++; // Middle Left

			if (point.x + 1 < board[point.y].size() && board[point.y][point.x + 1].GetContentOption() == Tile::ContentOption::BOMB) count++; // Middle Right


			if (point.y + 1 < board.size() && point.x - 1 >= 0 && board[point.y + 1][point.x - 1].GetContentOption() == Tile::ContentOption::BOMB) count++; // Bottem Left

			if (point.y + 1 < board.size() && board[point.y + 1][point.x].GetContentOption() == Tile::ContentOption::BOMB) count++; // Bottem Middle

			if (point.y + 1 < board.size() && point.x + 1 < board[point.y].size() && board[point.y + 1][point.x + 1].GetContentOption() == Tile::ContentOption::BOMB) count++; // Bottem Right


			return count;
		}

		void PlaceBombsOnBoard() {
			const float BOMB_DENSITY = 0.15f; // 15% of tiles are bombs
			const int numberOfBombs = static_cast<int>(m_grid.size() * m_grid[0].size() * BOMB_DENSITY);

			std::set<std::pair<int, int>> bombCoordinates;
			while (bombCoordinates.size() < numberOfBombs) {
				bombCoordinates.insert({
					GenerateRandomInteger(0, m_grid[0].size() - 1),
					GenerateRandomInteger(0, m_grid.size() - 1)
					});
			}

			for (const auto& [x, y] : bombCoordinates) {
				m_grid[y][x].SetContentOption(Tile::ContentOption::BOMB);
				m_grid[y][x].m_contentTexture = "./resources/textures/bomb.png";
			}

			// Simplified number assignment
			for (int y = 0; y < m_grid.size(); y++) {
				for (int x = 0; x < m_grid[0].size(); x++) {
					if (m_grid[y][x].GetContentOption() != Tile::ContentOption::BOMB) {
						int bombCount = GetNumberOfBombsAroundPoint(m_grid, IntVector2{ x,y });
						m_grid[y][x].SetContentOption(static_cast<Tile::ContentOption>(bombCount));
					}
				}
			}
		}

	public:
		MinesweeperGrid(IntVector2 dimensions, Minesweeper::Tile sampleTile, GameBoard::Grid<Tile>::AnchorPoints anchorPoint, IntVector2 position)
			: Grid(dimensions, sampleTile, anchorPoint, position)
		{
			PlaceBombsOnBoard();
		}

		void ProcessMouseInput() override
		{
			if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;

			Vector2 mousePosition = GetMousePosition();

			for (int y = 0; y < m_grid.size(); y++)
			{
				for (int x = 0; x < m_grid[y].size(); x++)
				{
					Tile tile = m_grid[y][x];

					if (tile.GetPositionOnScreen().x < mousePosition.x
						&& tile.GetPositionOnScreen().x + tile.GetWidth() > mousePosition.x
						&& tile.GetPositionOnScreen().y < mousePosition.y
						&& tile.GetPositionOnScreen().y + tile.GetHeight() > mousePosition.y)
					{
						m_grid[y][x].m_isCovered = false;
						
						m_grid[y][x].SetTexture(m_grid[y][x].m_contentTexture);
						break;
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

	Minesweeper::Tile sampleTile(IntVector2{ 40,40 }, IntVector2{ 10,10 });

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
