#pragma once

// https://github.com/fallahn/sfml-tmxloader/blob/master/include/tmx/MapLayer.h
#include "MapObject.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Vertex.hpp>

#include <memory>
#include <array>
#include <vector>
#include <string>
#include <map>

class LayerSet;
class TileQuad final
{
    friend class LayerSet;
public:
    typedef std::unique_ptr<TileQuad> Ptr;
    TileQuad(sf::Uint16 i0, sf::Uint16 i1, sf::Uint16 i2, sf::Uint16 i3);
    void move(const sf::Vector2f& distance);
    
private:
    std::array<sf::Uint16, 4u> mIndices;
    sf::Vector2f mMovement;
    LayerSet* mParentSet;
    sf::Int32 mPatchIndex;
    bool mDirty;
};

// drawable composed of vertices representing a set of tiles on a layer
class LayerSet final : public sf::Drawable
{
    friend class TileQuad;
public:
    LayerSet(const sf::Texture& texture, sf::Uint8 patchSize, const sf::Vector2u& mapSize, const sf::Vector2u tileSize);
    TileQuad* addTile(sf::Vertex vt0, sf::Vertex vt1, sf::Vertex vt2, sf::Vertex vt3, sf::Uint16 x, sf::Uint16 y);
    void cull(const sf::FloatRect& bounds);

private:
    const sf::Texture& mTexture;
    const sf::Uint8 mPatchSize;
    const sf::Vector2u mMapSize;
    const sf::Vector2u mPatchCount;
    const sf::Vector2u mTileSize;
    
    std::vector<TileQuad::Ptr> mQuads;
    mutable std::vector<TileQuad*> mDirtyQuads;
    
    sf::Vector2i mVisiblePatchStart, mVisiblePatchEnd;
    mutable std::vector<std::vector<sf::Vertex>> mPatches;
    
    void draw(sf::RenderTarget& rt, sf::RenderStates states) const;
    
    mutable sf::FloatRect mBoundingBox;
    void updateAABB(sf::Vector2f position, sf::Vector2f size);
    bool mVisible;
};

// used to query the type of layer, for example when looking for layers containing collision objects.
namespace tmx
{
    enum MapLayerType
    {
        Layer,
        ObjectGroup,
        ImageLayer
    };
}

// represents a layer of tiles, corresponding to a tmx layer, object group or image layer
class MapLayer final : public sf::Drawable
{
public:
    // used for drawing specific layers
    enum DrawType
    {
        Front, 
        Back,
        Debug,
        All
    };

    explicit MapLayer(tmx::MapLayerType layerType);
    std::string name;
    float opacity;
    bool visible;
    MapTiles tiles;
    MapObjects objects;
    tmx::MapLayerType type;
    std::map<std::string, std::string> properties;

    std::map<sf::Uint16, std::shared_ptr<LayerSet>> layerSets;
    void setShader(const sf::Shader& shader);
    void cull(const sf::FloatRect& bounds);
    
private:
    const sf::Shader* mShader;
    void draw(sf::RenderTarget& rt, sf::RenderStates states) const;
};