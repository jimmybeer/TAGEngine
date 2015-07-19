#include "TileLayer.hpp"

#include <utility>
#include <cassert>

TileLayer::TileLayer(const TileMap& tileMap, const std::string& name, int z = 0, bool isStatic=true)
 : Layer(tileMap, name, isStatic)
 , mX(0)
 , mY(0)
 , mRows(0)
 , mCols(0)
 , mTiles()
 , visible(true)
{}

void TileLayer::addTile(int index, int gid)
{
    Tile::Ptr t(new Tile(gid));
	mTiles[index] = std::move(t);
}
	
void TileLayer::setDimensions(unsigned int cols, unsigned int rows)
{
    assert(cols > 0 && rows > 0);
    mCols = cols;
	mRows = rows;
	mTiles.resize(mCols * mRows);
}

void TileLayer::setOffsets(int xOff, int yOff)
{
    mX = xOff;
	mY = yOff;
}

void TileLayer::setVisible(bool visible)
{
    mVisible = visible;
}
	
int TileLayer::getXOffset() const
{
    return mX;
}

int TileLayer::getYOffset() const
{
    return mY;
}

unsigned int TileLayer::getCols() const
{
    return mCols;
}

unsigned int TileLayer::getRows() const
{
    return mRows;
}

bool TileLayer::isVisible() const
{
    return mVisible;
}