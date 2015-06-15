#include "World.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

World::World(sf::RenderWindow& window)
 : mWindow(window)
 , mWorldView(window.getDefaultView())
 , mSceneGraph()
 , mSceneLayers()
 , mWorldBounds(0.f, 0.f, mWorldView.getSize().x, mWorldView.getSize().y)
{}

int World::addLayer(SceneNode* layer)
{
    SceneNode::Ptr node(layer);
	
	int layerIndex = mSceneLayers.size();
	mSceneLayers[layerIndex] = node.get();
	
	mSceneGraph.attachChild(std::move(node));
	
	return layerIndex;
}

int World::addLayer(SceneNode* layer, const std::string& id)
{
    auto inserted = mLayerId.insert(id, addLayer(layer));
	assert(inserted);
}

SceneNode* World::getLayer(const std::string& id)
{    
    auto found = mLayerId.find(id);
	assert(found != mLayerId.end());
	
	return mSceneLayers[found->second];
}

void World::update(sf::Time dt)
{
    while(!mCommandQueue.isEmpty())
	{
	    mSceneGraph.onCommend(mCommendQueue.pop(), dt);
	}
	
    mSceneGraph.update(dt);
}

void World::draw()
{
    mWindow.setView(mWorldView);
	mWindow.draw(mSceneGraph);
}

CommandQueue& World::getCommandQueue()
{
    return mCommandQueue;
}