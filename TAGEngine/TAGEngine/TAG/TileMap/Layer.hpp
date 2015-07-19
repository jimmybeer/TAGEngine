#pragma once


#include <SFML/Graphics.hpp>

#include <string>

class TileMap;

namespace TagTiles
{

class Layer
{
public:
    Layer(const Layer&) = delete;
	Layer& operator=(const Layer&) = delete;
	static int getCreated();
	static int getDestroyed();
	
	Layer(const TileMap& tileMap, const std::string& name, int z = 0, bool isStatic=true);
	virtual ~Layer();
	virtual void sort() = 0;
	
	const std::string& getName() const;
	
	int z() const;
	
	bool isStatic() const;

protected:
    const bool mIsStatic;
	
	const TileMap& mTileMap;
	
private:
	const std::string& mName;
	
	const int mZ;
	
	static int LayersCreated;
	static int LayersDestroyed;
};

}