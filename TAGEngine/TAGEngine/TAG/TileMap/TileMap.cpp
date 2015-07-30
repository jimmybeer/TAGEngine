#include "TileMap.hpp"
#include "Logger.hpp"

#include "ResourcePath.hpp"

#include "SceneNode.hpp"
//#include "Square.hpp"

#include <algorithm>
#include <sstream>
#include <cstring>
#include <zlib.h>
#include <utility>
#include <cassert>

int Logger::mLogFilter = (Type::Error | Type::Info | Type::Warning);

TileMap::TileMap(sf::Uint8 patchSize)
 : mTileWidth(1u)
 , mTileHeight(1u)
 , mCols(1u)
 , mRows(1u)
 , mPatchSize(patchSize)
 , mLayers()
 , mTilesetTextures()
 , mProperties()
 , mTileInfo()
 , mCachedImages()
 , mFailedImage(false)
 , mQuadTreeAvailable(false)
{
    // reserve some space
    mLayers.reserve(5);
    
    assert(patchSize > 0);
}

TileMap::~TileMap()
{
    unLoad();
}

void TileMap::draw(sf::RenderTarget& rt, MapLayer::DrawType type, bool debug)
{
    setDrawingBounds(rt.getView());
    switch(type)
    {
        default:
        case MapLayer::All :
            for(const auto& l : mLayers)
                rt.draw(l);
            break;
        case MapLayer::Back :
            {
                // remember front of vector actually draws furthes back
                MapLayer& layer = mLayers.front();
                drawLayer(rt, layer, debug);
            }
            break;
        case MapLayer::Front :
            {
                
                MapLayer& layer = mLayers.back();
                drawLayer(rt, layer, debug);
            }
        case MapLayer::Debug :
            for(auto layer : mLayers)
            {
                if(layer.type == tmx::ObjectGroup)
                {
                    for(const auto& object : layer.objects)
                    {
                        if(mBounds.intersects(object.getAABB()))
                            object.drawDebugShape(rt);
                    }
                }
            }
            rt.draw(mGridVertices);
            rt.draw(mRootNode);
            break;
    }
}

void TileMap::draw(sf::RenderTarget& rt, sf::Uint16 index, bool debug)
{
    setDrawingBounds(rt.getView());
    drawLayer(rt, mLayers[index], debug);
}

void TileMap::drawLayer(sf::RenderTarget& rt, MapLayer& layer, bool debug)
{
    rt.draw(layer);
    
    if(debug && layer.type == tmx::ObjectGroup)
    {
        for(const auto& object : layer.objects)
            if(mBounds.intersects(object.getAABB()))
            {
                object.drawDebugShape(rt);
            }
    }
}

void TileMap::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    sf::View view  = rt.getView();
    
    if(view.getCenter() != mLastViewPos)
    {
        sf::FloatRect bounds;
        bounds.left = view.getCenter().x - (view.getSize().x / 2.f);
        bounds.top = view.getCenter().y - (view.getSize().y / 2.f);
        bounds.width = view.getSize().x;
        bounds.height = view.getSize().y;
        
        //add a tile border to prevent gaps appearing
        bounds.left -= static_cast<float>(mTileWidth);
        bounds.top -= static_cast<float>(mTileHeight);
        bounds.width += static_cast<float>(mTileWidth * 2);
        bounds.height += static_cast<float>(mTileHeight * 2);
        mBounds = bounds;
        
        for(auto& layer : mLayers)
            layer.cull(mBounds);
    }
    mLastViewPos = view.getCenter();

	for(auto& layer : mLayers)
		rt.draw(layer);
}

void TileMap::setDrawingBounds(const sf::View& view)
{
    if(view.getCenter() != mLastViewPos)
	{
		sf::FloatRect bounds;
		bounds.left = view.getCenter().x - (view.getSize().x / 2.f);
		bounds.top = view.getCenter().y - (view.getSize().y / 2.f);
		bounds.width = view.getSize().x;
		bounds.height = view.getSize().y;

		//add a tile border to prevent gaps appearing
		bounds.left -= static_cast<float>(mTileWidth);
		bounds.top -= static_cast<float>(mTileHeight);
		bounds.width += static_cast<float>(mTileWidth * 2);
		bounds.height += static_cast<float>(mTileHeight * 2);
		mBounds = bounds;

		for(auto& layer : mLayers)
			layer.cull(mBounds);
	}
	mLastViewPos = view.getCenter();
}

void TileMap::updateQuadTree(const sf::FloatRect& rootArea)
{
    mRootNode.clear(rootArea);
    for(const auto& layer : mLayers)
    {
        for(const auto& object : layer.objects)
        {
            mRootNode.insert(object);
        }
    }
    mQuadTreeAvailable = true;
}

std::vector<MapObject*> TileMap::QueryQuadTree(const sf::FloatRect& testArea)
{
    // quad tree must be updated at least once with udateQuadTree before we call this
    assert(mQuadTreeAvailable);
    return mRootNode.retrieve(testArea);
}
    
bool TileMap::quadTreeAvailable() const
{
}
    
unsigned int TileMap::getTileWidth() const
{
    return mTileWidth;
}

