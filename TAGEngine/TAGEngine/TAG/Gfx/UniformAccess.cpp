#include "UniformAccess.hpp"
#include "Particle.hpp"

namespace tag
{

void setColor(Particle& particle, const sf::Color& color)
{
    particle.color = color;
}

void setAlpha(Particle& particle, sf::Uint8 alpha)
{
    particle.color.a = alpha;
}

}