#pragma once

#include "SceneNode.hpp"
#include "CommandQueue.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <array>

// Forward declaration
namespace sf
{
    class RenderWindow;
}

class World : private sf::NonCopyable
{
public:
    explicit World(sf::RenderWindow& window);
	void update(sf::Time dt);
	void draw();
	CommandQueue& getCommandQueue();
	
	int addLayer(SceneNode* layer);
	int addLayer(SceneNode* layer, const std::string& id);
	SceneNode* getLayer(const std::string& id);
	
protected:
    sf::RenderWindow& mWindow;
	sf::View mWorldView;
	
	SceneNode mSceneGraph;
	std::array<SceneNode*, LayerCount> mSceneLayers;
	std::map<std::string, int> mLayerId;
	CommandQueue mCommandQueue;
	
	sf::FloatRect worldBounds;
};