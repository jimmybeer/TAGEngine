#pragma once

#include "Layer.hpp"
#include "Tile.hpp"

#include <vector>

namespace TagTiles
{

class TileLayer : public Layer
{
public:
    TileLayer(const TileLayer&) = delete;
	TileLayer& operator=(const TileLayer&) = delete;
	
	TileLayer(const TileMap& tileMap, const std::string& name, int z = 0, bool isStatic=true);
	
	void setDimensions(unsigned int cols, unsigned int rows);
	void setOffsets(int xOff, int yOff);
	void setVisible(bool visible);
	
	int getXOffset() const;
	int getYOffset() const;
	unsigned int getCols() const;
	unsigned int getRows() const;
	bool isVisible() const;
	
	void addTile(int index, int gid);
	
private:
    int mX;    // x-offset
	int mY;    // y-offset
	unsigned int mRows;
	unsigned int mCols;
	bool visible;
	
	std::vector<Tile::Ptr> mTiles;
};

}