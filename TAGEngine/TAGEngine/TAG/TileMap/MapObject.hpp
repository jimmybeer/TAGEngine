#pragma once

#include "VectorAlgebra2D.hpp"
//#include "MathFuncs.hpp"
#include "DebugShape.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <SFML/System/NonCopyable.hpp>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <memory>

class TileQuad;

enum MapObjectShape
{
    Rectangle,
    Ellipse,
    Circle,
    Polygon,
    Polyline,
    Tile
};

// Map object class.
class MapObject final
{
private:
    struct Segment
    {
        Segment(const sf::Vector2f& start, const sf::Vector2f& end)
         : start(start)
         , end(end)
        {}
        
        bool intersects(const Segment& segment);
        
        sf::Vector2f start;
        sf::Vector2f end;
    };
  
public:
    MapObject();
    
    // Returns empty string if property not found.
    std::string getPropertyString(const std::string& name);
    
    // returns top left corner of bounding rectangle.
    sf::Vector2f getPosition() const { return mPosition; }
    
    // returns precomputed centre of mass, or zero for polylines.
    sf::Vector2f getCentre() const { return mCentrePoint; }
    
    // returns the type of shape of the object.
    MapObjectShape getShapeType() const { return mShape; }
    
    // returns the object's name
    std::string getName() const { return mName; }
    
    // returns the object's type
    std::string getType() const { return mType; }
    
    // returns the name of the object's parent layer
    std::string getParent() const { return mParent;  }
    
    // Returns the objects AABB in world coordinates
    sf::FloatRect getAABB() const { return mAABB; }
    
    // returns visibility
    bool visible() const { return mVisible; }
    
    // Sets a property value, or adds it if property doesn't exist
    void setProperty(const std::string& name, const std::string& value);
    
    // sets the object position in world coords
    void setPosition(float x, float y);
    void setPosition(const sf::Vector2f& position);
    
    // moves the object by given amount
    void move(float x, float y);
    void move(const sf::Vector2f& distance);
    
    // sets the width and height of the object
    void setSize(const sf::Vector2f& size) { mSize = size; }
    
    // sets the object's name
    void setName(const std::string& name) { mName = name;  }
    
    // sets the object's type
    void setType(const std::string& type) { mType = type; }
    
    // sets the object's parent layer
    void setParent(const std::string& parent) { mParent = parent; }
    
    // sets the shape type
    void setShapeType(MapObjectShape shape) { mShape = shape; }
    
    // sets the visiblity
    void setVisible(bool visible) { mVisible = visible; }
    
    // Adds a point to the list of polygon points. If calling this manually
    // call createDebugShape() afterwards to rebuild debug output
    void addPoint(const sf::Vector2f& point) { mPolypoints.push_back(point); }
    
    // checks if an object contains given point in world coords.
    // Always returns false for polylines.
    bool contains(sf::Vector2f point) const;
    
    // checks if two objects intersect, including polylines.
    bool intersects(const MapObject& object) const;
    
    // creates a shape used for debug drawing - points are in world space
    void createDebugShape(const sf::Color& color);
    
    // draws debug shape to given target
    void drawDebugShape(sf::RenderTarget& rt) const;
    
    // returns the first point of poly point member (if any)
    sf::Vector2f firstPoint() const;
    
    // returns the last point of poly point member (if any)
    sf::Vector2f lastPoint() const;
    
    // returns a unit vector normal to the polyline segment if intersected
    // takes the start and end point of a trajectory
    sf::Vector2f collisionNormal(const sf::Vector2f& start, const sf::Vector2f& end) const;
    
    // creates a vector of segments making up the poly shape.
    void createSegments();
    
    // returns if an object poly shape is convex or not.
    bool convex() const;
    
    // returns a reference to the array of points making up the object
    const std::vector<sf::Vector2f>& polyPoints() const;
    
    // reversing winding of object points
    void reverseWinding();
    
    // sets the quad usef to draw the tile for tile objects.
    void setQuad(TileQuad* quad);
    
private:
    // object properties, reflect those which are part of the tmx format.
    std::string mName, mType, mParent; // parent is the name of layer to which the object belongs.
    sf::Vector2f mPosition, mSize;
    std::map<std::string, std::string> mProperties; // map of custom name/value properties.
    bool mVisible;
    std::vector<sf::Vector2f> mPolypoints;
    MapObjectShape mShape;
    DebugShape mDebugShape;
    sf::Vector2f mCentrePoint;
    
    std::vector<Segment> mPolySegs; // segments which make up shape, if any
    TileQuad* mTileQuad;
    
    // The furthest distance from centre of object to vertex - used for intersection testing
    // AABB created from polygonal shapes, used for adding MapObjects to a QuadTreeNode.
    // Note that the position of this box may not necessarily match the MapObject's position, 
    // as polygonal points may have negative values relative to the object's world position.
    float mFurthestPoint; 
    sf::FloatRect mAABB;
    
    // returns centre of poly shape if available, else centre of
    // bounding rectangle in world space
    sf::Vector2f calcCentre() const;
    
    // precomputes centre point and furthest point to be used in intersection testing
    void calcTestValues();
    
    // creates an AABB around the object based on its polygonal points, in world space
    void createAABB();
};
typedef std::vector<MapObject> MapObjects;
    
// represents a single tile on a layer
struct MapTile final
{
    // returns the base centre point of sprite / tile
    sf::Vector2f getBase() const
    {
        return sf::Vector2f(sprite.getPosition().x + (sprite.getLocalBounds().width / 2.f),
                            sprite.getPosition().y + sprite.getLocalBounds().height);
    }
    
    sf::Sprite sprite;
    sf::Vector2i gridCoord;
    sf::RenderStates renderStates; // used to perform any rendering with custom shaders or blend mode
};
typedef std::vector<MapTile> MapTiles;