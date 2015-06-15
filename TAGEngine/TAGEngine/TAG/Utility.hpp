#pragma once

#include <sstream>

namespace sf
{
    class Sprite;
	class Text;
}

// Since std::to_string doesn't work on MinGW we have implemented 
// our own to support all platforms
template <typename T>
std::string toString(const T& value);

void centerOrigin(sf::Sprite& sprite);
void centerOrigin(sf::Text& text);

#include <Utility.inl>