#pragma once

#include "Distribution.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>

// Brief Functors to create random distributions of geometric shapes

namespace tag
{
// Namespace for some predefined distribution functions
namespace Distributions
{
    // Uniform random distribution in an int interval
    Distribution<int> uniform(int min, int max);
   
    // Uniform random distribution in an unsigned int interval
    Distribution<unsigned int> uniform(unsigned int min, unsigned int max);
   
    // Uniform random distribution in an float interval
    Distribution<float> uniform(float min, float max);
   
    // Uniform random distribution in an time interval
    Distribution<sf::Time> uniform(sf::Time min, sf::Time max);
   
    // Uniform random distribution in a rectangle
    Distribution<sf::Vector2f> rect(sf::Vector2f center, sf::Vector2f halfSize);
   
    // Uniform random distribution in a circle
    Distribution<sf::Vector2f> circle(sf::Vector2f center, float radius);
   
    // Vector rotation with a random angle
    Distribution<sf::Vector2f> deflect(sf::Vector2f direction, float maxRotation);
 
}
}