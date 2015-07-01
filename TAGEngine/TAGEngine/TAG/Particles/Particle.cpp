#include "Particle.hpp"

sf::Time Particle::getElapsedLifetime() const
{
    return lifetime;
}

sf::Time Particle::getTotalLifetime() const
{
    return totalLife;
}

sf::Time Particle::getRemainingLifetime() const
{
    return totalLife - lifetime;
}

float Particle::getElapsedRatio() const
{
    return lifetime / totalLife;
}

float Particle::getRemainingRatio() const
{
    return getRemainingLifetime() / totalLife;
}
	
void Particle::abandonParticle()
{
    lifetime = totalLife;
}
	