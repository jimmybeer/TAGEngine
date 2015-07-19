 #include "Tileset.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <cassert>
#include <cmath>


namespace TagTiles
{

Tileset::Tileset(const std::string& id)
 : mId(id)
 , mTexture(nullptr)
 , mTextureRects()
 , mFirstgid(0)
 , mLastgid(0)
 , mTileWidth(0)
 , mTileHeight(0)
 , mImageWidth(0)
 , mRows(0)
 , mCols(0)
{}

Tileset::~Tileset()
{
    mTexture = nullptr;
}

void Tileset::setTexture(const sf::Texture* texture, unsigned int startingGid, 
                         unsigned int tWidth, unsigned int tHeight, 
						 unsigned int iWidth, unsigned int iHeight)
{
    mFirstgid = startingGid;
	mTexture = texture;
	
	mTileWidth = tWidth;
	mTileHeight = tHeight;
	
	mImageWidth = iWidth;
	mImageHeight = iHeight;
	
	assert(mTileWidth > 0 && mTileHeight > 0);
	mCols = std::floor(mImageWidth / mTileWidth);
	mRows = std::floor(mImageHeight / mTileHeight);
	
	mLastgid = mCols * mRows + mFirstgid - 1;
	
	for(unsigned int y = 0; y < mRows; ++y)
	{
	    for(unsigned int x = 0; x < mCols; ++x)
		{
		    mTextureRects.push_back(sf::IntRect((x * mTileWidth), (y * mTileHeight), mTileWidth, mTileHeight));
		}
	}
}

const sf::Texture* Tileset::getTexture()
{
    return mTexture;
}
	
const sf::IntRect& Tileset::getTileRect(unsigned int id)
{
    assert(id < mTextureRects.size());
	return mTextureRects[id];
}

}