unsigned int TileMap::getTileHeight() const
{
    return mTileHeight;
}

unsigned int TileMap::getNumCols() const
{
    return mCols;
}

unsigned int TileMap::getNumRows() const
{
    return mRows;
}

unsigned int TileMap::getMapWidth() const
{
    return mCols * mTileHeight;
}

unsigned int TileMap::getMapHeight() const
{
    return mRows * mTileWidth;
}

std::vector<MapLayer>& TileMap::getLayers()
{
    return mLayers;
}

const std::vector<MapLayer>& TileMap::getLayers() const
{
    return mLayers;
}

std::string TileMap::getPropertyString(const std::string& name)
{
	assert(mProperties.find(name) != mProperties.end());
	return mProperties[name];
}

void TileMap::setLayerShader(sf::Uint16 layerId, const sf::Shader& shader)
{
    mLayers[layerId].setShader(shader);
}

void TileMap::unLoad()
{ 
    mTilesetTextures.clear();
    mTileInfo.clear();
    mLayers.clear();
    mProperties.clear();
    mFailedImage = false;
}

bool TileMap::loadMap(const std::string& filename)
{
    // Clear any old data first
    unLoad();

    // parse map xml, return on error.
    pugi::xml_document mapDoc;
    pugi::xml_parse_result result = mapDoc.load_file(filename.c_str());
    if(!result)
    {
        LOG_ERR("Failed to open <" + filename);
        LOG_ERR("Reason: " + std::string(result.description()));
        return false;
    }
    
    // Set map properties
    pugi::xml_node mapNode = mapDoc.child("map");
    if(!mapNode)
    {
        LOG_ERR("Map node not found. Map <" + filename + "> not loaded.");
        return false;
    }
    
    if(!parseMapNode(mapNode)) return false;
    if(!parseTilesets(mapNode)) return false;
    
    // Traverse map node children and parse each layer.
    pugi::xml_node currentNode = mapNode.first_child();
    while(currentNode)
    {
        std::string name = currentNode.name();
        if(name == "layer")
        {
            if(!parseLayer(currentNode))
            {
                unLoad();
                return false;
            }
        }
        else if(name == "imagelayer")
        {
            if(!parsetImageLayer(currentNode))
            {
                unLoad();
                return false;
            }
        }
        else if(name == "objectgroup")
        {
            if(!parseObjectGroup(currentNode))
            {
                unLoad();
                return false;
            }
        }
        currentNode = currentNode.next_sibling();
    }
    
    createDebugGrid();
    
    LOG_INF("Parsed " + std::to_string(mLayers.size()) + " layers.");
    LOG_INF("Loaded <" + filename + "> successfully.");
    
    return true;
}

bool TileMap::parseMapNode(const pugi::xml_node& mapNode)
{
    if(!(mTileWidth = mapNode.attribute("tilewidth").as_int()) 
    || !(mTileHeight = mapNode.attribute("tileheight").as_int())
    || !(mCols = mapNode.attribute("width").as_int())
    || !(mRows = mapNode.attribute("height").as_int()))
    {
        LOG_ERR("Invalid tile size found, check map data. Map not loaded.");
        return false;
    }
	
	std::string orientation = mapNode.attribute("orientation").as_string();
	if(orientation != "orthogonal")
    {
        LOG_ERR("Map orientation " + orientation + " not currently supported. Map not loaded.");
        return false;
    }
    
    // parse any map properties
    if(pugi::xml_node propertiesNode = mapNode.child("properties"))
    {
        pugi::xml_node propertyNode = propertiesNode.child("property");
        while(propertyNode)
        {
            std::string name = propertyNode.attribute("name").as_string();
            std::string value = propertyNode.attribute("value").as_string();
            mProperties[name] = value;
            propertyNode = propertyNode.next_sibling("property");
            LOG_INF("Added map property " + name + " with value " + value); 
        }
    }
	
	return true;
}

bool TileMap::parseTilesets(const pugi::xml_node& mapNode)
{
    pugi::xml_node tileset;
    if(!(tileset = mapNode.child("tileset")))
    {
        LOG_WRN("No tilesets found.");
        return false;
    }
    LOG_INF("Caching image files, please wait...");
    
    // Empty vertex tile
    mTileInfo.push_back(TileInfo());
    
    // parse tile sets in order so GUIDs match index.
    while(tileset)
    {
        // if source attrib parse external tsx
        if(tileset.attribute("source"))
        {
            // try loading tsx
            std::string file = fileFromPath(tileset.attribute("source").as_string());
            std::string path = resourcePath() + file;
            
            pugi::xml_document tsxDoc;
            pugi::xml_parse_result result;
            
            result = tsxDoc.load_file(path.c_str());
            if(!result)
            {
                LOG_ERR("Failed to open external tsx doucment: " + path);
                LOG_ERR("Reason: " + std::string(result.description()));
                unLoad();
                return false;
            }
            
            // try parsing tileset node
            pugi::xml_node ts = tsxDoc.child("tileset");
            
            if(!processTiles(ts)) return false;
        }
        else // try for tmx map file data
        {
            if(!processTiles(tileset)) return false;
        }
        
        // move on to next tileset node
        tileset = tileset.next_sibling("tileset");
    }
    
    return true;
}
 
