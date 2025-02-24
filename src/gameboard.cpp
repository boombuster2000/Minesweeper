#include "gameboard.h"

bool IntVector2::operator==(const IntVector2& other) const
{
    return x == other.x && y == other.y;
}

Gameboard::Drawable::Drawable(const IntVector2 dimensions, const IntVector2 margin)
    : m_dimensions(dimensions), m_margin(margin)
{
}

int Gameboard::Drawable::GetWidth() const
{
    return m_dimensions.x;
}

void Gameboard::Drawable::SetWidth(const int width)
{
    m_dimensions.x = width;
}

int Gameboard::Drawable::GetHeight() const
{
    return m_dimensions.y;
}

void Gameboard::Drawable::SetHeight(const int height)
{
    m_dimensions.y = height;
}

int Gameboard::Drawable::GetMarginWidth() const
{
    return m_margin.x;
}

void Gameboard::Drawable::SetMarginWidth(const int marginWidth)
{
    m_margin.x = marginWidth;
}

int Gameboard::Drawable::GetMarginHeight() const
{
    return m_margin.y;
}

void Gameboard::Drawable::SetMarginHeight(const int marginHeight)
{
    m_margin.y = marginHeight;
}

void Gameboard::Drawable::SetPositionOnScreen(const int x, const int y)
{
    m_positionOnScreen = { x, y };
}

IntVector2 Gameboard::Drawable::GetPositionOnScreen() const
{
    return m_positionOnScreen;
}

void Gameboard::Drawable::SetGridCoords(const IntVector2 coords)
{
    m_coords = coords;
}

IntVector2 Gameboard::Drawable::GetGridCoords() const
{
    return m_coords;
}

Gameboard::DrawableTexture::DrawableTexture(const char* textureFilePath, const IntVector2 pixalDimensions, const IntVector2 margin)
    : Drawable(pixalDimensions, margin), m_renderedTexture(LoadTexture(textureFilePath))
{
}

Texture2D Gameboard::DrawableTexture::GetTexture() const
{
    return m_renderedTexture;
}

void Gameboard::DrawableTexture::SetTexture(const char* textureFilePath)
{
    if (!FileExists(textureFilePath)) throw std::invalid_argument("File does not exist.");

    m_renderedTexture = LoadTexture(textureFilePath);
}

void Gameboard::DrawableTexture::Render() const
{
    IntVector2 positionOnScreen = GetPositionOnScreen();
    float scaleY = GetHeight() / m_renderedTexture.height;
    float scaleX = GetWidth() / m_renderedTexture.width;

    float scale = std::min(scaleX, scaleY);

    DrawTextureEx(m_renderedTexture, { (float)positionOnScreen.x, (float)positionOnScreen.y }, 0, scale, WHITE);
}

Gameboard::Text::Text(std::string text, int fontSize, Color colour)
    : Drawable(IntVector2{ MeasureText(text.c_str(), fontSize), 10 }, IntVector2{ 0,0 }),
    m_text(text), m_fontSize(fontSize), m_colour(colour)
{
}

std::string Gameboard::Text::GetText() const
{
    return m_text;
}

void Gameboard::Text::SetText(const std::string text)
{
    m_text = text;
}

int Gameboard::Text::GetFontSize() const
{
    return m_fontSize;
}

void Gameboard::Text::SetFontSize(const int fontSize)
{
    m_fontSize = fontSize;
}

int Gameboard::Text::GetWidth() const
{
    return MeasureTextEx(m_font, m_text.c_str(), m_fontSize, 0).x;
}

int Gameboard::Text::GetHeight() const
{
    return MeasureTextEx(m_font, m_text.c_str(), m_fontSize, 0).y;
}

Gameboard::AnchorPoints Gameboard::Text::GetAnchorPoint() const
{
    return m_anchorPoint;
}

void Gameboard::Text::SetAnchorPoint(const AnchorPoints anchorpoint)
{
    m_anchorPoint = anchorpoint;
    SetPositionOnScreen(m_positionOnScreen.x, m_positionOnScreen.y);
}

void Gameboard::Text::SetPositionOnScreen(int x, int y)
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

Color Gameboard::Text::GetColour() const
{
    return m_colour;
}

void Gameboard::Text::SetColour(const Color colour)
{
    m_colour = colour;
}

void Gameboard::Text::Render() const
{
    DrawText(m_text.c_str(), m_positionOnScreen.x, m_positionOnScreen.y, m_fontSize, m_colour);
}
