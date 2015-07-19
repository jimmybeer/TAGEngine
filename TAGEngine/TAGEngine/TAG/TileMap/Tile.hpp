#pragma once

#include <memory>

namespace sf
{
    class Texture;
	class IntRect;
}

namespace TagTiles
{

class Tile
{
public:
    typedef std::unique_ptr<Tile> Ptr;
	
    Tile(const Tile&) = delete;
	Tile& operator=(const Tile&) = delete;
	static int getCreated();
	static int getDestroyed();
	
	Tile(unsigned int gid);
	~Tile();
	
private:
    unsigned int mGid;
	sf::Texture* mTexture;
	sf::IntRect* mTextureRect;	
	
	static int TilesCreated;
	static int TilesDestroyed;
};

}