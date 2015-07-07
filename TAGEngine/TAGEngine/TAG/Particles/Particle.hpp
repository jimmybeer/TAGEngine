#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Color.hpp>

struct Particle
{
    enum Type
	{
        none,
	    Propellant,
		Smoke,
		ParticleCount
	};
	
	sf::Vector2f position;	//< Current position
	sf::Vector2f velocity;	//< Velocity (change in position per second)
	float rotation;			//< Current rotation angle
	float rotationSpeed;	//< Angular velocity (change in rotation per second)
	sf::Vector2f scale;		//< Scale, where (1,1) represents the original size
	sf::Color color;		//< Particle color
	unsigned int textureIndex; //< Index of the used texture rect, returned by ParticleSystem::addTextureRect()
	sf::Time lifetime;
    sf::Time totalLife;

    // Returns the time passed since the particle has been emitted.
    sf::Time getElapsedLifetime() const;

    //  Returns the total time the particle is alive.
    sf::Time getTotalLifetime() const;

    // Returns the time left until the particle dies.
    sf::Time getRemainingLifetime() const;

    // Returns elapsed lifetime / total lifetime<.
    float getElapsedRatio() const;

    // Returns remaining lifetime / total lifetime.
    float getRemainingRatio() const;
		
	// Marks a particle for removal.
    // This function can be used when the lifetime of particles is not bound to time, but another condition.
    //  For example, in a physics simulation, particles may disappear as soon as they hit an object. Calling this function
    //  will set the particle's elapsed time to the total lifetime, and the next update of the particle system will remove it.
	void abandonParticle();
};