#pragma once

#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include <functional>
#include <memory>

class Particle;
// Create Affector objects and add to the particle node you wish to be affected. 
// However, you are responsible to ensure the lifetime of the referenced object
// as the particle node only stores a reference to the affector. Example:
//
// Create affector and particle system
// ForceAffector affector(...);
// ParticleNode system(...);
//
// Add affector to particle system
// system.addAffector(affector);
//
// Change affector properties later
// affector.setAcceleration(acceleration);
class Affector
{
public:
    typedef std::unique_ptr<Affector> Ptr;
	virtual void affect(Particle& particle, sf::Time dt) = 0;
};

// Applies a translational acceleration to particles over time.
// Affector class that applies an acceleration vector to each particle. A popular use case is gravity.
class ForceAffector : public Affector
{
public:
    explicit ForceAffector(sf::Vector2f acceleration);
	
	void affect(Particle& particle, sf::Time dt);
	
	void setAcceleration(sf::Vector2f acceleration);
	sf::Vector2f getAcceleration() const;

private:
    sf::Vector2f mAcceleration;
};

// Applies a rotational acceleration to particles over time.
// Affector class that applies a scalar angular acceleration value to each particle.
class TorqueAffector : public Affector
{
public:
    explicit TorqueAffector(float angularAcceleration);
	
	void affect(Particle& particle, sf::Time dt);
	
	void setAngularAcceleration(float angularAcceleration);
	float getAngularAcceleration() const;

private:
	float mAngularAcceleration;
};

// Scales particles over time.
class ScaleAffector : public Affector
{
public:
    explicit ScaleAffector(sf::Vector2f scaleFactor);
	
	void affect(Particle& particle, sf::Time dt);
	
	//  Sets the factor by which particles are scaled every second.
	void setScaleFactor(sf::Vector2f scaleFactor);
	sf::Vector2f getScaleFactor() const;
private:
    sf::Vector2f mScaleFactor;
};

class FadeAffector : public Affector
{
public:
    explicit FadeAffector();
	
	void affect(Particle& particle, sf::Time dt);
};

// Affector that animates particles using a function.
// This affector can be used to apply animations of Animations module to particles. Such animations are described
// by a function with signature void(Particle& animated, float progress)
class AnimationAffector : public Affector
{
public:
	// Applies an animation during the whole lifetime of the particles.
	// particleAnimation An animation function that is applied to the particle. Its second parameter progress
	// corresponds to getElapsedRatio(particle), the delta time of operator() is ignored.
	explicit AnimationAffector(std::function<void(Particle&, float)> particleAnimation);

	void affect(Particle& particle, sf::Time dt);
	
private:
    std::function<void(Particle&, float)> mAnimation;

};