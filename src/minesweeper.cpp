#include "minesweeper.h"
using namespace Minesweeper;

Gameboard::AssetsHandler Minesweeper::assets;

Tile::Tile(const IntVector2 dimensions, const IntVector2 margin)
    : DrawableTexture(assets.textures.Get("covered-tile"), dimensions, margin)
{
}

Tile::ContentOption Tile::GetContentOption() const
{
    return m_entityOption;
}

void Tile::SetContentOption(ContentOption contentOption)
{
    m_entityOption = contentOption;

    switch (m_entityOption)
    {
    case ContentOption::BOMB:
        SetContentTexture(assets.textures.Get("bomb"));
        break;

    case ContentOption::ONE:
        SetContentTexture(assets.textures.Get("one"));
        break;

    case ContentOption::TWO:
        SetContentTexture(assets.textures.Get("two"));
        break;

    case ContentOption::THREE:
        SetContentTexture(assets.textures.Get("three"));
        break;

    case ContentOption::FOUR:
        SetContentTexture(assets.textures.Get("four"));
        break;

    case ContentOption::FIVE:
        SetContentTexture(assets.textures.Get("five"));
        break;

    case ContentOption::SIX:
        SetContentTexture(assets.textures.Get("six"));
        break;

    case ContentOption::SEVEN:
        SetContentTexture(assets.textures.Get("seven"));
        break;

    case ContentOption::EIGHT:
        SetContentTexture(assets.textures.Get("eight"));
        break;

    default:
        break;
    }
}

std::shared_ptr<Texture2D> Tile::GetContentTexture() const
{
    return m_contentTexture;
}

void Tile::SetContentTexture(std::shared_ptr<Texture2D> texture)
{
    m_contentTexture = texture;
}

bool Tile::IsTileCovered() const
{
    return m_isCovered;
}

void Tile::ToggleCovered()
{
    m_isCovered = !m_isCovered;
}

bool Tile::IsTileFlagged() const
{
    return m_isFlagged;
}

void Tile::ToggleFlag()
{
    m_isFlagged = !m_isFlagged;
}

void Tile::Render() const
{
    IntVector2 positionOnScreen = GetPositionOnScreen();
    float scale = (float)GetHeight() / GetTexture()->height;

    DrawTextureEx(*GetTexture(), { (float)positionOnScreen.x, (float)positionOnScreen.y }, 0, scale, WHITE);

    if (IsTileFlagged() && IsTileCovered())
    {
        DrawTextureEx(*m_flagTexture, { (float)positionOnScreen.x, (float)positionOnScreen.y }, 0, scale, WHITE);
    }
}


MinesweeperGrid::MinesweeperGrid(const IntVector2 dimensions, const Tile sampleTile, const Gameboard::AnchorPoints anchorPoint, const IntVector2 position)
    : Grid(dimensions, sampleTile, anchorPoint, position)
{
    m_numberOfBombs = static_cast<int>(dimensions.y * dimensions.x * m_bombDensity);
    m_numberOfBombsLeft = m_numberOfBombs;
    m_numberOfFlagsLeft = m_numberOfBombs;

    PlaceBombsOnBoard();
}

int MinesweeperGrid::GetNumberOfBombsAroundTile(const Tile& tile) const
{
    int count = 0;
    const std::vector<Tile> neighbours = GetNeighbours(tile);

    for (auto const& neighbour : neighbours)
    {
        if (neighbour.GetContentOption() == Tile::ContentOption::BOMB) count++;
    }

    return count;
}

