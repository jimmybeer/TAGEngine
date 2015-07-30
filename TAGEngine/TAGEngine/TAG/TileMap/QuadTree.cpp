#include "QuadTree.hpp"

QuadTreeNode::QuadTreeNode(sf::Uint16 level, const sf::FloatRect& bounds)
 : MaxObjects(5u)
 , MaxLevels(5u)
 , mLevel(level)
 , mBounds(bounds)
{
    mChildren.reserve(4);
    mDebugShape = sf::RectangleShape(sf::Vector2f(bounds.width, bounds.height));
    mDebugShape.setPosition(bounds.left, bounds.top);
    mDebugShape.setFillColor(sf::Color::Transparent);
    mDebugShape.setOutlineColor(sf::Color::Green);
    mDebugShape.setOutlineThickness(-2.f);
}

void QuadTreeRoot::clear(const sf::FloatRect& newBounds)
{
    mObjects.clear();
    mChildren.clear();
    mBounds = newBounds;
    mDebugShape.setPosition(newBounds.left, newBounds.top);
    mDebugShape.setSize(sf::Vector2f(newBounds.width, newBounds.height));
    
    mSearchDepth = 0;
    mDepth = 0;
}

std::vector<MapObject*> QuadTreeNode::retrieve(const sf::FloatRect& bounds, sf::Uint16& searchDepth)
{
    searchDepth = mLevel;
    std::vector<MapObject*> foundObjects;
    sf::Int16 index = getIndex(bounds);
    
    // recursively add objects of child node if bounds are fully contained
    if(!mChildren.empty() && index != -1)
    {
        foundObjects = mChildren[index]->retrieve(bounds, searchDepth);
    }
    else
    {
        // add all objects of child nodes which intersect teat area
        for(auto& child : mChildren)
        {
            if(bounds.intersects(child->mBounds))
            {
                std::vector<MapObject*> childObjects = child->retrieve(bounds, searchDepth);
                foundObjects.insert(foundObjects.end(), childObjects.begin(), childObjects.end());
            }
        }
    }
    // and append objects in this node
    foundObjects.insert(foundObjects.end(), mObjects.begin(), mObjects.end());
    mDebugShape.setOutlineColor(sf::Color::Red);
    return foundObjects;
}

void QuadTreeNode::insert(const MapObject& object)
{
    // check if an object falls completely outside a node
    if(!object.getAABB().intersects(mBounds)) return;
    
    // if node is already split add object to corresponding child node
    // if it fits
    if(!mChildren.empty())
    {
        sf::Int16 index = getIndex(object.getAABB());
        if(index != -1)
        {
            mChildren[index]->insert(object);
            return;
        }
    }
    // else add object to this node
    mObjects.push_back(const_cast<MapObject*>(&object));
    
    // check number of objects in this node, and split if necessary
    // adding any objects that fit to the new child node.
    if(mObjects.size() > MaxObjects && mLevel < MaxLevels)
    {
        // split if there are no child nodes.
        if(mChildren.empty()) split();
        
        sf::Uint16 i = 0;
        while(i < mObjects.size())
        {
            sf::Int16 index = getIndex(mObjects[i]->getAABB());
            if(index != -1)
            {
                mChildren[index]->insert(*mObjects[i]);
                mObjects.erase(mObjects.begin() + i);
            }
            else
            {
                // We only increment i when not erasing, beacuse erasing moves
                // everything up one index inside the vector.
                i++;
            }
        }
    }
}

void QuadTreeNode::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    // Draw children
    for(auto& child : mChildren)
    {
        rt.draw(*child);
    }
    
    rt.draw(mDebugShape);
}

void QuadTreeNode::split()
{
    const float halfWidth = mBounds.width / 2.f;
    const float halfHeight = mBounds.height / 2.f;
    const float x = mBounds.left;
    const float y = mBounds.top;
    
    mChildren.push_back(std::unique_ptr<QuadTreeNode>(new QuadTreeNode(mLevel+1, sf::FloatRect(x + halfWidth, y, halfWidth, halfHeight))));
    mChildren.push_back(std::unique_ptr<QuadTreeNode>(new QuadTreeNode(mLevel+1, sf::FloatRect(x, y, halfWidth, halfHeight))));
    mChildren.push_back(std::unique_ptr<QuadTreeNode>(new QuadTreeNode(mLevel+1, sf::FloatRect(x, y + halfHeight, halfWidth, halfHeight))));
    mChildren.push_back(std::unique_ptr<QuadTreeNode>(new QuadTreeNode(mLevel+1, sf::FloatRect(x + halfWidth, y + halfHeight, halfWidth, halfHeight))));
}

sf::Int16 QuadTreeNode::getIndex(const sf::FloatRect& bounds)
{
    sf::Int16 index = -1;
    float verticalMidpoint = mBounds.left + (mBounds.width / 2.f);
    float horizontalMidpoint = mBounds.top + (mBounds.height / 2.f);
    
    // Object can completely fit within the top quadrants.
    bool topQuadrant = (bounds.top < horizontalMidpoint && (bounds.top + bounds.height) < horizontalMidpoint);
    // Object can completely fit within the bottom quadtants.
    bool bottomQuadrant = (bounds.top > horizontalMidpoint);
    
    // Object can completely fit within the left quadrants
    if(bounds.left < verticalMidpoint && (bounds.left + bounds.width) < verticalMidpoint)
    {
        if(topQuadrant)
        {
            index = 1;
        }
        else if(bottomQuadrant)
        {
            index = 2;
        }
    }
    // Object can completely fit within the right quadrants
    else if(bounds.left > verticalMidpoint)
    {
        if(topQuadrant)
        {
            index = 0;
        }
        else if(bottomQuadrant)
        {
            index = 3;
        }
    }
    return index;
}