#include "UniversalEmitterNode.hpp"
#include "ParticleNode.hpp"
#include "Command.hpp"
#include "CommandQueue.hpp"
 
UniversalEmitterNode::UniversalEmitterNode(Particle::Type type)
: SceneNode()
, mEnabled(true)
, mEmissionRate(1.f)
, mEmissionInterval(sf::seconds(1.f) / mEmissionRate)
, mParticleLifetime(sf::seconds(1.f)) 
, mParticlePosition(sf::Vector2f(0.f, 0.f))
, mParticleVelocity(sf::Vector2f(0.f, 0.f))
, mParticleRotation(0.f)
, mParticleRotationSpeed(0.f)
, mParticleScale(sf::Vector2f(1.f, 1.f))
, mParticleColor(sf::Color::White)
, mParticleTextureIndex(0u)
, mAccumulatedTime(sf::Time::Zero)
, mType(type)
, mParticleSystem(nullptr)
, mTimeBound(false)
, mLifetime(sf::Time::Zero)
, mTotalLife(sf::Time::Zero)
{}

void UniversalEmitterNode::setEmissionRate(float particlesPerSecond)
{
    mEmissionRate = particlesPerSecond;
	mEmissionInterval = sf::seconds(1.f) / mEmissionRate;
}

void UniversalEmitterNode::setTotalLifetime(sf::Time time)
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

void UniversalEmitterNode::setEnabled(bool enable)
{
    mEnabled = enable;
}

bool UniversalEmitterNode::isEnabled() const
{
    return mEnabled;
}

void UniversalEmitterNode::setParticleSystem(ParticleNode* particleSystem)
{
    mParticleSystem = particleSystem;
}

void UniversalEmitterNode::updateCurrent(sf::Time dt, CommandQueue& commands)
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

void UniversalEmitterNode::emitParticles(sf::Time dt)
{	
	mAccumulatedTime += dt;
	
	while(mAccumulatedTime > mEmissionInterval)
	{
	    mAccumulatedTime -= mEmissionInterval;
		
	    Particle particle;
		particle.position = getWorldPosition() + mParticlePosition();
		particle.velocity = mParticleVelocity();
		particle.rotation = mParticleRotation();
		particle.rotationSpeed = mParticleRotationSpeed();
		particle.scale = mParticleScale();
		particle.color = mParticleColor();
		particle.textureIndex = mParticleTextureIndex();
	    particle.lifetime = sf::Time::Zero;
	    particle.totalLife = mParticleLifetime();
		
		mParticleSystem->addParticle(particle);
	}
}

void UniversalEmitterNode::setParticleLifetime(tag::Distribution<sf::Time> particleLifetime)
{
	mParticleLifetime = std::move(particleLifetime);
}

void UniversalEmitterNode::setParticlePosition(tag::Distribution<sf::Vector2f> particlePosition)
{
	mParticlePosition = std::move(particlePosition);
}

void UniversalEmitterNode::setParticleVelocity(tag::Distribution<sf::Vector2f> particleVelocity)
{
	mParticleVelocity = std::move(particleVelocity);
}

void UniversalEmitterNode::setParticleRotation(tag::Distribution<float> particleRotation)
{
	mParticleRotation = std::move(particleRotation);
}

void UniversalEmitterNode::setParticleRotationSpeed(tag::Distribution<float> particleRotationSpeed)
{
	mParticleRotationSpeed = std::move(particleRotationSpeed);
}

void UniversalEmitterNode::setParticleScale(tag::Distribution<sf::Vector2f> particleScale)
{
	mParticleScale = std::move(particleScale);
}

void UniversalEmitterNode::setParticleColor(tag::Distribution<sf::Color> particleColor)
{
	mParticleColor = std::move(particleColor);
}

void UniversalEmitterNode::setParticleTextureIndex(tag::Distribution<unsigned int> particleTextureIndex)
{
	mParticleTextureIndex = std::move(particleTextureIndex);
}