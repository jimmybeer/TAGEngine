#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>

#include <sstream>

namespace sf
{
    class Sprite;
	class Text;
}

class Animation;

// Since std::to_string doesn't work on MinGW we have implemented 
// our own to support all platforms
template <typename T>
std::string toString(const T& value);

// Converts enumerators to strings
std::string toString(sf::Keyboard::Key key);

// Centers the origin of the supplied subject.
void centerOrigin(sf::Sprite& sprite);
void centerOrigin(sf::Text& text);
void centerOrigin(Animation& animation);

// Degree/radian conversion
float toDegree(float radian);
float toRadian(float degree);

// Random number generation
int randomInt(int exclusiveMax);

// Vector operations
float length(sf::Vector2f vector);
sf::Vector2f unitVector(sf::Vector2f vector);

#include "Utility.inl"