bool TileMap::processTiles(const pugi::xml_node& tilesetNode)
{ 
    sf::Uint16 tileWidth, tileHeight, spacing, margin;
    
    // try and parse tile sizes
    if(!(tileWidth = tilesetNode.attribute("tilewidth").as_int())
    || !(tileHeight = tilesetNode.attribute("tileheight").as_int()))
    {
        LOG_ERR("Invalid tileset data found. Map not loaded.");
        unLoad();
        return false;
    }
    spacing = (tilesetNode.attribute("spacing")) ? tilesetNode.attribute("spacing").as_int() : 0u;
    margin = (tilesetNode.attribute("margin")) ? tilesetNode.attribute("margin").as_int() : 0u;
    
    // try parsing image node
    pugi::xml_node imageNode;
    if(!(imageNode = tilesetNode.child("image")) || !imageNode.attribute("source"))
    {
        LOG_ERR("Missing image data in tmx file. Map not loaded.");
        unLoad();
        return false;
    }
    
    // process image from disk
    std::string imageName = fileFromPath(imageNode.attribute("source").as_string());
    sf::Image sourceImage = loadImage(resourcePath() + imageName);
    if(mFailedImage)
    {
        LOG_ERR("Failed to load image " + imageName);
        return false;
    }
    
    // add transparency mask from colour if it exists
    if(imageNode.attribute("trans"))
        sourceImage.createMaskFromColor(colorFromHex(imageNode.attribute("trans").as_string()));
        
    // store image as a texture for drawing with vertex array
    std::unique_ptr<sf::Texture> tileset(new sf::Texture);
    tileset->loadFromImage(sourceImage);
    mTilesetTextures.push_back(std::move(tileset));

    // parse offset node if it exists - TODO store somewhere tileset info can be referenced
    sf::Vector2u offset;
    if(pugi::xml_node offsetNode = tilesetNode.child("tileoffset"))
    {
        offset.x = (offsetNode.attribute("x")) ? offsetNode.attribute("x").as_uint() : 0u;
        offset.y = (offsetNode.attribute("y")) ? offsetNode.attribute("y").as_uint() : 0u;
    }
    // TODO parse any tile properties and store with offset above
    
    // slice into tiles
    int columns = (sourceImage.getSize().x - 2u * margin + spacing) / (tileWidth + spacing);
    int rows = (sourceImage.getSize().y - 2u * margin + spacing) / (tileHeight + spacing);
    
    for(int y = 0; y < rows; y++)
    {
        for(int x = 0; x < columns; x++)
        {
            sf::IntRect rect; // must account for any spacing or margin on the tileset.
            rect.top = (y * (tileHeight + spacing)) + margin;
            rect.height = tileHeight;
            rect.left = (x * (tileWidth + spacing)) + margin;
            rect.width = tileWidth;
            
            // store texture coords and tileset index for vertex array
            mTileInfo.push_back(TileInfo(rect,
                sf::Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height)),
                mTilesetTextures.size() - 1u));
        }
    }
    
    LOG_INF("Processed " + imageName);
    return true;
}

