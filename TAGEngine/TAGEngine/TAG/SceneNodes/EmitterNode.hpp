#pragma once

#include "SceneNode.hpp"
#include "Particle.hpp"

class ParticleNode;

class EmitterNode : public SceneNode
{
public:
    explicit EmitterNode(Particle::Type type);
	void setEmissionRate(float particlesPerSecond);
	void setTotalLifetime(sf::Time time);
	void setEnabled(bool enable);
	bool isEnabled() const;
	
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
};