#include "Distributions.hpp"
#include "Random.hpp"
#include "VectorAlgebra2D.hpp"
#include "PolarVector2.hpp"

#include <cassert>

namespace tag
{
namespace Distributions
{
    namespace
	{
	    template<typename T>
		Distribution<T> uniformT(T min, T max)
		{
		    assert(min <= max);
			
			return [=]() -> T
			{
			    return random(min, max);
			};
		}
	}
	
	Distribution<int> uniform(int min, int max)
	{
	    return uniformT(min, max);
	}

	Distribution<unsigned int> uniform(unsigned int min, unsigned int max)
	{
	    return uniformT(min, max);
	}
	
    Distribution<float> uniform(float min, float max)
	{
	    return uniformT(min, max);
	}
   
    Distribution<sf::Time> uniform(sf::Time min, sf::Time max)
	{
	    assert(min <= max);
		
		const float floatMin = min.asSeconds();
		const float floatMax = max.asSeconds();
		
		return [=]() -> sf::Time
		{
		    return sf::seconds(random(floatMin, floatMax));
		};
	}
   
    Distribution<sf::Vector2f> rect(sf::Vector2f center, sf::Vector2f halfSize)
	{
	    assert(halfSize.x >= 0.f && halfSize.y >= 0.f);
		
		return [=]() -> sf::Vector2f
		{
		    return sf::Vector2f(randomDev(center.x, halfSize.x),
			                    randomDev(center.y, halfSize.y));
		};
	}
   
    Distribution<sf::Vector2f> circle(sf::Vector2f center, float radius)
	{
	    assert(radius >= 0.f);
		
		return [=]() -> sf::Vector2f
		{
		    sf::Vector2f radiusVector = PolarVector2f(radius * std::sqrt(random(0.f, 1.f)), random(0.f, 360.f));
			return center + radiusVector;
		};
	}
   
    Distribution<sf::Vector2f> deflect(sf::Vector2f direction, float maxRotation)
	{
	    return [=]() -> sf::Vector2f
		{
		    return rotatedVector(direction, randomDev(0.f, maxRotation));
		};
	}
}
}