bool TileMap::parseLayer(const pugi::xml_node& layerNode)
{	
    LOG_INF("Found standard map layer " + std::string(layerNode.attribute("name").as_string()));
    
    MapLayer layer(tmx::Layer);
    if(layerNode.attribute("name")) layer.name = layerNode.attribute("name").as_string();
    if(layerNode.attribute("opacity")) layer.opacity = layerNode.attribute("opacity").as_float();
    if(layerNode.attribute("visible")) layer.visible = layerNode.attribute("visible").as_bool();
    
    pugi::xml_node dataNode;
    if(!(dataNode = layerNode.child("data")))
    {
        LOG_ERR("Layer data missing or corrupt. Map not loaded.");
        return false;
    }
    
    // Decode and decomplress data first if necessary. See https://github.com/bjorn/tiled/wiki/TMX-Map-Format#data
	//for explanation of bytestream retrieved when using compression
    if(dataNode.attribute("encoding"))
    {
        std::string encoding = dataNode.attribute("encoding").as_string();
        std::string data = dataNode.text().as_string();
        
        if(encoding == "base64")
        {
            LOG_INF("Found Base64 encoded layer data, decoding ...");
            // remove any newlines or white space create by tab spaces in document.
            std::stringstream ss;
            ss << data;
            ss >> data;
            data = base64Decode(data);
            
            // clac the expected size of the uncompressed string
            int expectedSize = mCols * mRows * 4; // number of tiles * 4 bytes = 32 bits/tile
            std::vector<unsigned char> byteArray; // to hold decompressed data as bytes;
            byteArray.reserve(expectedSize);
            
            // check for compression (only used with base64 encoded data)
            if(dataNode.attribute("compression"))
            {
                std::string compression = dataNode.attribute("compression").as_string();
                LOG_INF("Found " + compression + " compressed layer data, decompressing...");
                
                // decompress with zlib
                int dataSize = data.length() * sizeof(unsigned char);
                if(!decompress(data.c_str(), byteArray, dataSize, expectedSize))
                {
                    LOG_ERR("Failed to decompress map data. Map not loaded.");
                    return false;
                }
            }
            else // uncompressed 
            {            
                byteArray.insert(byteArray.end(), data.begin(), data.end());    
            }
            
            //extract tile GIDs using bitshift (See https://github.com/bjorn/tiled/wiki/TMX-Map-Format#data) and add the tiles to layer
            sf::Uint16 x, y;
            x = y = 0;
            for(int i = 0; i < expectedSize - 3; i += 4)
            {
                sf::Uint32 tileGID = byteArray[i] | byteArray[i + 1] << 8 || byteArray[i + 2] << 16 | byteArray[i + 3] << 24;
                // resolve rotation
			    addTileToLayer(layer, x, y, tileGID);
                
                x++;
                if(x == mCols)
                {
                    x = 0;
                    y++;
                }
            }
        }
        else if(encoding == "csv")
        {
            LOG_INF("CSV encoded layer data found.");
            
            std::vector<sf::Uint32> tileGIDs;
            std::stringstream datastream(data);
            
            // parse csv string into vector of IDs
            sf::Uint32 i;
            while(datastream >> i)
            {
                tileGIDs.push_back(i);
                if(datastream.peek() == ',')
                    datastream.ignore();
            }
            
            // create tiles from IDs
            sf::Uint16 x, y;
            x = y = 0;
            unsigned int len = tileGIDs.size();
            for(int i = 0; i < len; i++)
            {
                // resolve rotation
			    addTileToLayer(layer, x, y, tileGIDs[i]);
                
                x++;
                if(x == mCols)
                {
                    x = 0;
                    y++;
                }
            }
        }
        else
        {
            LOG_ERR("Unsupported encoding of layer data found. Map not loaded.");
            return false;
        }
    }
    else // not encoded
    {
        LOG_INF("Found unencoded data.");
        pugi::xml_node tileNode;
        if(!(tileNode = dataNode.child("tile")))
        {
            LOG_ERR("No tile data found. Map not loaded.");
            return false;
        }
        
        sf::Uint16 x, y;
        x = y = 0;
        while(tileNode)
        {
            sf::Uint32 gid = tileNode.attribute("gid").as_uint();
            // resolve rotation
            
			addTileToLayer(layer, x, y, gid);
            
            tileNode = tileNode.next_sibling("tile");
            x++;
            if(x == mCols)
            {
                x = 0;
                y++;
            }
        }
    }
    
    // parse any layer properties
    if(pugi::xml_node propertiesNode = layerNode.child("properties"))
        parseLayerProperties(propertiesNode, layer);
        
    // TODO convert layer tile coords to isometric if needed
    
    mLayers.push_back(layer);
    return true;
}

