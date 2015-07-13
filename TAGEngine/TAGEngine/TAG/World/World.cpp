#include "World.hpp"
#include "Projectile.hpp"
#include "Pickup.hpp"
#include "TextNode.hpp"
#include "EmitterNode.hpp"
#include "ParticleNode.hpp"
#include "Affectors.hpp"
#include "ResourcePath.hpp"
#include "SoundNode.hpp"
#include "MyCategory.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

World::World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds)
 : mTarget(outputTarget)
 , mSceneTexture()
 , mWorldView(outputTarget.getDefaultView())
 , mFonts(fonts)
 , mSounds(sounds)
 , mTextures()
 , mSceneGraph()
 , mSceneLayers()
 , mWorldBounds(0.f, 0.f, mWorldView.getSize().x, 2000.0f)
 , mSpawnPosition(mWorldView.getSize().x / 2.f, mWorldBounds.height - mWorldView.getSize().y / 2.f)
 , mScrollSpeed(-50.f)
 , mPlayerAircraft(nullptr)
 , mEnemySpawnPoints()
 , mActiveEnemies()
 , mPostEffectsSupported(false)
{
    mPostEffectsSupported = PostEffect::isSupported();
	if(mPostEffectsSupported)
	{
        mSceneTexture.create(mTarget.getSize().x, mTarget.getSize().y);
	}
	
    loadTextures();
	buildScene();
	
	// Prepare the view.
	mWorldView.setCenter(mSpawnPosition);
}

void World::update(sf::Time dt)
{
    // Scroll the world, reset player velocity
	mWorldView.move(0.f, mScrollSpeed * dt.asSeconds());
	mPlayerAircraft->setVelocity(0.f, 0.f);
	
	// Setup commands to destroy entities, and guided missiles.
	destroyEntitiesOutsideView();
	guideMissiles();
	
	// Forwards commands to scene graph, adapt velocity (scrolling, diagonal correction)
    while(!mCommandQueue.isEmpty())
	{
	    mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	}
	adaptPlayerVelocity();
	
	// Collision detection and response (map destroy entities).
	handleCollisions();
	
	// Remove all destroyed entities, create new ones
	mSceneGraph.removeWrecks();
	spawnEnemies();
	
    mSceneGraph.update(dt, mCommandQueue);
	adaptPlayerPosition();
	
	updateSounds();
}

void World::draw()
{
    if(mPostEffectsSupported)
	{
	    mSceneTexture.clear();
		mSceneTexture.setView(mWorldView);
		mSceneTexture.draw(mSceneGraph);
		mSceneTexture.display();
		mBloomEffect.apply(mSceneTexture, mTarget);
	}
	else
	{
        mTarget.setView(mWorldView);
	    mTarget.draw(mSceneGraph);
	}
}

CommandQueue& World::getCommandQueue()
{
    return mCommandQueue;
}

bool World::hasAlivePlayer() const
{
    return !mPlayerAircraft->isMarkedForRemoval();
}

bool World::hasPlayerReachedEnd() const
{
    return !mWorldBounds.contains(mPlayerAircraft->getPosition());
}

void World::loadTextures()
{
    mTextures.load(Textures::Entities, resourcePath() + TEXTURES + "Entities.png");
    mTextures.load(Textures::Jungle, resourcePath() + TEXTURES + "Jungle.png");
    mTextures.load(Textures::Explosions, resourcePath() + TEXTURES + "Explosion.png");
    mTextures.load(Textures::Particle, resourcePath() + TEXTURES + "Particle.png");
    mTextures.load(Textures::FinishLine, resourcePath() + TEXTURES + "FinishLine.png");
}

void World::adaptPlayerPosition()
{
    // Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 40.f;
	
	sf::Vector2f position = mPlayerAircraft->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top + borderDistance);
	position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
	mPlayerAircraft->setPosition(position);
}

void World::adaptPlayerVelocity()
{
    sf::Vector2f velocity = mPlayerAircraft->getVelocity();
	
	// If moving diagonally, reduce velocity (to have always same velocity)
	if(velocity.x != 0.f && velocity.y != 0.f)
	{
	    mPlayerAircraft->setVelocity(velocity / std::sqrt(2.f));
	}
	
	// Add scrolling velocity
	mPlayerAircraft->accelerate(0.f, mScrollSpeed);
}

bool matchCategories(SceneNode::Pair& colliders, unsigned int type1, unsigned int type2)
{
    unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();
	
	// Make sure first pair entry has category type 1 and seconds has type 2;
	if(type1 & category1 && type2 & category2)
	{
	    return true;
	}
	else if(type1 & category2 && type2 & category1)
	{
	    std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
	    return false;
	}
}

