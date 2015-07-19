#pragma once

#include "BaseTile.hpp"

namespace TagTiles
{
    namespace geometry
	{
	    struct Square : public BaseSquare<Square>
		{
		    static sf::Vector2f mapCoordsToPixel(int x, int y, float scale);
			static sf::Vector2i mapPixelToCoords(float x, float y, float scale);
		};
	}
}