#include "gameboard.h"
using namespace Gameboard;

bool IntVector2::operator==(const IntVector2& other) const
{
    return x == other.x && y == other.y;
}

Drawable::Drawable(const IntVector2 dimensions, const IntVector2 margin)
    : m_dimensions(dimensions), m_margin(margin)
{
}

int Drawable::GetWidth() const
{
    return m_dimensions.x;
}

void Drawable::SetWidth(const int width)
{
    m_dimensions.x = width;
}

int Drawable::GetHeight() const
{
    return m_dimensions.y;
}

void Drawable::SetHeight(const int height)
{
    m_dimensions.y = height;
}

int Drawable::GetMarginWidth() const
{
    return m_margin.x;
}

void Drawable::SetMarginWidth(const int marginWidth)
{
    m_margin.x = marginWidth;
}

int Drawable::GetMarginHeight() const
{
    return m_margin.y;
}

void Drawable::SetMarginHeight(const int marginHeight)
{
    m_margin.y = marginHeight;
}

void Drawable::SetPositionOnScreen(const int x, const int y)
{
    m_positionOnScreen = { x, y };
}

IntVector2 Drawable::GetPositionOnScreen() const
{
    return m_positionOnScreen;
}

void Drawable::SetGridCoords(const IntVector2 coords)
{
    m_coords = coords;
}

IntVector2 Drawable::GetGridCoords() const
{
    return m_coords;
}

DrawableTexture::DrawableTexture(const char* textureFilePath, const IntVector2 pixalDimensions, const IntVector2 margin)
    : Drawable(pixalDimensions, margin), m_renderedTexture(LoadTexture(textureFilePath))
{
}

Texture2D DrawableTexture::GetTexture() const
{
    return m_renderedTexture;
}

void DrawableTexture::SetTexture(const char* textureFilePath)
{
    if (!FileExists(textureFilePath)) throw std::invalid_argument("File does not exist.");

    m_renderedTexture = LoadTexture(textureFilePath);
}

void DrawableTexture::Render() const
{
    IntVector2 positionOnScreen = GetPositionOnScreen();
    float scaleY = (float)GetHeight() / (float)m_renderedTexture.height;
    float scaleX = (float)GetWidth() / (float)m_renderedTexture.width;

    float scale = std::min(scaleX, scaleY);

    DrawTextureEx(m_renderedTexture, { (float)positionOnScreen.x, (float)positionOnScreen.y }, 0, scale, WHITE);
}

Text::Text(std::string text, int fontSize, Color colour)
    : Drawable(IntVector2{ MeasureText(text.c_str(), fontSize), 10 }, IntVector2{ 0,0 }),
    m_text(text), m_fontSize(fontSize), m_colour(colour)
{
}

std::string Text::GetText() const
{
    return m_text;
}

void Text::SetText(const std::string text)
{
    m_text = text;
}

int Text::GetFontSize() const
{
    return m_fontSize;
}

void Text::SetFontSize(const int fontSize)
{
    m_fontSize = fontSize;
}

int Text::GetWidth() const
{
    return (int)MeasureTextEx(m_font, m_text.c_str(), (float)m_fontSize, 0).x;
}

int Text::GetHeight() const
{
    return (int)MeasureTextEx(m_font, m_text.c_str(), (float)m_fontSize, 0).y;
}

AnchorPoints Text::GetAnchorPoint() const
{
    return m_anchorPoint;
}

void Text::SetAnchorPoint(const AnchorPoints anchorpoint)
{
    m_anchorPoint = anchorpoint;
    SetPositionOnScreen(m_positionOnScreen.x, m_positionOnScreen.y);
}

void Text::SetPositionOnScreen(int x, int y)
{
    const int height = GetHeight();
    const int width = GetWidth();
    IntVector2 offset = { 0,0 };

    switch (m_anchorPoint)
    {
    case Gameboard::TOP_MIDDLE:
        offset.x = width / 2;
        break;

    case Gameboard::TOP_RIGHT:
        offset.x = width;
        break;

    case Gameboard::MIDDLE_LEFT:
        offset.y = height / 2;
        break;

    case Gameboard::MIDDLE:
        offset.x = width / 2;
        offset.y = height / 2;
        break;

    case Gameboard::MIDDLE_RIGHT:
        offset.x = width;
        offset.y = height / 2;
        break;

    case Gameboard::BOTTOM_LEFT:
        offset.y = height;
        break;

    case Gameboard::BOTTOM_MIDDLE:
        offset.x = width / 2;
        offset.y = height;
        break;

    case Gameboard::BOTTOM_RIGHT:
        offset.x = width;
        offset.y = height;
        break;

    default:
        break;
    }

    x -= offset.x;
    y -= offset.y;

    m_positionOnScreen = { x,y };
}

Color Text::GetColour() const
{
    return m_colour;
}

void Text::SetColour(const Color colour)
{
    m_colour = colour;
}

void Text::Render() const
{
    DrawText(m_text.c_str(), m_positionOnScreen.x, m_positionOnScreen.y, m_fontSize, m_colour);
}
