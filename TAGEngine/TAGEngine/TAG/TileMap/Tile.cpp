#include "Tile.hpp"

namespace TagTiles
{

int Tile::TilesCreated = 0;
int Tile::TilesDestroyed = 0;

int Tile::getCreated()
{
    return TilesCreated;
}

int Tile::getDestroyed()
{
    return TilesDestroyed;
}

Tile::Tile(unsigned int gid)
 : mGid(gid)
 , mTexture(nullptr)
 , mTextureRect(nullptr)
{
    TilesCreated++;
}

Tile::~Tile()
{
    mGid = 0;
	mTexture = nullptr;
	mTextureRect = nullptr;
	TilesDestroyed++;
}

}