void World::handleCollisions()
{
    std::set<SceneNode::Pair> collisionPairs;
	mSceneGraph.checkSceneCollision(mSceneGraph, collisionPairs);
	
	for(SceneNode::Pair pair : collisionPairs)
	{
	    if(matchCategories(pair, Category::PlayerAircraft, Category::EnemyAircraft))
		{
		    auto& player = static_cast<Aircraft&>(*pair.first);
			auto& enemy = static_cast<Aircraft&>(*pair.second);
			
			// Collision: Player damage = enemey's remaining HP
			player.damage(enemy.getHitpoints());
		    enemy.destroy();
		}
		else if(matchCategories(pair, Category::PlayerAircraft, Category::Pickup))
		{
		    auto& player = static_cast<Aircraft&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);
			
			// Apply pickup effect to player, destroy projectile
			pickup.apply(player);
			pickup.destroy();
			player.playLocalSound(mCommandQueue, SoundEffect::CollectPickup);
		}
		else if(matchCategories(pair, Category::EnemyAircraft, Category::AlliedProjectile)
		     || matchCategories(pair, Category::PlayerAircraft, Category::EnemyProjectile))
	    {
		    auto& aircraft = static_cast<Aircraft&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);
			
			// Apply projectile damage to aircraft, destroy projectile.
			aircraft.damage(projectile.getDamage());
			projectile.destroy();
		}
	}
}

void World::updateSounds()
{
    // Set listener's position to player position
	mSounds.setListenerPosition(mPlayerAircraft->getWorldPosition());
	
	// Remove unused sounds
	mSounds.removeStoppedSounds();
}

void World::buildScene()
{
    // Initialize the different layers
	for(std::size_t i = 0; i < LayerCount; ++i)
	{
	    unsigned int category = (i == LowerAir) ? Category::SceneAirLayer : Category::None;
		
		SceneNode::Ptr layer(new SceneNode(category));
		mSceneLayers[i] = layer.get();
		
		mSceneGraph.attachChild(std::move(layer));
	}
	
	// Prepare tiled background
	sf::Texture& jungleTexture = mTextures.get(Textures::Jungle);
	jungleTexture.setRepeated(true);
	
	float viewHeight = mWorldView.getSize().y;
	sf::IntRect textureRect(mWorldBounds);
	textureRect.height += static_cast<int>(viewHeight);
	
	// Add the background sprite to the scene.
	std::unique_ptr<SpriteNode> jungleSprite(new SpriteNode(jungleTexture, textureRect));
	jungleSprite->setPosition(mWorldBounds.left, mWorldBounds.top - viewHeight);
	mSceneLayers[Background]->attachChild(std::move(jungleSprite));
	
	// Add the finish line to the scene
	sf::Texture& finishTexture = mTextures.get(Textures::FinishLine);
	std::unique_ptr<SpriteNode> finishSprite(new SpriteNode(finishTexture));
	finishSprite->setPosition(0.f, -76.f);
	mSceneLayers[Background]->attachChild(std::move(finishSprite));
	
    EmitterNode::Table.resize(Particle::ParticleCount);
    EmitterNode::addParticleData(Particle::Propellant, sf::Color(255, 255, 50), sf::seconds(0.6f), sf::Vector2f(0.75f, 0.75f));
    EmitterNode::addParticleData(Particle::Smoke, sf::Color(50, 50, 50), sf::seconds(1.f), sf::Vector2f(1.f, 1.f));
    
	// Add particle node to the scene
	std::unique_ptr<ParticleNode> smokeNode(new ParticleNode(Particle::Smoke, &mTextures.get(Textures::Particle)));
    Affector::Ptr fa1(new FadeAffector);
    smokeNode->addAffector(std::move(fa1));
    mSceneLayers[LowerAir]->attachChild(std::move(smokeNode));
	
	// Add propellant
    std::unique_ptr<ParticleNode> propellantNode(new ParticleNode(Particle::Propellant, &mTextures.get(Textures::Particle)));
    Affector::Ptr fa3(new FadeAffector);
    propellantNode->addAffector(std::move(fa3));
	mSceneLayers[LowerAir]->attachChild(std::move(propellantNode));
	
	// Add sound effect node
	std::unique_ptr<SoundNode> soundNode(new SoundNode(mSounds));
	mSceneGraph.attachChild(std::move(soundNode));
	
	// Add player's aircraft
    std::unique_ptr<Aircraft> player(new Aircraft(Aircraft::Eagle, mTextures, mFonts));
	mPlayerAircraft = player.get();
	mPlayerAircraft->setPosition(mSpawnPosition);
	mSceneLayers[UpperAir]->attachChild(std::move(player));
	
	// Add enemy aircraft
	addEnemies();
}

