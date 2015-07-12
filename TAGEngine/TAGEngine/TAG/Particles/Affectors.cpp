#include "Affectors.hpp"
#include "Particle.hpp"

#include <cassert>
#include <cmath>

ForceAffector::ForceAffector(sf::Vector2f acceleration)
 : mAcceleration(acceleration)
{}

void ForceAffector::affect(Particle& particle, sf::Time dt)
{
    particle.velocity += dt.asSeconds() * mAcceleration;
}

void ForceAffector::setAcceleration(sf::Vector2f acceleration)
{
    mAcceleration = acceleration;
}

sf::Vector2f ForceAffector::getAcceleration() const
{
    return mAcceleration;
}


TorqueAffector::TorqueAffector(float angularAcceleration)
 : mAngularAcceleration(angularAcceleration)
{}

void TorqueAffector::affect(Particle& particle, sf::Time dt)
{
    particle.rotationSpeed += dt.asSeconds() * mAngularAcceleration;
}

void TorqueAffector::setAngularAcceleration(float angularAcceleration)
{
    mAngularAcceleration = angularAcceleration;
}

float TorqueAffector::getAngularAcceleration() const
{
    return mAngularAcceleration;
}


ScaleAffector::ScaleAffector(sf::Vector2f scaleFactor)
 : mScaleFactor(scaleFactor)
{}

void ScaleAffector::affect(Particle& particle, sf::Time dt)
{
    particle.scale += dt.asSeconds() * mScaleFactor;
}

void ScaleAffector::setScaleFactor(sf::Vector2f scaleFactor)
{
    mScaleFactor = scaleFactor;
}

sf::Vector2f ScaleAffector::getScaleFactor() const
{
    return mScaleFactor;
}

FadeAffector::FadeAffector()
{}
	
void FadeAffector::affect(Particle& particle, sf::Time dt)
{
    particle.color.a = static_cast<sf::Uint8>(255 * std::fmax(particle.getRemainingRatio(), 0.f));
}

AnimationAffector::AnimationAffector(std::function<void(Particle&, float)> particleAnimation)
 : mAnimation(std::move(particleAnimation))
{}

void AnimationAffector::affect(Particle& particle, sf::Time)
{
    mAnimation(particle, particle.getElapsedRatio());
}