#include "Square.hpp"

namespace TagTiles
{
    namespace geometry
	{
	    BASE_SQUARE_INIT(Square);
		
	    sf::Vector2f Square::mapCoordsToPixel(int X, int Y, float scale)
		{
		    return sf::Vector2f(X*scale*height,
			                    Y*scale*height);
		}
		
		sf::Vector2i Square::mapPixelToCoords(float X, float Y, float scale)
		{
		    return round(X/scale/height,
			             Y/scale/height);
		}
	}
}