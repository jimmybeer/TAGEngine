#pragma once

#include "SceneNode.hpp"
#include "ResourceHolder.hpp"
//#include "ResourceIdentifiers.hpp"
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
	    typedef std::unique_ptr<AffectorRef> Ptr;
		
	    AffectorRef(Affector::Ptr affector, sf::Time timeUntilRemoval)
		 : affector(std::move(affector))
		 , timeUntilRemoval(timeUntilRemoval)
		{}
		
		Affector::Ptr affector;
		sf::Time timeUntilRemoval;
	};
	
    // Vertex quads, used to cache texture rectangles
	typedef std::array<sf::Vertex, 4> Quad;
	
public:
    ParticleNode();
    ParticleNode(unsigned int type, const sf::Texture* textures = nullptr);
	
	// Sets the used texture.
	// Only one texture can be used at a time. If you need multiple particle representations, specify different texture
	// rectangles using the method addTextureRect(). If no texture rect is added, the whole texture will be used.
	// The texture must remain valid as long as the particle node is using it.
	void setTexture(const sf::Texture* texture);
	
	// Defines a new texture rect to represent a particle.
	// Can be used to create different visual representations of a particle, for example different shapes of debris.
	// Returns the index of the texture rect just added. This should be assiged to the Particle::textureIndex variable.
	unsigned int addTextureRect(const sf::IntRect& textureRect);
	
	// Adds a particle affector for a certain amount of time.
	// Affectors are applied in the order they were added to the system.
	void addAffector(Affector::Ptr affectorRef, sf::Time timeUntilRemoval = sf::Time::Zero);
	
	void clearAffectors();
	
	void addParticle(const Particle& particle);
	unsigned int getParticleType() const;
	virtual unsigned int getCategory() const;
	
	void clearParticles();
	
private:
    virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	
	// Recomputes the vertex array.
	void computeVertices() const;
	
	// Recomputes the cached rectangles (position and texCoords quads)
	void computeQuads() const;
	void computeQuad(Quad& quad, const sf::IntRect& textureRect) const;
		
	unsigned int mType;
	std::deque<Particle> mParticles;
	
	const sf::Texture* mTexture;
	std::vector<sf::IntRect> mTextureRects;	
	
	std::vector<AffectorRef::Ptr> mAffectors;
	int mExpiringAffectors;
	
	mutable sf::VertexArray mVertexArray;
	mutable bool mNeedsVertexUpdate;
	mutable std::vector<Quad> mQuads;
	mutable bool mNeedsQuadUpdate;
};