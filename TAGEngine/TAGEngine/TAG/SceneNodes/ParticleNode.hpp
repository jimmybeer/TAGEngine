#pragma once

#include "SceneNode.hpp"
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "Particle.hpp"
#include "Affectors.hpp"

#include <SFML/Graphics/VertexArray.hpp>

#include <deque>
#include <vector>
#include <utility>
#include <functional>
#include <memory>
#include <array>

class ParticleNode : public SceneNode
{
private:
    // Type to store a reference to an Affector + time until removal
	struct AffectorRef
	{
	    AffectorRef(Affector& affector, sf::Time timeUntilRemoval)
		 : affector(affector)
		 , timeUntilRemoval(timeUntilRemoval)
		{}
        
        AffectorRef(const AffectorRef& rhs)
        : affector(rhs.affector)
        , timeUntilRemoval(rhs.timeUntilRemoval)
        {}
        
        AffectorRef& operator=(const AffectorRef& rhs)
        {
            this->affector = rhs.affector;
            this->timeUntilRemoval = rhs.timeUntilRemoval;
        }
		
		Affector& affector;
		sf::Time timeUntilRemoval;
	};
	
    // Vertex quads, used to cache texture rectangles
	typedef std::array<sf::Vertex, 4> Quad;
	
public:
    ParticleNode();
    ParticleNode(Particle::Type type, const TextureHolder& textures, int category = -4);
	
	// Sets the used texture.
	// Only one texture can be used at a time. If you need multiple particle representations, specify different texture
	// rectangles using the method addTextureRect(). If no texture rect is added, the whole texture will be used.
	// The texture must remain valid as long as the particle node is using it.
	void setTexture(const sf::Texture& texture);
	
	// Defines a new texture rect to represent a particle.
	// Can be used to create different visual representations of a particle, for example different shapes of debris.
	// Returns the index of the texture rect just added. This should be assiged to the Particle::textureIndex variable.
	unsigned int addTextureRect(const sf::IntRect& textureRect);
	
	// Adds a particle affector for a certain amount of time.
	// Affectors are applied in the order they were added to the system.
	void addAffector(Affector& affectorRef, sf::Time timeUntilRemoval = sf::Time::Zero);
	
	void clearAffectors();
	
	void addParticle(const Particle& particle);
	Particle::Type getParticleType() const;
	
	void clearParticles();
	
private:
    virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	
	// Recomputes the vertex array.
	void computeVertices() const;
	
	// Recomputes the cached rectangles (position and texCoords quads)
	void computeQuads() const;
	void computeQuad(Quad& quad, const sf::IntRect& textureRect) const;
		
	Particle::Type mType;
	std::deque<Particle> mParticles;
	
	const sf::Texture* mTexture;
	std::vector<sf::IntRect> mTextureRects;	
	
	std::vector<AffectorRef> mAffectors;
	int mExpiringAffectors;
	
	mutable sf::VertexArray mVertexArray;
	mutable bool mNeedsVertexUpdate;
	mutable std::vector<Quad> mQuads;
	mutable bool mNeedsQuadUpdate;
};