void MinesweeperGrid::PlaceBombsOnBoard()
{
    while (m_bombCoordinates.size() < m_numberOfBombs) {
        m_bombCoordinates.insert({
            GetRandomValue(0, (int)m_grid[0].size() - 1),
            GetRandomValue(0, (int)m_grid.size() - 1)
            });
    }

    for (const auto& [x, y] : m_bombCoordinates) {
        m_grid[y][x].SetContentOption(Tile::ContentOption::BOMB);
        m_grid[y][x].SetContentTexture(assets.textures.Get("bomb"));
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

void MinesweeperGrid::ClearEmptyNeighbours(const Tile& homeTile)
{
    std::vector<Tile> neighbours = GetNeighbours(homeTile);
    std::vector<Tile> clearedNeighbours;

    for (auto& neighbour : neighbours)
    {
        if (neighbour.GetContentOption() == Tile::ContentOption::BOMB) continue;
        if (!neighbour.IsTileCovered()) continue;

        IntVector2 coords = neighbour.GetGridCoords();
        Tile& tile = m_grid[coords.y][coords.x];

        tile.SetTexture(neighbour.GetContentTexture());
        tile.ToggleCovered();

        clearedNeighbours.push_back(neighbour);
    }

    for (auto& clearedNeighbour : clearedNeighbours)
    {
        if (clearedNeighbour.GetContentOption() != Tile::ContentOption::EMPTY) continue;
        ClearEmptyNeighbours(clearedNeighbour); // Need to coords of tile.
    }
}

bool MinesweeperGrid::IsTileUnderMouse(const Tile& tile, const Vector2& mousePosition) const
{
    const IntVector2 pos = tile.GetPositionOnScreen();
    return
        pos.x < mousePosition.x && pos.x + tile.GetWidth() > mousePosition.x &&
        pos.y < mousePosition.y && pos.y + tile.GetHeight() > mousePosition.y;
}

void MinesweeperGrid::HandleRightClick(Tile& tile)
{
    if (!tile.IsTileCovered()) return;

    if (tile.IsTileFlagged()) // Flag Remove
    {
        if (tile.GetContentOption() == Tile::ContentOption::BOMB) m_numberOfBombsLeft += 1;
        m_numberOfFlagsLeft += 1;
        PlaySound(*assets.sounds.Get("flag-up"));

    }
    else // Flag Add
    {
        if (m_numberOfFlagsLeft <= 0) return;
        if (tile.GetContentOption() == Tile::ContentOption::BOMB) m_numberOfBombsLeft -= 1;
        m_numberOfFlagsLeft -= 1;
        PlaySound(*assets.sounds.Get("flag-down"));
    }

    tile.ToggleFlag();
}

void MinesweeperGrid::HandleLeftClick(Tile& tile)
{
    if (!tile.IsTileCovered() || tile.IsTileFlagged()) return;

    tile.ToggleCovered();
    tile.SetTexture(tile.GetContentTexture());
    PlaySound(*assets.sounds.Get("uncover"));

    switch (tile.GetContentOption()) {
    case Tile::ContentOption::EMPTY:
        ClearEmptyNeighbours(tile);
        break;
    case Tile::ContentOption::BOMB:
        m_isBombTriggered = true;
        PlaySound(*assets.sounds.Get("explosion"));
        break;
    default:
        break;
    }
}

void MinesweeperGrid::ProcessMouseInput()
{
    if (!(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))) return;

    Vector2 mousePosition = GetMousePosition();
    bool isClickedTileFound = false;

    for (auto& row : m_grid)
    {
        for (auto& tile : row)
        {
            if (!IsTileUnderMouse(tile, mousePosition)) continue;

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) HandleLeftClick(tile);
            else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) HandleRightClick(tile);

            isClickedTileFound = true;
            break;
        }

        if (isClickedTileFound) break;
    }
}

bool MinesweeperGrid::IsBombTriggered() const
{
    return m_isBombTriggered;
}

int MinesweeperGrid::GetNumberOfFlagsLeft() const
{
    return m_numberOfFlagsLeft;
}

int MinesweeperGrid::GetNumberOfBombsLeft() const
{
    return m_numberOfBombsLeft;
}

void MinesweeperGrid::DisplayBombs()
{
    for (const auto& [x, y] : m_bombCoordinates) {
        if (m_grid[y][x].IsTileFlagged() && m_grid[y][x].GetContentOption() == Tile::ContentOption::BOMB) continue;
        if (!m_grid[y][x].IsTileCovered()) continue;
        m_grid[y][x].SetTexture(m_grid[y][x].GetContentTexture());
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

            m_grid[y][x].SetTexture(assets.textures.Get("incorrect"));
            m_grid[y][x].ToggleFlag();
        }
    }

    m_bombCoordinates.clear();
}
