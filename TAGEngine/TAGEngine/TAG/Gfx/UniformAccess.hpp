#pragma once

// Global functions to access SFML and TAG objects uniformly

#include <SFML/Graphics/Color.hpp>

class Particle;

namespace tag
{

// Set the color of a graphical object;
template <typename T>
void setColor(T& object, const sf::Color& color);

// Sets the color of a particle
void setColor(Particle& particle, const sf::Color& color);

// Sets the alpha color value of a graphical object
// The object shall support the methods getColor() and setColor()
template <typename T>
void setAlpha(T& object, sf::Uint8 alpha);

// Sets the alpha color value of a particle
void setAlpha(Particle& particle, sf::Uint8 alpha);

}
#include "UniformAccess.inl"
