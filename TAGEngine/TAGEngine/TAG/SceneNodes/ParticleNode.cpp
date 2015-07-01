#include "ParticleNode.hpp"
#include "DataTables.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <algorithm>

namespace
{
    const std::vector<ParticleData> Table = initialiseParticleData();
	
	sf::IntRect getFullRect(const sf::Texture* texture)
	{
	    return sf::IntRect(0, 0, texture->getSize().x, texture->getSize().y);
	}
}

ParticleNode::ParticleNode()
 : SceneNode()
 , mParticles()
 , mTexture(nullptr)
 , mTextureRects()
 , mAffectors()
 , mExpiringAffectors(0)
 , mType(Particle::Propellant)
 , mVertexArray(sf::Quads)
 , mNeedsVertexUpdate(false)
 , mQuads()
 , mNeedsQuadUpdate(false)
{}

ParticleNode::ParticleNode(Particle::Type type, const TextureHolder& textures)
 : SceneNode()
 , mParticles()
 , mTexture(&textures.get(Textures::Particle))
 , mTextureRects()
 , mAffectors()
 , mExpiringAffectors(0)
 , mType(type)
 , mVertexArray(sf::Quads)
 , mNeedsVertexUpdate(true)
 , mQuads()
 , mNeedsQuadUpdate(true)
{}
	
void ParticleNode::setTexture(const sf::Texture& texture)
{
    mTexture = &texture;
	mNeedsQuadUpdate = true;
}
	
unsigned int ParticleNode::addTextureRect(const sf::IntRect& textureRect)
{
    mTextureRects.push_back(textureRect);
	mNeedsQuadUpdate = true;
	
	return mTextureRects.size() - 1;
}

void ParticleNode::addParticle(const Particle& particle)
{	
	mParticles.push_back(particle);
}

void ParticleNode::addAffector(Affector& affectorRef, sf::Time timeUntilRemoval)
{
    AffectorRef ref(affectorRef, timeUntilRemoval);
	mAffectors.push_back(ref);
	
	if(timeUntilRemoval > sf::Time::Zero)
	{
	 	mExpiringAffectors++;
	}
}

void ParticleNode::clearAffectors()
{
    mAffectors.clear();
}

Particle::Type ParticleNode::getParticleType() const
{
    return mType;
}

unsigned int ParticleNode::getCategory() const
{
    return Category::ParticleSystem;
}

void ParticleNode::clearParticles()
{
    mParticles.clear();
}

void ParticleNode::updateCurrent(sf::Time dt, CommandQueue&)
{
    // Remove expired particles at beginning
	while(!mParticles.empty() && mParticles.front().getRemainingLifetime() <= sf::Time::Zero)
	{
	    mParticles.pop_front();
	}
	
	// Decrease lifetime of existing particles
	for(Particle& particle : mParticles)
	{
	    particle.lifetime += dt;
		
		particle.position += dt.asSeconds() * particle.velocity;
		particle.rotation += dt.asSeconds() * particle.rotationSpeed;
		
		for(AffectorRef aRef : mAffectors)
		{
		    aRef.affector(particle, dt);
		}
	}
	
	if(mExpiringAffectors > 0)
	{
	    // Clean up expired affectors
	    mAffectors.erase(
	        std::remove_if(mAffectors.begin(),
		                   mAffectors.end(),
		    			   [&](AffectorRef& ref) {
                               if(ref.timeUntilRemoval != sf::Time::Zero && (ref.timeUntilRemoval -= dt) <= sf::Time::Zero)
							    {
							       mExpiringAffectors--;
								   return true;
								}
								else
								{
								    return false;
								}
	    				   }),
	    	mAffectors.end());
	}
	
	mNeedsVertexUpdate = true;
}

void ParticleNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
    if(mNeedsQuadUpdate)
	{
	    computeQuads();
	    mNeedsQuadUpdate = false;
	}
	
    if(mNeedsVertexUpdate)
	{
	    computeVertices();
		mNeedsVertexUpdate = false;
	}
	
	// Apply particle texture
	states.texture = mTexture;
	
	// Draw vertices
	target.draw(mVertexArray, states);
}

void ParticleNode::computeVertices() const
{
	// Clear vertex array (keeps memory allocated)
	mVertexArray.clear();
	
	// Fill xertex array.
	for(const Particle& p : mParticles)
	{
	    sf::Transform transform;
		transform.translate(p.position);
		transform.rotate(p.rotation);
		transform.scale(p.scale);
		
		// Ensure valid index -- if this fails addTextureRect() has not been called enough times, or p.textureIndex is simply wrong
		assert(p.textureIndex == 0 || p.textureIndex < mTextureRects.size());
		
		const auto& quad = mQuads[p.textureIndex];
		for(unsigned int i = 0; i < 4; ++i)
		{
		    sf::Vertex vertex;
			vertex.position = transform.transformPoint(quad[i].position);
			vertex.texCoords = quad[i].texCoords;
			vertex.color = p.color;
			
			mVertexArray.append(vertex);
		}
	}
}

void ParticleNode::computeQuads() const
{
	// Ensure setTexture() has been called
	assert(mTexture);

	// No texture rects: Use full texture, cache single rectangle
	if (mTextureRects.empty())
	{
		mQuads.resize(1);
		computeQuad(mQuads[0], getFullRect(mTexture));
	}

	// Specified texture rects: Cache every one
	else
	{
		mQuads.resize(mTextureRects.size());
		for (std::size_t i = 0; i < mTextureRects.size(); ++i)
			computeQuad(mQuads[i], mTextureRects[i]);
	}
}

void ParticleNode::computeQuad(Quad& quad, const sf::IntRect& textureRect) const
{
	sf::FloatRect rect(textureRect);

	quad[0].texCoords = sf::Vector2f(rect.left,              rect.top);
	quad[1].texCoords = sf::Vector2f(rect.left + rect.width, rect.top);
	quad[2].texCoords = sf::Vector2f(rect.left + rect.width, rect.top + rect.height);
	quad[3].texCoords = sf::Vector2f(rect.left,              rect.top + rect.height);

	quad[0].position = sf::Vector2f(-rect.width, -rect.height) / 2.f;
	quad[1].position = sf::Vector2f( rect.width, -rect.height) / 2.f;
	quad[2].position = sf::Vector2f( rect.width,  rect.height) / 2.f;
	quad[3].position = sf::Vector2f(-rect.width,  rect.height) / 2.f;
}