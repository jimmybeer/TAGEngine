#pragma once

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

class DebugShape : public sf::Drawable, public sf::Transformable
{
public:
    DebugShape();
    void addVertex(const sf::Vertex& vert);
    void reset();
    void closeShape();
   
private:
    bool mClosed;
    sf::VertexArray mArray;
    void draw(sf::RenderTarget& rt, sf::RenderStates states) const;
};