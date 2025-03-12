#pragma once

#include "raylib.h"
#include "gameboard.h"
#include <vector>
#include <random>
#include <set>
#include <string>
#include <map>
#include <memory>

namespace Minesweeper
{

    extern const std::string texturesBaseFilePath;
    extern const std::map<std::string, std::string> textureFilePaths;
    extern Gameboard::AssetsHandler assetsHandler;


    class Tile : public Gameboard::DrawableTexture
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

    private:
        ContentOption m_entityOption = ContentOption::EMPTY;
        std::shared_ptr<Texture2D> m_contentTexture = assetsHandler.texturesHandler.GetAsset("empty-tile");
        const std::shared_ptr<Texture2D> m_flagTexture = assetsHandler.texturesHandler.GetAsset("flag");
        bool m_isCovered = true;
        bool m_isFlagged = false;

    public:
        Tile(const IntVector2 dimensions, const IntVector2 margin);

        ContentOption GetContentOption() const;
        void SetContentOption(ContentOption contentOption);

        std::shared_ptr<Texture2D> GetContentTexture() const;
        void SetContentTexture(std::shared_ptr<Texture2D> textureFilePath);

        bool IsTileCovered() const;
        void ToggleCovered();

        bool IsTileFlagged() const;
        void ToggleFlag();

        void Render() const override;
    };

    class MinesweeperGrid : public Gameboard::Grid<Tile>
    {
    private:
        typedef std::vector<std::vector<Tile>> TileGrid;
        std::set<std::pair<int, int>> m_bombCoordinates;

        float m_bombDensity = 0.15f;
        int m_numberOfBombs;
        int m_numberOfBombsLeft;
        int m_numberOfFlagsLeft;

        bool m_isBombTriggered = false;

    private:
        int GetNumberOfBombsAroundTile(const Tile& tile) const;
        void PlaceBombsOnBoard();
        void ClearEmptyNeighbours(const Tile& homeTile);
        bool IsTileUnderMouse(const Tile& tile, const Vector2& mousePosition) const;
        void HandleRightClick(Tile& tile);
        void HandleLeftClick(Tile& tile);

    public:
        MinesweeperGrid(const IntVector2 dimensions, const Tile sampleTile, const Gameboard::AnchorPoints anchorPoint, const IntVector2 position);

        void ProcessMouseInput() override;

        bool IsBombTriggered() const;
        int GetNumberOfFlagsLeft() const;
        int GetNumberOfBombsLeft() const;
        void DisplayBombs();
    };
};