bool TileMap::parseObjectGroup(const pugi::xml_node& groupNode)
{
    LOG_INF("Found object layer " + std::string(groupNode.attribute("name").as_string()));
    
    pugi::xml_node objectNode;
    if(!(objectNode = groupNode.child("object")))
    {
        LOG_WRN("Object froup contains no objects");
        return true;
    }
    
    // add layer to map layers.
    MapLayer layer(tmx::ObjectGroup);
    
    layer.name = groupNode.attribute("name").as_string();
    if(groupNode.attribute("opacity")) layer.opacity = groupNode.attribute("opacity").as_float();
    if(pugi::xml_node propertiesNode = groupNode.child("properties"))
    {
        parseLayerProperties(propertiesNode, layer);
    }
    
    // NOTE we push layer onto the vector at the end of the function in case we add any objects
    // with tile data to the layer's tiles property.
    
    // parse all object nodes into MapObjects
    while(objectNode)
    {
        if(!objectNode.attribute("x") || !objectNode.attribute("y"))
        {
            LOG_ERR("Object missing position data. Map not loaded.");
            unLoad();
            return false;
        }
        MapObject object;
        
        // set position
        sf::Vector2f position(objectNode.attribute("x").as_float(),
                              objectNode.attribute("y").as_float());
        object.setPosition(position);
        
        // set size if specified
        if(objectNode.attribute("width") && objectNode.attribute("height"))
        {
            sf::Vector2f size(objectNode.attribute("width").as_float(),
                              objectNode.attribute("height").as_float());
            if(objectNode.child("ellipse"))
            {
                // add points to make ellipse
                const float x = size.x / 2.f;
                const float y = size.y / 2.f;
                const float tau = 6.283185f;
                const float step = tau / 16.f; // number of points to make up ellipse
                for(float angle = 0.f; angle < tau; angle += step)
                {
                    sf::Vector2f point(x + x * cos(angle), y + y * sin(angle));
                    object.addPoint(point);
                }
                
                if(size.x == size.y) object.setShapeType(Circle);
                else object.setShapeType(Ellipse);
            }
            else // add points for rectangle to use in intersection testing
            {
                object.addPoint(sf::Vector2f());
                object.addPoint(sf::Vector2f(size.x, 0.f));
                object.addPoint(sf::Vector2f(size.x, size.y));
                object.addPoint(sf::Vector2f(0.f, size.y));
            }
            object.setSize(size);
        }
        // else parse poly points
        else if(objectNode.child("polygon") || objectNode.child("polyline"))
        {
            if(objectNode.child("polygon"))
                object.setShapeType(Polygon);
            else 
                object.setShapeType(Polyline);
            
            // split coords into pairs
            if(objectNode.first_child().attribute("points"))
            {
                LOG_INF("Processing poly shape...");
                std::string pointlist = objectNode.first_child().attribute("points").as_string();
                std::stringstream stream(pointlist);
                std::vector<std::string> points;
                std::string pointstring;
                while(std::getline(stream, pointstring, ' '))
                    points.push_back(pointstring);
                    
                // parse each pair into sf::vector2i
                for(unsigned int i = 0; i < points.size(); i++)
                {
                    std::vector<float> coords;
                    std::stringstream coordstream(points[i]);
                    
                    float j;
                    while(coordstream >> j)
                    {
                        coords.push_back(j);
                        if(coordstream.peek() == ',')
                            coordstream.ignore();
                    }
                    object.addPoint(sf::Vector2f(coords[0], coords[1]));
                }
            }
            else
            {
                LOG_WRN("Points for poylgon or polyline object are missing");
            }
        }
        else if(!objectNode.attribute("gid")) // invalid attributes
        {
            LOG_WRN("Objects with no parameters found, skipping...");
            objectNode = objectNode.next_sibling("object");
            continue;
        }
        
        // parse object node property values
        if(pugi::xml_node propertiesNode = objectNode.child("properties"))
        {
            pugi::xml_node propertyNode = propertiesNode.child("property");
            while(propertyNode)
            {
                std::string name = propertyNode.attribute("name").as_string();
                std::string value = propertyNode.attribute("value").as_string();
                object.setProperty(name, value);
                
                LOG_INF("Set object property " + name + " with value " + value);
                propertyNode = propertyNode.next_sibling("property");
            }
        }
        
        //set object properties
        if(objectNode.attribute("name")) object.setName(objectNode.attribute("name").as_string());
        if(objectNode.attribute("type")) object.setType(objectNode.attribute("type").as_string());
        //if(objectNode.attribute("rotation")) TODO handle rotation attribute
        if(objectNode.attribute("visible")) object.setVisible(objectNode.attribute("visible").as_bool());
        if(objectNode.attribute("gid"))
        {
            sf::Uint32 gid = objectNode.attribute("gid").as_int();
            
            LOG_INF("Found object with tile GID " + gid);
            object.move(0.f, static_cast<float>(-mTileHeight)); // offset for tile origins being at the bottom in tiled.
            const sf::Uint16 x = static_cast<sf::Uint16>(object.getPosition().x / mTileWidth);
            const sf::Uint16 y = static_cast<sf::Uint16>(object.getPosition().y / mTileHeight);
            
            sf::Vector2f offset((object.getPosition().x - (x * mTileWidth)), (object.getPosition().y - (y * mTileHeight)));
            object.setQuad(addTileToLayer(layer, x, y, gid, offset));
            object.setShapeType(Tile);
            
            TileInfo info = mTileInfo[gid];
            // create bounding poly
            float width = static_cast<float>(info.size.x);
            float height = static_cast<float>(info.size.y);
            
            object.addPoint(sf::Vector2f());
            object.addPoint(sf::Vector2f(width, 0.f));
            object.addPoint(sf::Vector2f(width, height));
            object.addPoint(sf::Vector2f(0.f, height));
            
            // move object if tile not map tile size
            if(info.size.y != mTileHeight)
                object.move(0.f, static_cast<float>(mTileHeight - info.size.y) / 2.f);
        }
        object.setParent(layer.name);
        
        // call objects create debug shape function with colour / opacity
        sf::Color debugColor;
        if(groupNode.attribute("color"))
        {
            std::string color = groupNode.attribute("color").as_string();
            // crop leading hash and pop the last (duplicated) char
            std::remove(color.begin(), color.end(), '#');
            color.pop_back();
            debugColor = colorFromHex(color.c_str());
        }
        else
        {
            debugColor = sf::Color(127u, 127u, 127u);
        }
        debugColor.a = static_cast<sf::Uint8>(255.f * layer.opacity);
        object.createDebugShape(debugColor);
        
        // creates line segments from any available points
        object.createSegments();
        
        // add object to vector
        layer.objects.push_back(object);
        objectNode = objectNode.next_sibling("object");
    }
    
    mLayers.push_back(layer);
    LOG_INF("Processed " + std::to_string(layer.objects.size()) + " object.");
    return true;
}

