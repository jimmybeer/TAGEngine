#include "Projectile.hpp"
#include "UniversalEmitterNode.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"
#include "ResourceHolder.hpp"
#include "MyCategory.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cmath>
#include <cassert>

namespace
{
    const std::vector<ProjectileData> Table = initialiseProjectileData();
}

Projectile::Projectile(Type type, const TextureHolder& textures)
 : Entity(1)
 , mType(type)
 , mSprite(textures.get(Table[type].texture), Table[type].textureRect)
 , mTargetDirection()
{
    centerOrigin(mSprite);
	
	if(isGuided())
	{
	    std::unique_ptr<UniversalEmitterNode> smoke(new UniversalEmitterNode(Particle::Smoke));
		smoke->setPosition(0.f, getBoundingRect().height / 2.f);
        smoke->setEmissionRate(60.f);
        smoke->setParticleLifetime(sf::seconds(1.5f));
        smoke->setParticleColor(sf::Color(50, 50, 50));
        smoke->setParticleScale(sf::Vector2f(1.f, 1.f));
		attachChild(std::move(smoke));
		
		std::unique_ptr<UniversalEmitterNode> propellant(new UniversalEmitterNode(Particle::Propellant));
		propellant->setPosition(0.f, getBoundingRect().height / 2.f);
        propellant->setEmissionRate(60.f);
        propellant->setParticleLifetime(sf::seconds(0.6f));
        propellant->setParticleColor(sf::Color(255, 255, 50));
        propellant->setParticleScale(sf::Vector2f(0.75f, 0.75f));
		attachChild(std::move(propellant));
	}
}

void Projectile::guideTowards(sf::Vector2f position)
{
    assert(isGuided());
	mTargetDirection = unitVector(position - getWorldPosition());
}

bool Projectile::isGuided() const
{
    return mType == Missile;
}

void Projectile::updateCurrent(sf::Time dt, CommandQueue& commands)
{
    if(isGuided())
	{
	    const float approachRate = 200.0f;
		
		sf::Vector2f newVelocity = unitVector(approachRate * dt.asSeconds() * mTargetDirection + getVelocity());
		newVelocity *= getMaxSpeed();
		float angle = std::atan2(newVelocity.y, newVelocity.x);
		
		setRotation(toDegree(angle) + 90.f);
		setVelocity(newVelocity);
	}
	
	Entity::updateCurrent(dt, commands);
}

void Projectile::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(mSprite, states);
}

unsigned int Projectile::getCategory() const
{
    if(mType == EnemyBullet)
	{
	    return Category::EnemyProjectile;
	}
	else
	{
	    return Category::AlliedProjectile;
	}
}

sf::FloatRect Projectile::getBoundingRect() const
{
    return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

float Projectile::getMaxSpeed() const
{
    return Table[mType].speed;
}

int Projectile::getDamage() const
{
    return Table[mType].damage;
}