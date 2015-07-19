#pragma once

#include <SFML/Graphics/Rect.hpp>

#include <vector>
#include <string>

namespace sf
{
    class Texture;
}

namespace TagTiles
{

class Tileset
{
public:
    Tileset(const Tileset&) = delete;
	Tileset& operator=(const Tileset&) = delete;
	
	Tileset(const std::string& id);
	virtual ~Tileset();
	
	void setTexture(const sf::Texture* texture, unsigned int startingGid, 
                    unsigned int tWidth, unsigned int tHeight, 
				    unsigned int iWidth, unsigned int iHeight);
	const sf::Texture* getTexture();
	
	const sf::IntRect& getTileRect(unsigned int id);
private:
    const std::string mId;
	unsigned int mFirstgid;
	unsigned int mLastgid;
	unsigned int mTileWidth;
	unsigned int mTileHeight;
	unsigned int mImageWidth;
	unsigned int mImageHeight;
	const sf::Texture* mTexture;
	std::vector<sf::IntRect> mTextureRects;	
	unsigned int mRows;
	unsigned int mCols;
};

}