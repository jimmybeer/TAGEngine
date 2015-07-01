#include "EmitterNode.hpp"
#include "ParticleNode.hpp"
#include "CommandQueue.hpp"
#include "Command.hpp"
#include "DataTables.hpp"

namespace
{
    const std::vector<ParticleData> Table = initialiseParticleData();
}

EmitterNode::EmitterNode(Particle::Type type)
 : SceneNode()
 , mEnabled(true)
 , mEmissionRate(30.f)
 , mEmissionInterval(sf::seconds(1.f) / mEmissionRate)
 , mAccumulatedTime(sf::Time::Zero)
 , mType(type)
 , mParticleSystem(nullptr)
 , mTimeBound(false)
 , mLifetime(sf::Time::Zero)
 , mTotalLife(sf::Time::Zero)
{}
	
void EmitterNode::setEmissionRate(float particlesPerSecond)
{
    mEmissionRate = particlesPerSecond;
	mEmissionInterval = sf::seconds(1.f) / mEmissionRate;
}

void EmitterNode::setTotalLifetime(sf::Time time)
{
    if(time == sf::Time::Zero)
	{
	    mTimeBound = false;
	}
	else
	{
	    mTimeBound = true;
	}
	mTotalLife = time;
}

void EmitterNode::setEnabled(bool enable)
{
    mEnabled = enable;
}

bool EmitterNode::isEnabled() const
{
    return mEnabled;
}

void EmitterNode::updateCurrent(sf::Time dt, CommandQueue& commands)
{
    if(mParticleSystem)
	{
	    if(mEnabled)
		{
	        if(mTimeBound)
		    {
		        mLifetime += dt;
			
			    if(mLifetime > mTotalLife)
			    {
			        mEnabled = false;
			    	return;
			    }
		    }
	        emitParticles(dt);
		}
	}
	else
	{
	    // Find particle node with the same type as emitter node
		auto finder = [this] (ParticleNode& container, sf::Time)
		{
		    if(container.getParticleType() == mType)
			{
			    mParticleSystem = &container;
			}
		};
		
		Command command;
		command.category = Category::ParticleSystem;
		command.action = derivedAction<ParticleNode>(finder);
		
		commands.push(command);
	}
}

void EmitterNode::emitParticles(sf::Time dt)
{	
	mAccumulatedTime += dt;
	
	while(mAccumulatedTime > mEmissionInterval)
	{
	    mAccumulatedTime -= mEmissionInterval;
		
	    Particle particle;
	    particle.position = getWorldPosition();
	    particle.color = Table[mType].color;
	    particle.lifetime = sf::Time::Zero;
	    particle.totalLife = Table[mType].lifeTime;
		particle.textureIndex = 0;
		
		mParticleSystem->addParticle(particle);
	}
}