bool TileMap::parsetImageLayer(const pugi::xml_node& imageLayerNode)
{
    LOG_INF("Found image layer " + std::string(imageLayerNode.attribute("name").as_string()));
    
    pugi::xml_node imageNode;
    // load image.
    if(!(imageNode = imageLayerNode.child("image")) || !imageNode.attribute("source"))
    {
        LOG_ERR("Image layer " + std::string(imageLayerNode.attribute("name").as_string()) + " missing image source property. Map not loaded.");
        return false;
    }
    
    std::string imageName = imageNode.attribute("source").as_string();
    sf::Image image = loadImage(resourcePath() + imageName);
    if(mFailedImage)
    {
        LOG_ERR("Failed to load image at " + imageName);
        return false;
    }
    
    // set transparency if required
    if(imageNode.attribute("trans"))
    {
        image.createMaskFromColor(colorFromHex(imageNode.attribute("trans").as_string()));
    }
    
    // load image to texture
    std::unique_ptr<sf::Texture> texture(new sf::Texture);
    texture->loadFromImage(image);
    mImageLayerTextures.push_back(std::move(texture));
    
    // Add texture to layer as sprite, set layer properties
    MapTile tile;
    tile.sprite.setTexture(*mImageLayerTextures.back());
    
    MapLayer layer(tmx::ImageLayer);
    layer.name = imageLayerNode.attribute("name").as_string();
    if(imageLayerNode.attribute("opacity"))
    {
        layer.opacity = imageLayerNode.attribute("opacity").as_float();
        sf::Uint8 opacity = static_cast<sf::Uint8>(255.f * layer.opacity);
        tile.sprite.setColor(sf::Color(255u, 255u, 255u, opacity));
    }
    layer.tiles.push_back(tile);
    
    // parse layer properties
    if(pugi::xml_node propertiesNode = imageLayerNode.child("properties"))
        parseLayerProperties(propertiesNode, layer);
        
    // push back layer
    mLayers.push_back(layer);
    
    return true;
}

void TileMap::parseLayerProperties(const pugi::xml_node& propertiesNode, MapLayer& destLayer)
{
	pugi::xml_node propertyNode = propertiesNode.child("property");
	while(propertyNode)
	{
		std::string name = propertyNode.attribute("name").as_string();
		std::string value = propertyNode.attribute("value").as_string();
		destLayer.properties[name] = value;
		propertyNode = propertyNode.next_sibling("property");
		LOG_INF("Added layer property " + name + " with value " + value);
	}
}

TileQuad* TileMap::addTileToLayer(MapLayer& layer, sf::Uint16 x, sf::Uint16 y, sf::Uint32 gid, const sf::Vector2f& offset)
{
    sf::Uint8 opacity = static_cast<sf::Uint8>(255.f * layer.opacity);
    sf::Color color = sf::Color(255u, 255u, 255u, opacity);
    
    // Get bits and tile id
    std::pair<sf::Uint32, std::bitset<3>> idAndFlags = resolveRotation(gid);
    gid = idAndFlags.first;
    
    // update the layer's tile set(s)
    sf::Vertex v0, v1, v2, v3;
    
    // applying half pixel trick avoid artifacting when scrolling
    v0.texCoords = mTileInfo[gid].coords[0] + sf::Vector2f(0.5f, 0.5f);
    v1.texCoords = mTileInfo[gid].coords[1] + sf::Vector2f(-0.5f, 0.5f);
    v2.texCoords = mTileInfo[gid].coords[2] + sf::Vector2f(-0.5f, -0.5f);
    v3.texCoords = mTileInfo[gid].coords[3] + sf::Vector2f(0.5f, -0.5f);
    
    // flip texture coordinates according to bits set
    doFlips(idAndFlags.second, &v0.texCoords, &v1.texCoords, &v2.texCoords, &v3.texCoords);
    
    v0.position = sf::Vector2f(static_cast<float>(mTileWidth * x), static_cast<float>(mTileHeight * y));
    v1.position = sf::Vector2f(static_cast<float>(mTileWidth * x) + mTileInfo[gid].size.x, static_cast<float>(mTileHeight * y));
    v2.position = sf::Vector2f(static_cast<float>(mTileWidth * x) + mTileInfo[gid].size.x, static_cast<float>(mTileHeight * y) + mTileInfo[gid].size.y);
    v3.position = sf::Vector2f(static_cast<float>(mTileWidth * x), static_cast<float>(mTileHeight * y) + mTileInfo[gid].size.y);
    
    // offset tiles with size not equal to map grid size
    sf::Uint16 tileHeight = static_cast<sf::Uint16>(mTileInfo[gid].size.y);
    if(tileHeight != mTileHeight)
    {
        float diff = static_cast<float>(mTileHeight - tileHeight);
        v0.position.y += diff;
        v1.position.y += diff;
        v2.position.y += diff;
        v3.position.y += diff;
    }
    
    // TODO adjust position for isometic maps
    
    v0.color = color;
    v1.color = color;
    v2.color = color;
    v3.color = color;
    
    v0.position += offset;
    v1.position += offset;
    v2.position += offset;
    v3.position += offset;
    
    sf::Uint16 id = mTileInfo[gid].tilesetId;
    if(layer.layerSets.find(id) == layer.layerSets.end())
    {
        // create a new layerset for texture
        layer.layerSets.insert(std::make_pair(id, std::make_shared<LayerSet>(*mTilesetTextures[id], mPatchSize, sf::Vector2u(mCols, mRows), sf::Vector2u(mTileWidth, mTileHeight))));
    }
    
    // add tile to set
    return layer.layerSets[id]->addTile(v0, v1, v2, v3, x, y);
}
void TileMap::flipY(sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3)
{
    //Flip Y
    sf::Vector2f tmp = *v0;
    v0->y = v2->y;
    v1->y = v2->y;
    v2->y = tmp.y ;
    v3->y = v2->y;
}
void TileMap::flipX(sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3)
{
    //Flip X
    sf::Vector2f tmp = *v0;
    v0->x = v1->x;
    v1->x = tmp.x;
    v2->x = v3->x;
    v3->x = v0->x;
}
void TileMap::flipD(sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3)
{
    //Diaganol flip
    sf::Vector2f tmp = *v1;
    v1->x = v3->x;
    v1->y = v3->y;
    v3->x = tmp.x;
    v3->y = tmp.y;
}
void TileMap::doFlips(std::bitset<3> bits,sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3)
{
    //000 = no change
    //001 = vertical = swap y axis
    //010 = horizontal = swap x axis
    //011 = horiz + vert = swap both axes = horiz+vert = rotate 180 degrees
    //100 = diag = rotate 90 degrees right and swap x axis
    //101 = diag+vert = rotate 270 degrees right
    //110 = horiz+diag = rotate 90 degrees right
    //111 = horiz+vert+diag = rotate 90 degrees right and swap y axis

    if(!bits.test(0) && !bits.test(1) && !bits.test(2))
    {
        //Shortcircuit tests for nothing to do
        return;
    }
    else if(bits.test(0) && !bits.test(1) && !bits.test(2))
    {
        //001
        flipY(v0,v1,v2,v3);
    }
    else if(!bits.test(0) && bits.test(1) && !bits.test(2))
    {
        //010
        flipX(v0,v1,v2,v3);
    }
    else if(bits.test(0) && bits.test(1) && !bits.test(2))
    {
        //011
        flipY(v0,v1,v2,v3);
        flipX(v0,v1,v2,v3);
    }
    else if(!bits.test(0) && !bits.test(1) && bits.test(2))
    {
        //100
        flipD(v0,v1,v2,v3);
    }
    else if(bits.test(0) && !bits.test(1) && bits.test(2))
    {
        //101
        flipX(v0,v1,v2,v3);
        flipD(v0,v1,v2,v3);


    }
    else if(!bits.test(0) && bits.test(1) && bits.test(2))
    {
        //110
        flipY(v0,v1,v2,v3);
        flipD(v0,v1,v2,v3);

    }
    else if(bits.test(0) && bits.test(1) && bits.test(2))
    {
        //111
        flipY(v0,v1,v2,v3);
        flipX(v0,v1,v2,v3);
        flipD(v0,v1,v2,v3);
    }
}

