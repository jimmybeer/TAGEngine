#pragma once

#include "SceneNode.hpp"
#include "Particle.hpp"

#include "Distribution.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

class ParticleNode;

// Class that emits particles with customizable initial conditions.
// This emitter is universal with respect to the initial conditions of each emitted particle. It works with callbacks
//  that return initial values for the particle attributes (position, rotation, color, ...). So you can pass constants, random
//  distributions, or any functions that compute the value in an arbitrary way. Have a look at the tag::Distributions namespace
//  for useful predefined functions.
/**
 * Example Usage:
 *
 * std::unique_ptr<UniversalEmitter> emitter(new UniversalEmitter(particleType));
 * emitter->setPosition(0.f, 0.f);
 * emitter.setEmissionRate(30);
 * emitter.setParticleLifetime(sf::seconds(5));
 * emitter.setParticlePosition( tag::Distributions::circle(center, radius) );   // Emit particles in given circle
 * emitter.setParticleVelocity( tag::Distributions::deflect(direction, 15.f) ); // Emit towards direction with deviation of 15°
 * emitter.setParticleRotation( tag::Distributions::uniform(0.f, 360.f) );      // Rotate randomly
 * attachChild(std::move(emitter));
 */
class UniversalEmitterNode : public SceneNode
{
public:
    explicit UniversalEmitterNode(Particle::Type type);
	void setTotalLifetime(sf::Time time);
	void setEnabled(bool enable);
	bool isEnabled() const;
	
	void setParticleSystem(ParticleNode* particleSystem);
	
	// Sets the particle emission rate.
	// particlesPerSecond How many particles are emitted in 1 second. The type is not integral to allow
	// flexibility (e.g. 0.5 yields one particle every 2 seconds).
	void setEmissionRate(float particlesPerSecond);

	// Sets the lifetime (time between emission and death) of the particle.
	// particleLifetime Constant or function returning the particle lifetime.
	void setParticleLifetime(tag::Distribution<sf::Time> particleLifetime);

	// Sets the initial particle position.
	// particlePosition Constant or function returning the initial particle position.
	void setParticlePosition(tag::Distribution<sf::Vector2f> particlePosition);

	// Sets the initial particle velocity.
	//  particleVelocity Constant or function returning the initial particle velocity.
	void setParticleVelocity(tag::Distribution<sf::Vector2f> particleVelocity);

    // Sets the initial particle rotation.
	// particleRotation Constant or function returning the initial particle rotation.
    void setParticleRotation(tag::Distribution<float> particleRotation);

	// Sets the initial particle rotation speed.
	// particleRotationSpeed Constant or function returning the initial particle rotation speed.
	void setParticleRotationSpeed(tag::Distribution<float> particleRotationSpeed);

	// Sets the initial particle scale.
	// particleScale Constant or function returning the initial particle scale.
	void setParticleScale(tag::Distribution<sf::Vector2f> particleScale);

	// Sets the initial particle color.
	// particleColor Constant or function returning the initial particle color.
	void setParticleColor(tag::Distribution<sf::Color> particleColor);

	// Sets the initial particle texture index.
	// particleTextureIndex Constant or function returning the initial index of the particle's texture rectangle.
	void setParticleTextureIndex(tag::Distribution<unsigned int> particleTextureIndex);
	
private:
    virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	void emitParticles(sf::Time dt);
	
	float mEmissionRate;
    sf::Time mEmissionInterval;
	
	sf::Time mAccumulatedTime;
	Particle::Type mType;
	ParticleNode* mParticleSystem;
	bool mEnabled;
	bool mTimeBound;
	sf::Time mLifetime;
    sf::Time mTotalLife;
	
	tag::Distribution<sf::Time>		mParticleLifetime;
	tag::Distribution<sf::Vector2f>	mParticlePosition;
	tag::Distribution<sf::Vector2f>	mParticleVelocity;
	tag::Distribution<float>		mParticleRotation;
	tag::Distribution<float>		mParticleRotationSpeed;
	tag::Distribution<sf::Vector2f>	mParticleScale;
	tag::Distribution<sf::Color>	mParticleColor;
	tag::Distribution<unsigned int>	mParticleTextureIndex;
};