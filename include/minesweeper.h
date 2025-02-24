#pragma once

#include "raylib.h"
#include "gameboard.h"
#include <vector>
#include <random>
#include <set>
#include <string>

class Minesweeper
{
public:
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

        struct
        {
            const char* bomb = "./resources/textures/bomb.png";
            const char* coveredTile = "./resources/textures/covered-tile.png";
            const char* emptyTile = "./resources/textures/empty-tile.png";
            const char* flag = "./resources/textures/flag.png";
            const char* incorrect = "./resources/textures/incorrect.png";
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
        Tile(const IntVector2 dimensions, const IntVector2 margin);

        ContentOption GetContentOption() const;
        void SetContentOption(ContentOption contentOption);

        const char* GetContentTextureFilePath() const;
        void SetContentTextureFilePath(const char* textureFilePath);

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
        MinesweeperGrid(const IntVector2 dimensions, const Minesweeper::Tile sampleTile, const Gameboard::AnchorPoints anchorPoint, const IntVector2 position);

        void ProcessMouseInput() override;

        bool IsBombTriggered() const;
        int GetNumberOfFlagsLeft() const;
        int GetNumberOfBombsLeft() const;
        void DisplayBombs();
    };
};