std::vector<unsigned char> TileMap::intToBytes(sf::Uint32 paramInt)
{
    std::vector<unsigned char> arrayOfByte(4);
    for(int i = 0; i < 4; i++)
        arrayOfByte[i] = (paramInt >> (i * 8));
    return arrayOfByte;
}

std::pair<sf::Uint32, std::bitset<3> > TileMap::resolveRotation(sf::Uint32 gid)
{
    static const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
    static const unsigned FLIPPED_VERTICALLY_FLAG   = 0x40000000;
    static const unsigned FLIPPED_DIAGONALLY_FLAG   = 0x20000000;
    
    std::vector<unsigned char> bytes = intToBytes(gid);
    sf::Uint32 tileGID = bytes[0] | bytes[1] << 8 | bytes[2] << 16 | bytes[3] << 24;
    
    bool flipped_diagonally = (tileGID & FLIPPED_DIAGONALLY_FLAG);
    bool flipped_horizontally = (tileGID & FLIPPED_HORIZONTALLY_FLAG);
    bool flipped_vertically = (tileGID & FLIPPED_VERTICALLY_FLAG);
    
    std::bitset<3> b;
    b.set(0, flipped_vertically);
    b.set(1, flipped_horizontally);
    b.set(2, flipped_diagonally);
    
    tileGID &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);
    
    return std::pair<sf::Uint32, std::bitset<3>>(tileGID, b);
}

void TileMap::createDebugGrid()
{
// TODO isometric grid
	sf::Color debugColour(0u, 0u, 0u, 120u);
	float mapHeight = static_cast<float>(mTileHeight * mRows);
	for(int x = 0; x <= mCols; x += 2)
	{
		float posX = static_cast<float>(x * mTileWidth);
		mGridVertices.append(sf::Vertex(sf::Vector2f(posX, 0.f), debugColour));
		mGridVertices.append(sf::Vertex(sf::Vector2f(posX, mapHeight), debugColour));
		posX += static_cast<float>(mTileWidth);
		mGridVertices.append(sf::Vertex(sf::Vector2f(posX, mapHeight), debugColour));
		mGridVertices.append(sf::Vertex(sf::Vector2f(posX, 0.f), debugColour));
		posX += static_cast<float>(mTileWidth);
		mGridVertices.append(sf::Vertex(sf::Vector2f(posX, 0.f), debugColour));
	}
	float mapWidth = static_cast<float>(mTileWidth * mCols);
	for(int y = 0; y <= mRows; y += 2)
	{
		float posY = static_cast<float>(y *mTileHeight);
		mGridVertices.append(sf::Vertex(sf::Vector2f(0.f, posY), debugColour));
		posY += static_cast<float>(mTileHeight);
		mGridVertices.append(sf::Vertex(sf::Vector2f(0.f, posY), debugColour));
		mGridVertices.append(sf::Vertex(sf::Vector2f(mapWidth, posY), debugColour));
		posY += static_cast<float>(mTileHeight);
		mGridVertices.append(sf::Vertex(sf::Vector2f(mapWidth, posY), debugColour));
	}

	mGridVertices.setPrimitiveType(sf::LinesStrip);
}


 
sf::Image& TileMap::loadImage(const std::string& imageName)
{
    const auto i = mCachedImages.find(imageName);
    if(i != mCachedImages.cend())
        return *i->second;
        
    // else attempt to load.
    std::shared_ptr<sf::Image> newImage = std::make_shared<sf::Image>();
    
    bool loaded = newImage->loadFromFile(imageName);
    if(!loaded)
    {
        newImage->create(20u, 20u, sf::Color::Yellow);
        mFailedImage = true;
    }
    
    mCachedImages[imageName] = newImage;
    return *mCachedImages[imageName];
}
   
