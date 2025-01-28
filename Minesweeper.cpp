#include "raylib.h"
#include "GameBoard.h"
#include <vector>
#include <random>
#include <set>
#include <string>

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
		
		struct
		{
			const char* bomb = "./resources/textures/bomb.png";
			const char* coveredTile = "./resources/textures/covered-tile.png";
			const char* emptyTile = "./resources/textures/empty-tile.png";
			const char* flag = "./resources/textures/flag.png";
			const char* one = "./resources/textures/one.png";
			const char* two = "./resources/textures/two.png";
			const char* three = "./resources/textures/three.png";
			const char* four = "./resources/textures/four.png";
			const char* five = "./resources/textures/five.png";
			const char* six = "./resources/textures/six.png";
			const char* seven = "./resources/textures/seven.png";
			const char* eight = "./resources/textures/eight.png";
		} contentTexturesFilePaths;
	
	private:
		ContentOption m_entityOption = ContentOption::EMPTY;
		const char* m_contentTextureFilePath = contentTexturesFilePaths.emptyTile;
		const Texture2D m_flagTexture = LoadTexture(contentTexturesFilePaths.flag);
		bool m_isCovered = true;
		bool m_isFlagged = false;
		

	public:
		Tile(const IntVector2 dimensions, const IntVector2 margin)
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
				m_contentTextureFilePath = contentTexturesFilePaths.bomb;
				break;
			case ContentOption::ONE:
				m_contentTextureFilePath = contentTexturesFilePaths.one;
				break;
			case ContentOption::TWO:
				m_contentTextureFilePath = contentTexturesFilePaths.two;
				break;
			case ContentOption::THREE:
				m_contentTextureFilePath = contentTexturesFilePaths.three;
				break;
			case ContentOption::FOUR:
				m_contentTextureFilePath = contentTexturesFilePaths.four;
				break;
			case ContentOption::FIVE:
				m_contentTextureFilePath = contentTexturesFilePaths.five;
				break;
			case ContentOption::SIX:
				m_contentTextureFilePath = contentTexturesFilePaths.six;
				break;
			case ContentOption::SEVEN:
				m_contentTextureFilePath = contentTexturesFilePaths.seven;
				break;
			case ContentOption::EIGHT:
				m_contentTextureFilePath = contentTexturesFilePaths.eight;
				break;
			default:
				break;
			}
		}

		const char* GetContentTextureFilePath() const
		{
			return m_contentTextureFilePath;
		}
		void SetContentTextureFilePath(const char* textureFilePath)
		{
			m_contentTextureFilePath = textureFilePath;
		}
	
		bool IsTileCovered() const
		{
			return m_isCovered;
		}
		void ToggleCovered()
		{
			m_isCovered = !m_isCovered;
		}
	
		bool IsTileFlagged() const
		{
			return m_isFlagged;
		}
		void ToggleFlag()
		{
			m_isFlagged = !m_isFlagged;
		}
		
		void Render() const override
		{
			IntVector2 positionOnScreen = GetPositionOnScreen();
			float scale = GetHeight() / GetTexture().height;

			DrawTextureEx(GetTexture(), {(float)positionOnScreen.x, (float)positionOnScreen.y}, 0, scale, WHITE);

			if (IsTileFlagged() && IsTileCovered())
			{
				DrawTextureEx(m_flagTexture, { (float)positionOnScreen.x, (float)positionOnScreen.y }, 0, scale, WHITE);
			}
		}

	};
	
	class MinesweeperGrid : public GameBoard::Grid<Tile> 
	{

	private:
	
		typedef std::vector<std::vector<Tile>> TileGrid;

		float m_bombDensity = 0.15f;
		int m_numberOfBombs;
		int m_numberOfBombsLeft;
		int m_numberOfFlagsLeft;

		bool m_isBombTriggered = false;

	private:

		int GetNumberOfBombsAroundTile(const Tile& tile) const
		{
			int count = 0;
			const std::vector<Tile> neighbours = GetNeighbours(tile);

			for (auto const &neightbour: neighbours)
			{
				if (neightbour.GetContentOption() == Tile::ContentOption::BOMB) count++;
			}

			return count;
		}

		void PlaceBombsOnBoard() {

			std::set<std::pair<int, int>> bombCoordinates;
			while (bombCoordinates.size() < m_numberOfBombs) {
				bombCoordinates.insert({
					GenerateRandomInteger(0, m_grid[0].size() - 1),
					GenerateRandomInteger(0, m_grid.size() - 1)
				});
			}

			for (const auto& [x, y] : bombCoordinates) {
				m_grid[y][x].SetContentOption(Tile::ContentOption::BOMB);
				m_grid[y][x].SetContentTextureFilePath("./resources/textures/bomb.png");
			}

			// Simplified number assignment
			for (int y = 0; y < m_grid.size(); y++) {
				for (int x = 0; x < m_grid[0].size(); x++) {
					if (m_grid[y][x].GetContentOption() != Tile::ContentOption::BOMB) {
						int bombCount = GetNumberOfBombsAroundTile(m_grid[y][x]);
						m_grid[y][x].SetContentOption(static_cast<Tile::ContentOption>(bombCount));
					}
				}
			}
		}

		void ClearEmptyNeighbours(const Tile& homeTile)
		{
			std::vector<Tile> neighbours = GetNeighbours(homeTile);
			std::vector<Tile> clearedNeighbours;

			for (auto &neighbour : neighbours)
			{
				if (neighbour.GetContentOption() == Tile::ContentOption::BOMB) continue;
				if (!neighbour.IsTileCovered()) continue;
				
				IntVector2 coords = neighbour.GetCoords();
				Tile& tile = m_grid[coords.y][coords.x];

				tile.SetTexture(neighbour.GetContentTextureFilePath());
				tile.ToggleCovered();

				clearedNeighbours.push_back(neighbour);
			}

			for (auto& clearedNeighbour : clearedNeighbours)
			{
				if (clearedNeighbour.GetContentOption() != Tile::ContentOption::EMPTY) continue;
				ClearEmptyNeighbours(clearedNeighbour); // Need to coords of tile.
			}
		}

		bool IsTileUnderMouse(const Tile& tile, const Vector2& mousePosition) const {
			const IntVector2 pos = tile.GetPositionOnScreen();
			return 
				pos.x < mousePosition.x && pos.x + tile.GetWidth() > mousePosition.x && 
				pos.y < mousePosition.y && pos.y + tile.GetHeight() > mousePosition.y;

		}

		void HandleRightClick(Tile& tile) {
			if (!tile.IsTileCovered()) return;

			if (tile.IsTileFlagged())
			{
				if (tile.GetContentOption() == Tile::ContentOption::BOMB) m_numberOfBombsLeft += 1;
				m_numberOfFlagsLeft += 1;
			}
			else
			{
				if (tile.GetContentOption() == Tile::ContentOption::BOMB) m_numberOfBombsLeft -= 1;
				m_numberOfFlagsLeft -= 1;
			}
			

			tile.ToggleFlag();
			
		}

		void HandleLeftClick(Tile& tile) {
			if (!tile.IsTileCovered() || tile.IsTileFlagged()) return;
			
			tile.ToggleCovered();
			tile.SetTexture(tile.GetContentTextureFilePath());
			
			switch (tile.GetContentOption()) {
				case Tile::ContentOption::EMPTY:
					ClearEmptyNeighbours(tile);
					break;
				case Tile::ContentOption::BOMB:
					m_isBombTriggered = true;
					break;
				default:
					break;
			}
		}

	public:
		MinesweeperGrid(const IntVector2 dimensions, const Minesweeper::Tile sampleTile, const GameBoard::Grid<Tile>::AnchorPoints anchorPoint, const IntVector2 position)
			: Grid(dimensions, sampleTile, anchorPoint, position)
		{
			m_numberOfBombs = static_cast<int>(dimensions.y * dimensions.x * m_bombDensity);
			m_numberOfBombsLeft = m_numberOfBombs;
			m_numberOfFlagsLeft = m_numberOfBombs;

			PlaceBombsOnBoard();
		}

		void ProcessMouseInput() override
		{
			if (!(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))) return;

			Vector2 mousePosition = GetMousePosition();

			for (auto& row : m_grid)
			{
				for (auto& tile : row)
				{
					if (!IsTileUnderMouse(tile, mousePosition)) continue;

					if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) HandleLeftClick(tile);
					else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) HandleRightClick(tile); 
					
					break;
				}
			}
		}
	
		bool IsBombTriggered() const
		{
			return m_isBombTriggered;
		}

		int GetNumberOfFlagsLeft() const
		{
			return m_numberOfFlagsLeft;
		}
	
		int GetNumberOfBombsLeft() const
		{
			return m_numberOfBombsLeft;
		}
};
};

int main()
{
	InitWindow(800, 600, "Minesweeper");
	SetTargetFPS(60);

	Minesweeper::Tile sampleTile(IntVector2{ 40,40 }, IntVector2{ 10,10 });

	Minesweeper::MinesweeperGrid game(
		IntVector2{ 9,9 }, 
		sampleTile, 
		GameBoard::Grid<Minesweeper::Tile>::AnchorPoints::MIDDLE,
		IntVector2{ GetScreenWidth() / 2,GetScreenHeight() / 2 }
	);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);

		game.DisplayGrid();

		const int numberOfFlagsLeft = game.GetNumberOfFlagsLeft();
		const std::string numberOfFlagsLeftString = std::to_string(numberOfFlagsLeft);
		DrawText(numberOfFlagsLeftString.c_str(), GetScreenWidth() - 100, GetScreenHeight() - 100, 50, RED);

		if (game.GetNumberOfBombsLeft() == 0)
		{
			DrawText("You found all the bombs!", 20, 10, 50, YELLOW);
		}
		else if (!game.IsBombTriggered())
		{
			game.ProcessMouseInput();
		}
		else
		{
			DrawText("You triggered a bomb!", 20, 10, 50, YELLOW);
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
