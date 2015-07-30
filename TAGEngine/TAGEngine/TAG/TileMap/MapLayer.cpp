#include "MapLayer.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cmath>
#include <iostream>

TileQuad::TileQuad(sf::Uint16 i0, sf::Uint16 i1, sf::Uint16 i2, sf::Uint16 i3)
 : mParentSet(nullptr)
 , mPatchIndex(-1)
 , mDirty(false)
{
    mIndices[0] = i0;
    mIndices[1] = i1;
    mIndices[2] = i2;
    mIndices[3] = i3;
}

void TileQuad::move(const sf::Vector2f& distance)
{
    mMovement = distance;
    if(!mDirty && mParentSet)
    {
        mDirty = true;
        mParentSet->mDirtyQuads.push_back(this);
    }
}

LayerSet::LayerSet(const sf::Texture& texture, sf::Uint8 patchSize, const sf::Vector2u& mapSize, const sf::Vector2u tileSize)
 : mTexture(texture)
 , mPatchSize(patchSize)
 , mMapSize(mapSize)
 , mTileSize(tileSize)
 , mPatchCount(std::ceil(static_cast<float>(mapSize.x) / patchSize)+1, std::ceil(static_cast<float>(mapSize.y) / patchSize)+1)
 , mVisible(true)
{
    mPatches.resize(mPatchCount.x * mPatchCount.y);
}

TileQuad* LayerSet::addTile(sf::Vertex vt0, sf::Vertex vt1, sf::Vertex vt2, sf::Vertex vt3, sf::Uint16 x, sf::Uint16 y)
{
    sf::Int32 patchX = static_cast<sf::Int32>(std::ceil(x / mPatchSize));
    sf::Int32 patchY = static_cast<sf::Int32>(std::ceil(y / mPatchSize));
    sf::Int32 patchIndex = mPatchCount.x * patchY + patchX;
    
    mPatches[patchIndex].push_back(vt0);
    mPatches[patchIndex].push_back(vt1);
    mPatches[patchIndex].push_back(vt2);
    mPatches[patchIndex].push_back(vt3);
    
    sf::Uint16 i = mPatches[patchIndex].size() - 4u;
    mQuads.emplace_back(TileQuad::Ptr(new TileQuad(i, i + 1, i + 2, i + 3)));
    mQuads.back()->mParentSet = this;
    mQuads.back()->mPatchIndex = patchIndex;
    
    updateAABB(vt0.position, vt2.position);
    
    return mQuads.back().get();
}

void LayerSet::cull(const sf::FloatRect& bounds)
{
    mVisible = mBoundingBox.intersects(bounds);
    
    // update visible patch indices
    mVisiblePatchStart.x = static_cast<int>(std::floor((bounds.left / mTileSize.x) / mPatchSize));
    mVisiblePatchStart.y = static_cast<int>(std::floor((bounds.top / mTileSize.y) / mPatchSize));
    if(mVisiblePatchStart.x < 0) mVisiblePatchStart.x = 0;
    if(mVisiblePatchStart.y < 0) mVisiblePatchStart.y = 0;
    
    mVisiblePatchEnd.x = static_cast<int>(std::ceil((bounds.width / mTileSize.x) / mPatchSize));
    mVisiblePatchEnd.y = static_cast<int>(std::ceil((bounds.height / mTileSize.y) / mPatchSize));
    if(mVisiblePatchEnd.x > mPatchCount.x) mVisiblePatchEnd.x = mPatchCount.x;
    if(mVisiblePatchEnd.y > mPatchCount.y) mVisiblePatchEnd.y = mPatchCount.y;
    
    mVisiblePatchEnd += mVisiblePatchStart;
}

void LayerSet::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    for(const auto& q : mDirtyQuads)
    {
        q->mDirty = false;
        for(const auto& p : q->mIndices)
        {
            mPatches[q->mPatchIndex][p].position += q->mMovement;
        }
    }
    mDirtyQuads.clear();
    
    if(!mVisible) return;
    
    for(auto x = mVisiblePatchStart.x; x <= mVisiblePatchEnd.x; ++x)
    {
        for(auto y = mVisiblePatchStart.y; y <= mVisiblePatchEnd.y; ++y)
        {
            auto index = y * mPatchCount.x + x;
            if(index < mPatches.size() && !mPatches[index].empty())
            {
                states.texture = &mTexture;
                rt.draw(mPatches[index].data(), static_cast<unsigned>(mPatches[index].size()), sf::Quads, states);
            }
        }
    }
}

void LayerSet::updateAABB(sf::Vector2f position, sf::Vector2f size)
{
    if(mBoundingBox.width == 0.f)
    {
        // Not been set yet so take on initial size
        mBoundingBox = sf::FloatRect(position, size);
        return;
    }
    
    if(position.x < mBoundingBox.left)
        mBoundingBox.left = position.x;
      
    if(position.y < mBoundingBox.top)
        mBoundingBox.top = position.y;
        
    if(size.x > mBoundingBox.width + std::fabs(mBoundingBox.left))
        mBoundingBox.width = size.x + std::fabs(mBoundingBox.left);
        
    if(size.y > mBoundingBox.height + std::fabs(mBoundingBox.top))
        mBoundingBox.height = size.y + std::fabs(mBoundingBox.top);
}

MapLayer::MapLayer(tmx::MapLayerType type)
 : opacity(1.f)
 , visible(true)
 , type(type)
 , mShader(nullptr)
{}

void MapLayer::setShader(const sf::Shader& shader)
{
    mShader = & shader;
}

void MapLayer::cull(const sf::FloatRect& bounds)
{
    for(auto& ls : layerSets)
        ls.second->cull(bounds);
}

void MapLayer::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    if(!visible) return;
    
    states.shader = mShader;
    for(const auto& ls : layerSets)
    {
        rt.draw(*ls.second, states);
    }
    
    if(type == tmx::ImageLayer)
    {
        for(const auto& tile : tiles)
        {
            rt.draw(tile.sprite, tile.renderStates);
        }
    }
}