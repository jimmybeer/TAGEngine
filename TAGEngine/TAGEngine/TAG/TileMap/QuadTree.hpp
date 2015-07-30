#pragma once

// Creates a node used to build quad trees for spatial partioning of MapObjects.
// Example usage: create a root node the size of the viewable area, and insert each 
// available map object. Then test the root node by calling retrieve passing for example
// the AABB of a sprite. The resulting vector will contain pointers to any objects contained 
// in quads which are themselves contained, or intersected, by the sprites AABB. These can
// then be collision tested.

#include "MapObject.hpp"
#include <memory>

class QuadTreeNode : public sf::Drawable
{
public:
    QuadTreeNode(sf::Uint16 level = 0, const sf::FloatRect& bounds = sf::FloatRect(0.f, 0.f, 0.1f, 0.1f));
    virtual ~QuadTreeNode() {};
    
    // fills vector with references to all object which
    // appear in quads which are contained or intersect bounds.
    std::vector<MapObject*> retrieve(const sf::FloatRect& bounds, sf::Uint16& currentDepth);
    
    // insert a reference to the object into the node's object list
    void insert(const MapObject& object);
    
protected:
    // maximum objects per node before splitting
    const sf::Uint16 MaxObjects;
    // maximum number of levels to split
    const sf::Uint16 MaxLevels;
    
    sf::Uint16 mLevel;
    sf::FloatRect mBounds;
    std::vector<MapObject*> mObjects; // objects contined in current node.
    std::vector<std::unique_ptr<QuadTreeNode>> mChildren; // vector of child nodes.
    sf::RectangleShape mDebugShape;
    
    // Returns the index of the child node into which the given bounds fits.
    // returns -1 if it doesn't completely fit a child. Numbered anti-clockwise
    // rom top right node.
    sf::Int16 getIndex(const sf::FloatRect& bounds);
    
    // divides node by creating 4 children
    void split(void);

private:
    void draw(sf::RenderTarget& rt, sf::RenderStates states)const;
};

// Specialisation of QuadTreeNode for counting tree depth
class QuadTreeRoot final : public QuadTreeNode
{
public:
    QuadTreeRoot(sf::Uint16 level = 0, const sf::FloatRect& bounds = sf::FloatRect(0.f, 0.f, 1.f, 1.f))
     : QuadTreeNode(level, bounds), mDepth(0u), mSearchDepth(0u)
    {}
    
    // clears node and all children
    void clear(const sf::FloatRect& newBounds);
    
    // retrieves all objects in quads which contains or intersectes test area
    std::vector<MapObject*> retrieve(const sf::FloatRect& bounds)
    {
        return QuadTreeNode::retrieve(bounds, mSearchDepth);
    }
    
private:
    // Total depth of tree, and depth reached when querying.
    sf::Uint16 mDepth, mSearchDepth;
};