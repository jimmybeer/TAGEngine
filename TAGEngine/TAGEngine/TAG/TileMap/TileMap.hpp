#pragma once

// https://github.com/fallahn/sfml-tmxloader/tree/master/src

#include "Tileset.hpp"
#include "MapLayer.hpp"

#include "pugixml.hpp"

#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/NonCopyable.hpp>

#include <string>
#include <list>
#include <vector>
#include <map>
#include <memory>
#include <array>
#include <bitset>

class SceneNode;


namespace tmx
{
    enum MapOrientation
    {
        Orthogonal,
        Isometric,
        SteppedIsometric
    };
}

class TileMap : public sf::Drawable, private sf::NonCopyable
{
public:
    TileMap(const TileMap&) = delete;
	TileMap& operator=(const TileMap&) = delete;
	
	TileMap(sf::Uint8 patchSize = 10u);
	virtual ~TileMap();
	
    //returns a vector of map layers
    std::vector<MapLayer>& getLayers();
	const std::vector<MapLayer>& getLayers() const;
	
    //draws visible tiles to given target, optionally draw outline of objects for debugging
    void draw(sf::RenderTarget& rt, MapLayer::DrawType type, bool debug = false);
    //overload for drawing layer by index
    void draw(sf::RenderTarget& rt, sf::Uint16 index, bool debug = false);
    
	unsigned int getTileWidth() const;
	unsigned int getTileHeight() const;
	unsigned int getNumCols() const;
	unsigned int getNumRows() const;
	unsigned int getMapWidth() const;
	unsigned int getMapHeight() const;
    
	//loads a given tmx file, returns false on failure
	bool loadMap(const std::string& filename);
    void unLoad();
    
	//sets the shader property of a layer's rendering states member
    void setLayerShader(sf::Uint16 layerId, const sf::Shader& shader);
    
    //returns empty string if property not found
    std::string getPropertyString(const std::string& name);
protected:
	unsigned int mTileWidth;
	unsigned int mTileHeight;
	unsigned int mCols;
	unsigned int mRows;
	SceneNode* mParentNode;
	
private:
    bool parseMapNode(const pugi::xml_node& mapNode);
    bool parseTilesets(const pugi::xml_node& mapNode);
    bool parseLayer(const pugi::xml_node& layerNode);
    void parseLayerProperties(const pugi::xml_node& propertiesNode, MapLayer& destLayer);
    bool processTiles(const pugi::xml_node& tilesetNode);
    
    //Reading the flipped bits
    std::vector<unsigned char> intToBytes(sf::Uint32 paramInt);
    std::pair<sf::Uint32, std::bitset<3> > resolveRotation(sf::Uint32 gid);
    
    //Image flip functions
    void flipY(sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3);
    void flipX(sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3);
    void flipD(sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3);
    void doFlips(std::bitset<3> bits,sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3);
    TileQuad* addTileToLayer(MapLayer& layer, sf::Uint16 x, sf::Uint16 y, sf::Uint32 gid, const sf::Vector2f& offset = sf::Vector2f());
    
    void createDebugGrid();
	//sets the visible area of tiles to be drawn
	void setDrawingBounds(const sf::View& view);

    void drawLayer(sf::RenderTarget& rt, MapLayer& layer, bool debug = false);
	void draw(sf::RenderTarget& rt, sf::RenderStates states) const;
    
    mutable sf::FloatRect mBounds; //bounding area of tiles visible on screen
	mutable sf::Vector2f mLastViewPos; //save recalc bounds if view not moved
    
	mutable std::vector<MapLayer> mLayers;
    std::map<std::string, std::string> mProperties;
    
    std::vector<std::unique_ptr<sf::Texture>> mTilesetTextures;
    const sf::Uint8 mPatchSize;
    
    struct TileInfo // hold texture coords and tileset id of a tile
    {
        std::array<sf::Vector2f, 4> coords;
        sf::Vector2f size;
        sf::Uint16 tilesetId;
        TileInfo();
        TileInfo(const sf::IntRect& rect, const sf::Vector2f& size, sf::Uint16 tilesetId);
    };
    std::vector<TileInfo> mTileInfo; // stores information on all the tilesets for creating vertex arrays.
    sf::VertexArray mGridVertices; //used to draw map grid in debug
    
    // Caches loaded images to prevent loading the same tileset more than once
    sf::Image& loadImage(const std::string& imageName);
    std::map<std::string, std::shared_ptr<sf::Image>> mCachedImages;
    bool mFailedImage;
    
    bool decompress(const char* source, std::vector<unsigned char>& dest, int inSize, int expectedSize);
    std::string fileFromPath(const std::string& path);
    sf::Color colorFromHex(const char* hexStr) const;
};

//method for decoding base64 encoded strings
static std::string base64Decode(std::string const& string);