#include "TileMap.hpp"
#include "Layer.hpp"

namespace TagTiles
{

int Layer::LayersCreated = 0;
int Layer::LayersDestroyed = 0;

int Layer::getCreated()
{
    return LayersCreated;
}

int Layer::getDestroyed()
{
    return LayersDestroyed;
}

Layer::Layer(const TileMap& tileMap, const std::string& name, int z, bool isStatic)
 : mTileMap(tileMap)
 , mName(name)
 , mIsStatic(isStatic)
 , mZ(z)
{
    LayersCreated++;
}

Layer::~Layer()
{
    LayersDestroyed++;
}

const std::string& Layer::getName() const
{
    return mName;
}

int Layer::z() const
{
    return mZ;
}

bool Layer::isStatic() const
{
    return mIsStatic;
}

}