std::string TileMap::fileFromPath(const std::string& path)
{
    assert(!path.empty());
    
    for(auto it = path.rbegin(); it != path.rend(); ++it)
    {
        if(*it == '/' || *it == '\\')
        {
            int pos = std::distance(path.rbegin(), it);
            return path.substr(path.size() - pos);
        }
    }
    return path;
}

sf::Color TileMap::colorFromHex(const char* hexStr) const
{
    // TODO proper checking valid string length
    unsigned int value, r, g, b;
    std::stringstream input(hexStr);
    input >> std::hex >> value;
    
    r = (value >> 16) & 0xff;
    g = (value >> 8) & 0xff;
    b = value & 0xff;
    
    return sf::Color(r, g, b);
}

bool TileMap::decompress(const char* source, std::vector<unsigned char>& dest, int inSize, int expectedSize)
{
	if(!source)
	{
		LOG_ERR("Input string is empty, decompression failed.");
		return false;
	}

	int currentSize = expectedSize;
	//TODO switch to std::make_unique when compatible with all compilers
	std::unique_ptr<unsigned char[]> byteArray(new unsigned char[expectedSize / sizeof(unsigned char)]);
	z_stream stream;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	stream.next_in = (Bytef*)source;
	stream.avail_in = inSize;
	stream.next_out = (Bytef*)byteArray.get();
	stream.avail_out = expectedSize;

	if(inflateInit2(&stream, 15 + 32) != Z_OK)
	{
		LOG_ERR("inflate 2 failed");
		return false;
	}

	int result = 0;
	do
	{
		result = inflate(&stream, Z_SYNC_FLUSH);

		switch(result)
		{
		case Z_NEED_DICT:
		case Z_STREAM_ERROR:
			result = Z_DATA_ERROR;
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			inflateEnd(&stream);
			LOG_ERR(std::to_string(result));
			return false;
		}

		if(result != Z_STREAM_END)
		{
			int oldSize = currentSize;
			currentSize *= 2;
			std::unique_ptr<unsigned char[]> newArray(new unsigned char[currentSize / sizeof(unsigned char)]);
			std::memcpy(newArray.get(), byteArray.get(), currentSize / 2);
			byteArray = std::move(newArray);
			
			stream.next_out = (Bytef*)(byteArray.get() + oldSize);
			stream.avail_out = oldSize;

		}
	}
	while(result != Z_STREAM_END);

	if(stream.avail_in != 0)
	{
		LOG_ERR("stream.avail_in is 0");
		LOG_ERR("zlib decompression failed.");
		return false;
	}

	const int outSize = currentSize - stream.avail_out;
	inflateEnd(&stream);

	std::unique_ptr<unsigned char[]> newArray(new unsigned char[outSize / sizeof(unsigned char)]);
	std::memcpy(newArray.get(), byteArray.get(), outSize);
	byteArray = std::move(newArray);

	//copy bytes to vector
	int length = currentSize / sizeof(unsigned char);	
	dest.insert(dest.begin(), &byteArray[0], &byteArray[length]);

	return true;
}

TileMap::TileInfo::TileInfo()
 : tilesetId(0u)
{}

TileMap::TileInfo::TileInfo(const sf::IntRect& rect, const sf::Vector2f& size, sf::Uint16 tilesetId)
 : size(size)
 , tilesetId(tilesetId)
{
    coords[0] = sf::Vector2f(static_cast<float>(rect.left), static_cast<float>(rect.top));
    coords[1] = sf::Vector2f(static_cast<float>(rect.left + rect.width), static_cast<float>(rect.top));
    coords[2] = sf::Vector2f(static_cast<float>(rect.left + rect.width), static_cast<float>(rect.top + rect.height));
    coords[3] = sf::Vector2f(static_cast<float>(rect.left), static_cast<float>(rect.top + rect.height));
}

static const std::string base64Chars =
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz"
				"0123456789+/";

static inline bool isBase64(unsigned char c)
{
	return (isalnum(c) || (c == '+') || (c == '/'));
}

static std::string base64Decode(std::string const& encoded_string)
{
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && ( encoded_string[in_] != '=') && isBase64(encoded_string[in_]))
	{
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i ==4)
		{
			for (i = 0; i <4; i++)
				char_array_4[i] = base64Chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = base64Chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++)
			ret += char_array_3[j];
	}

	return ret;
}