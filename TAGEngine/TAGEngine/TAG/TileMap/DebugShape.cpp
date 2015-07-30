#include "DebugShape.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

DebugShape::DebugShape()
 : mArray(sf::LinesStrip)
 , mClosed(false)
{}

void DebugShape::addVertex(const sf::Vertex& v)
{
    if(mClosed)
    {
        sf::Uint16 i = mArray.getVertexCount() - 1;
        sf::Vertex vt = mArray[i];
        mArray[i] = v;
        mArray.append(vt);
    }
    else
    {
        mArray.append(v);
    }
}

void DebugShape::reset()
{
    mArray.clear();
}

void DebugShape::closeShape()
{
    if(mArray.getVertexCount())
    {
        mArray.append(mArray[0]);
        mClosed = true;
    }
}

void DebugShape::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    states.transform *= getTransform();
    rt.draw(mArray, states);
}