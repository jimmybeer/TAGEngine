#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>

namespace TagTiles
{
    namespace geometry
	{
	    template<typename T>
		struct BaseTile
		{
		    public:
		        static const sf::ConvexShape& getShape() { return T::mShape; };
			protected:
			    static sf::ConvexShape mShape;
		};
		
		#define BASE_TILE_INIT(klass) \
		template <> sf::ConvexShape BaseTile<klass>::mShape = sf::ConvexShape();
		
		template<typename T>
		struct _BaseSquare : public BaseTile<T>
		{
		    static sf::Vector2i round(float x, float y)
			{
			    if(x >= 0)
				    x+= 0.5;
				else
				    x -= 0.5;
					
				if(y >= 0)
				    y += 0.5;
				else
				    y -= 0.5;
					
				return sf::Vector2i(x,y);
			}
			
			static sf::IntRect getTextureRect(int x, int y, float scale)
			{
			    sf::Vector2f pos = T::mapCoordsToPixel(x,y,scale);
				sf::IntRect res(pos.x, pos.y,
				                T::width * scale,
								T::height * scale);
				return res;
			}
			
			static int distance(int x1, int y1, int x2, int y2)
			{
			    float x = x1 - x2;
				x = x*x;
				
				float y = y1 - y2;
				y = y*y;
				
				return std::ceil(sqrt(x+y));
			}
		};
		
		template<typename T>
		struct BaseSquare : public _BaseSquare<T>
		{
		public:
		    static constexpr float height = 2;
			static constexpr float width = height;
			
		protected:
            static void init()
            {
			    T::mShape.setPointCount(4);
				T::mShape.setPoint(0, sf::Vector2f(0,0));
				T::mShape.setPoint(1, sf::Vector2f(0,T::height));
				T::mShape.setPoint(2, sf::Vector2f(T::height,T::height));
				T::mShape.setPoint(3, sf::Vector2f(T::height,0));
				
				T::mShape.setOrigin(T::height/2.0, T::height/2.0);
            }

            static struct _Initialiser {
                _Initialiser() {
                    T::init();
                };
			} _initialiser_;
		};
		
		#define BASE_SQUARE_INIT(klass) \
		BASE__TILE_INIT(klass);\
		template <> BaseSquare<klass>::_Initialser BaseSquare<klass>::_initialiser_ = BaseSquare<klass>::_Initialiser();
		
		template<typename T>
		struct BaseSquareIso : public _BaseSquare<T>
		{
		public:
		    static constexpr float height = 1;
			static constexpr float width = 2;
			
		protected:
		    static void init()
			{
			    T::mShape.setPointCount(4);
				T::mShape.setPoint(0, sf::Vector2f(T::width/2.0,0));
				T::mShape.setPoint(1, sf::Vector2f(T::width,T::height/2.0));
				T::mShape.setPoint(2, sf::Vector2f(T::width/2.0,T::height));
				T::mShape.setPoint(3, sf::Vector2f(0,T::height/2.0));
				
				T::mShape.setOrigin(T::width/2.0, T::height/2.0);
            }

            static struct _Initialiser {
                _Initialiser() {
                    T::init();
                };
			} _initialiser_;
		};
		
		#define BASE_SQUARE_ISO_INIT(klass) \
		BASE_TILE_INIT(klass);\
		template <> BaseSquareIso<klass>::_Initialiser BaseSquareIso<klass>::_initialiser_ = BaseSquareIso<klass>::_Initialiser();
	}
}