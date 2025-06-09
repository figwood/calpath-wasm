#include "Color.h"
Color::Color()
{
    setARGB(0, 0, 0, 0);
}
Color::Color(unsigned int ole_color)
{
    fromOleColor(ole_color, &m_red, &m_green, &m_blue);
}
Color::Color(unsigned char red, unsigned char green, unsigned char blue)
{
    setARGB(0, red, green, blue);
}
Color::Color(unsigned char alpha, unsigned char red, unsigned char green, unsigned char blue)
{
    setARGB(alpha, red, green, blue);
}
void Color::setARGB(unsigned char alpha, unsigned char red, unsigned char green, unsigned char blue)
{
    m_alpha = alpha;
    m_red = red;
    m_green = green;
    m_blue = blue;
}
unsigned int Color::toOleColor(unsigned char alpha, unsigned char red, unsigned char green, unsigned char blue)
{
    return red + green * 256 + blue * 256 * 256;
}
void Color::fromOleColor(unsigned int oleColor, unsigned int *r, unsigned int *g, unsigned int *b)
{
    *r = oleColor & 0xff;
    *g = (oleColor >> 8) & 0xff;
    *b = (oleColor >> 16) & 0xff;
}