void World::addEnemies()
{
	// Add enemies to the spawn point container
	addEnemy(Aircraft::Raptor,    0.f,  500.f);
	addEnemy(Aircraft::Raptor,    0.f, 1000.f);
	addEnemy(Aircraft::Raptor, +100.f, 1150.f);
	addEnemy(Aircraft::Raptor, -100.f, 1150.f);
	addEnemy(Aircraft::Avenger,  70.f, 1500.f);
	addEnemy(Aircraft::Avenger, -70.f, 1500.f);
	addEnemy(Aircraft::Avenger, -70.f, 1710.f);
	addEnemy(Aircraft::Avenger,  70.f, 1700.f);
	addEnemy(Aircraft::Avenger,  30.f, 1850.f);
	addEnemy(Aircraft::Raptor,  300.f, 2200.f);
	addEnemy(Aircraft::Raptor, -300.f, 2200.f);
	addEnemy(Aircraft::Raptor,    0.f, 2200.f);
	addEnemy(Aircraft::Raptor,    0.f, 2500.f);
	addEnemy(Aircraft::Avenger,-300.f, 2700.f);
	addEnemy(Aircraft::Avenger,-300.f, 2700.f);
	addEnemy(Aircraft::Raptor,    0.f, 3000.f);
	addEnemy(Aircraft::Raptor,  250.f, 3250.f);
	addEnemy(Aircraft::Raptor, -250.f, 3250.f);
	addEnemy(Aircraft::Avenger,   0.f, 3500.f);
	addEnemy(Aircraft::Avenger,   0.f, 3700.f);
	addEnemy(Aircraft::Raptor,    0.f, 3800.f);
	addEnemy(Aircraft::Avenger,   0.f, 4000.f);
	addEnemy(Aircraft::Avenger,-200.f, 4200.f);
	addEnemy(Aircraft::Raptor,  200.f, 4200.f);
	addEnemy(Aircraft::Raptor,    0.f, 4400.f);
	
	// Sort all enemies according to their y value, such that lower enemies are checked first for spawning
	std::sort(mEnemySpawnPoints.begin(), mEnemySpawnPoints.end(), [] (SpawnPoint lhs, SpawnPoint rhs)
	{
	    return lhs.y < rhs.y;
	});
}

void World::addEnemy(Aircraft::Type type, float relX, float relY)
{
    SpawnPoint spawn(type, mSpawnPosition.x + relX, mSpawnPosition.y - relY);
	mEnemySpawnPoints.push_back(spawn);
}

void World::spawnEnemies()
{
    // Spawn all enemies entering the view area (including distance) this frame.
	while(!mEnemySpawnPoints.empty()
	    && mEnemySpawnPoints.back().y > getBattlefieldBounds().top)
	{
	    SpawnPoint spawn = mEnemySpawnPoints.back();
		
		std::unique_ptr<Aircraft> enemy(new Aircraft(spawn.type, mTextures, mFonts));
		enemy->setPosition(spawn.x, spawn.y);
		enemy->setRotation(180.f);
		
		mSceneLayers[UpperAir]->attachChild(std::move(enemy));
		
		// Enemy is spawned, remove from the list to spawn
		mEnemySpawnPoints.pop_back();
	}
}

void World::destroyEntitiesOutsideView()
{
    Command command;
	command.category = Category::Projectile | Category::EnemyAircraft;
	command.action = derivedAction<Entity>([this] (Entity& e, sf::Time)
	{
	    if(!getBattlefieldBounds().intersects(e.getBoundingRect()))
		{
		    e.destroy();
		}
	});
	
	mCommandQueue.push(command);
}

void World::guideMissiles()
{
    // Setup command that stores all enemies in mActiveEnemies
	Command enemyCollector;
	enemyCollector.category = Category::EnemyAircraft;
	enemyCollector.action = derivedAction<Aircraft>([this] (Aircraft& enemy, sf::Time)
	{
	    if(!enemy.isDestroyed())
		{
		    mActiveEnemies.push_back(&enemy);
		}
	});
	
	// Setup command that guides all missiles to the enemy which is currently closest to the player
	Command missileGuider;
	missileGuider.category = Category::AlliedProjectile;
	missileGuider.action = derivedAction<Projectile>([this] (Projectile& missile, sf::Time)
	{
	    // Ignore unguided bullets
		if(!missile.isGuided())
		{
		    return;
		}
		
		float minDistance = std::numeric_limits<float>::max();
		Aircraft* closestEnemy = nullptr;
		
		// Find closest enemy
		for(Aircraft* enemy : mActiveEnemies)
		{
	        float enemyDistance = distance(missile, *enemy);
			
			if(enemyDistance < minDistance)
			{
			    closestEnemy = enemy;
			    minDistance = enemyDistance;
			}
		}
		
		if(closestEnemy)
		{
		    missile.guideTowards(closestEnemy->getWorldPosition());
		}
	});
	
	// Push commands, reset active enemies
	mCommandQueue.push(enemyCollector);
	mCommandQueue.push(missileGuider);
	mActiveEnemies.clear();
}

sf::FloatRect World::getViewBounds() const
{
    return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}

sf::FloatRect World::getBattlefieldBounds() const
{
    // Return view bounds + some area at top , where enemies spawn
	sf::FloatRect bounds = getViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;
	
	return bounds;
}