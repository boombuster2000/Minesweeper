#pragma once
#include <vector>
#include <raylib.h>
#include <random>
#include <type_traits>
#include <string>
#include <map>
#include <memory>

struct IntVector2
{
    int x, y;

    bool operator==(const IntVector2& other) const;
};

namespace Gameboard
{

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

    template <typename T> class AssetHandler
    {
    protected:
        std::map<std::string, T> m_assets;
        T (*m_loadCallback)(const char* fileName); // Function pointer as different for each data type
        void (*m_unloadCallback)(T); // Function pointer as different for each data type

    public:
        AssetHandler(T(*loadCallback)(const char* filePath), void (*unloadCallback)(T))
            : m_loadCallback(loadCallback), m_unloadCallback(unloadCallback)
        {
        }
        ~AssetHandler()
        {
            UnloadAll();
        }


        void LoadAll(FilePathList assetsFilePaths)
        {
            for (int i = 0; i < assetsFilePaths.count; i++)
            {
                std::string currentFilePath = assetsFilePaths.paths[i];

                if (IsPathFile(currentFilePath.c_str()))
                {
                    m_assets.insert({ GetFileNameWithoutExt(currentFilePath.c_str()), m_loadCallback(currentFilePath.c_str()) });
                }
            }
        }
        std::shared_ptr<T> Get(std::string id)
        {
            return std::make_shared<T>(m_assets.at(id));
        }

        void UnloadAll()
        {
            for (auto asset : m_assets)
            {
                m_unloadCallback(asset.second);
            }

            m_assets.clear();
        } 

    };
    
    class AssetsHandler
    {
    public:
     
        AssetHandler<Texture2D> textures = AssetHandler<Texture2D>(LoadTexture, UnloadTexture);
        AssetHandler<Font> fonts = AssetHandler<Font>(LoadFont, UnloadFont);
        AssetHandler<Sound> sounds = AssetHandler<Sound>(LoadSound, UnloadSound);

    public:
        AssetsHandler();
        ~AssetsHandler()
        {
            textures.UnloadAll();
            fonts.UnloadAll();
            sounds.UnloadAll();
        }
    };

    class Drawable
    {
    protected:
        IntVector2 m_margin = { 0,0 };
        IntVector2 m_dimensions = { 0,0 };
        IntVector2 m_positionOnScreen = { 0,0 };
        IntVector2 m_coords = { 0,0 };

    public:
        Drawable(const IntVector2 dimensions, const IntVector2 margin);

        virtual void Render() const = 0;

        virtual int GetWidth() const;
        virtual void SetWidth(const int width);

        virtual int GetHeight() const;
        virtual void SetHeight(const int height);

        virtual int GetMarginWidth() const;
        virtual void SetMarginWidth(const int marginWidth);

        virtual int GetMarginHeight() const;
        virtual void SetMarginHeight(const int marginHeight);

        virtual void SetPositionOnScreen(const int x, const int y);
        virtual IntVector2 GetPositionOnScreen() const;

        virtual void SetGridCoords(const IntVector2 coords);
        virtual IntVector2 GetGridCoords() const;
    };

    class DrawableTexture : public Drawable
    {
    private:
        std::shared_ptr<Texture2D> m_renderedTexture;

    public:
        DrawableTexture(std::shared_ptr<Texture2D> texture, const IntVector2 pixalDimensions, const IntVector2 margin);

        std::shared_ptr<Texture2D> GetTexture() const;
        void SetTexture(std::shared_ptr<Texture2D> texture);

        void Render() const override;
    };

    class Text : public Drawable
    {
    private:

        std::string m_text;
        int m_fontSize;
        std::shared_ptr<Font> m_font;
        Color m_colour;
        AnchorPoints m_anchorPoint = AnchorPoints::TOP_LEFT;

    public:
        Text(std::string text, int fontSize, Color colour, std::shared_ptr<Font> font = std::make_shared<Font>(GetFontDefault()));

        std::string GetText() const;
        void SetText(const std::string text);

        int GetFontSize() const;
        void SetFontSize(const int fontSize);

        int GetWidth() const override;
        int GetHeight() const override;

        AnchorPoints GetAnchorPoint() const;
        void SetAnchorPoint(const AnchorPoints anchorpoint);

        void SetPositionOnScreen(int x, int y) override;

        Color GetColour() const;
        void SetColour(const Color colour);

        void Render() const override;
    };

    template <typename T_Entity = Drawable>
    class Grid
    {
    protected:
        AnchorPoints m_anchorPoint = AnchorPoints::TOP_LEFT;
        typedef std::vector<std::vector<T_Entity>> T_Grid;
        T_Grid m_grid;

    protected:
        T_Grid GenerateBoard(const IntVector2 dimensions, T_Entity sampleSquare)
        {
            static_assert(std::is_base_of<Drawable, T_Entity>::value, "T_Entity must derive from Gameboard::Drawable");

            if (dimensions.x <= 0 || dimensions.y <= 0) {
                throw std::invalid_argument("Board dimensions must be positive");
            }

            T_Grid board;

            // Populate board with tiles.
            for (int y = 0; y < dimensions.y; y++)
            {
                std::vector<T_Entity> row;

                for (int x = 0; x < dimensions.x; x++)
                {
                    sampleSquare.SetGridCoords(IntVector2{ x, y });
                    row.push_back(sampleSquare);
                }

                board.push_back(row);
            }

            return board;
        }

        IntVector2 GetBoardPixelDimensions() const
        {
            if (m_grid.empty() || m_grid[0].empty()) {
                throw std::runtime_error("Empty grid");
            }

            IntVector2 dimensions = {};
            T_Entity square = m_grid[0][0];

            dimensions.x = (int)m_grid[0].size() * square.GetWidth() + ((int)m_grid[0].size() - 1) * square.GetMarginWidth();
            dimensions.y = (int)m_grid.size() * square.GetHeight() + ((int)m_grid.size() - 1) * square.GetMarginHeight();

            return dimensions;
        }
        
        virtual bool ShouldRenderEntity(const IntVector2 coords) const
        {
            return true;
        }
    
    public:
        Grid(const IntVector2 dimensions, const T_Entity sampleEntity, const AnchorPoints anchorPoint, const IntVector2 position)
        {
            m_grid = GenerateBoard(dimensions, sampleEntity);
            SetAnchorPoint(anchorPoint);
            SetPositionsOnScreen(position);
        }
        
        void SetAnchorPoint(const AnchorPoints anchorPoint) {
            m_anchorPoint = anchorPoint;
        }        
        
        void SetPositionsOnScreen(const IntVector2 position)
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
                offset.y = pixelDimensions.y;
                break;

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

                    m_grid[y][x].SetPositionOnScreen(xPos, yPos);
                }
            }
        }

        void DisplayGrid() const
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

        std::vector<T_Entity> GetNeighbours(const T_Entity& tile) const
        {
            std::vector<T_Entity> neighbours;
            int x = tile.GetGridCoords().x;
            int y = tile.GetGridCoords().y;

            const int dx[] = { -1, -1, -1,  0, 0,  1, 1, 1 };
            const int dy[] = { -1,  0,  1, -1, 1, -1, 0, 1 };

            for (int i = 0; i < 8; ++i)
            {
                int newX = x + dx[i];
                int newY = y + dy[i];
                if (newX >= 0 && newX < m_grid[0].size() &&
                    newY >= 0 && newY < m_grid.size())
                {
                    neighbours.push_back(m_grid[newY][newX]);
                }
            }

            return neighbours;
        }
    };
};
