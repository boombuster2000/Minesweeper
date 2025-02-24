#include "minesweeper.h"

Minesweeper::Tile::Tile(const IntVector2 dimensions, const IntVector2 margin)
    : DrawableTexture("./resources/textures/covered-tile.png", dimensions, margin)
{
}

Minesweeper::Tile::ContentOption Minesweeper::Tile::GetContentOption() const
{
    return m_entityOption;
}

void Minesweeper::Tile::SetContentOption(ContentOption contentOption)
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

const char* Minesweeper::Tile::GetContentTextureFilePath() const
{
    return m_contentTextureFilePath;
}

void Minesweeper::Tile::SetContentTextureFilePath(const char* textureFilePath)
{
    m_contentTextureFilePath = textureFilePath;
}

bool Minesweeper::Tile::IsTileCovered() const
{
    return m_isCovered;
}

void Minesweeper::Tile::ToggleCovered()
{
    m_isCovered = !m_isCovered;
}

bool Minesweeper::Tile::IsTileFlagged() const
{
    return m_isFlagged;
}

void Minesweeper::Tile::ToggleFlag()
{
    m_isFlagged = !m_isFlagged;
}

void Minesweeper::Tile::Render() const
{
    IntVector2 positionOnScreen = GetPositionOnScreen();
    float scale = GetHeight() / GetTexture().height;

    DrawTextureEx(GetTexture(), { (float)positionOnScreen.x, (float)positionOnScreen.y }, 0, scale, WHITE);

    if (IsTileFlagged() && IsTileCovered())
    {
        DrawTextureEx(m_flagTexture, { (float)positionOnScreen.x, (float)positionOnScreen.y }, 0, scale, WHITE);
    }
}

Minesweeper::MinesweeperGrid::MinesweeperGrid(const IntVector2 dimensions, const Minesweeper::Tile sampleTile, const Gameboard::AnchorPoints anchorPoint, const IntVector2 position)
    : Grid(dimensions, sampleTile, anchorPoint, position)
{
    m_numberOfBombs = static_cast<int>(dimensions.y * dimensions.x * m_bombDensity);
    m_numberOfBombsLeft = m_numberOfBombs;
    m_numberOfFlagsLeft = m_numberOfBombs;

    PlaceBombsOnBoard();
}

int Minesweeper::MinesweeperGrid::GetNumberOfBombsAroundTile(const Tile& tile) const
{
    int count = 0;
    const std::vector<Tile> neighbours = GetNeighbours(tile);

    for (auto const& neighbour : neighbours)
    {
        if (neighbour.GetContentOption() == Tile::ContentOption::BOMB) count++;
    }

    return count;
}

void Minesweeper::MinesweeperGrid::PlaceBombsOnBoard()
{
    while (m_bombCoordinates.size() < m_numberOfBombs) {
        m_bombCoordinates.insert({
            GetRandomValue(0, m_grid[0].size() - 1),
            GetRandomValue(0, m_grid.size() - 1)
            });
    }

    for (const auto& [x, y] : m_bombCoordinates) {
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

void Minesweeper::MinesweeperGrid::ClearEmptyNeighbours(const Tile& homeTile)
{
    std::vector<Tile> neighbours = GetNeighbours(homeTile);
    std::vector<Tile> clearedNeighbours;

    for (auto& neighbour : neighbours)
    {
        if (neighbour.GetContentOption() == Tile::ContentOption::BOMB) continue;
        if (!neighbour.IsTileCovered()) continue;

        IntVector2 coords = neighbour.GetGridCoords();
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

bool Minesweeper::MinesweeperGrid::IsTileUnderMouse(const Tile& tile, const Vector2& mousePosition) const
{
    const IntVector2 pos = tile.GetPositionOnScreen();
    return
        pos.x < mousePosition.x && pos.x + tile.GetWidth() > mousePosition.x &&
        pos.y < mousePosition.y && pos.y + tile.GetHeight() > mousePosition.y;
}

void Minesweeper::MinesweeperGrid::HandleRightClick(Tile& tile)
{
    if (!tile.IsTileCovered()) return;

    if (tile.IsTileFlagged())
    {
        if (tile.GetContentOption() == Tile::ContentOption::BOMB) m_numberOfBombsLeft += 1;
        m_numberOfFlagsLeft += 1;
    }
    else
    {
        if (m_numberOfFlagsLeft <= 0) return;
        if (tile.GetContentOption() == Tile::ContentOption::BOMB) m_numberOfBombsLeft -= 1;
        m_numberOfFlagsLeft -= 1;
    }

    tile.ToggleFlag();
}

void Minesweeper::MinesweeperGrid::HandleLeftClick(Tile& tile)
{
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

void Minesweeper::MinesweeperGrid::ProcessMouseInput()
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

bool Minesweeper::MinesweeperGrid::IsBombTriggered() const
{
    return m_isBombTriggered;
}

int Minesweeper::MinesweeperGrid::GetNumberOfFlagsLeft() const
{
    return m_numberOfFlagsLeft;
}

int Minesweeper::MinesweeperGrid::GetNumberOfBombsLeft() const
{
    return m_numberOfBombsLeft;
}

void Minesweeper::MinesweeperGrid::DisplayBombs()
{
    for (const auto& [x, y] : m_bombCoordinates) {
        if (m_grid[y][x].IsTileFlagged() && m_grid[y][x].GetContentOption() == Tile::ContentOption::BOMB) continue;
        if (!m_grid[y][x].IsTileCovered()) continue;
        m_grid[y][x].SetTexture(m_grid[y][x].GetContentTextureFilePath());
        m_grid[y][x].ToggleCovered();
    }

    for (auto& row : m_grid)
    {
        for (auto& tile : row)
        {
            int x = tile.GetGridCoords().x;
            int y = tile.GetGridCoords().y;

            if (!tile.IsTileFlagged()) continue;
            if (tile.GetContentOption() == Tile::ContentOption::BOMB) continue;

            m_grid[y][x].SetTexture("./resources/textures/incorrect.png");
            m_grid[y][x].ToggleFlag();
        }
    }

    m_bombCoordinates.clear();
}
