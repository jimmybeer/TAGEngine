#pragma once

#include "SceneNode.hpp"
#include "Particle.hpp"

#include <vector>

class ParticleNode;

namespace sf
{
    class Color;
	class Time;
}

class EmitterNode : public SceneNode
{
public:
    struct ParticleData
    {
        ParticleData() {}
		
        sf::Color color;
	    sf::Time lifeTime;		
        unsigned int textureIndex;
        sf::Vector2f scale;
    };
    
    //typedef std::vector<ParticleData> Table;

    explicit EmitterNode(unsigned int type);
	void setEmissionRate(float particlesPerSecond);
	void setTotalLifetime(sf::Time time);
	void setEnabled(bool enable);
	bool isEnabled() const;
    
    static void addParticleData(unsigned int id, sf::Color color, sf::Time lifeTime, sf::Vector2f scale, unsigned int textureIndex = 0);
    static std::vector<ParticleData> Table;
    
private:
    virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	void emitParticles(sf::Time dt);
	
	float mEmissionRate;
	sf::Time mEmissionInterval;
	
	sf::Time mAccumulatedTime;
	unsigned int mType;
	ParticleNode* mParticleSystem;
	bool mEnabled;
	bool mTimeBound;
	sf::Time mLifetime;
    sf::Time mTotalLife;
};