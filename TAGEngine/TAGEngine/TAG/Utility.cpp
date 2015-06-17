#include "Utility.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <random>
#include <cmath>
#include <ctime>
#include <cassert>

namespace
{
    std::default_random_engine createRandomEngine()
	{
	    auto seed = static_cast<unsigned long>(std::time(nullptr));
		return std::default_random_engine(seed);
	}
	
	auto RandomEngine = createRandomEngine();
}

void centerOrigin(sf::Sprite& sprite)
{
    sf::FloatRect bounds = sprite.getLocalBounds();
	sprite.setOrigin(std::floor(bounds.left + bounds.width / 2.f), std::floor(bounds.top + bounds.height / 2.f));
}

void centerOrigin(sf::Text& text)
{
    sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(std::floor(bounds.left + bounds.width / 2.f), std::floor(bounds.top + bounds.height / 2.f));
}

float toDegree(float radian)
{
	return 180.f / 3.141592653589793238462643383f * radian;
}

float roRadian(float degree)
{
	return 3.141592653589793238462643383f / 180.f * degree;
}

int randomInt(int exclusiveMax)
{
    std::uniform_int_distribution<> distr(0, exclusiveMax - 1);
	return distr(RandomEngine);
}

float length(sf::Vector2f vector)
{
    return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}

sf::Vector2f unitVector(sf::Vector2f vector)
{
    assert(vector != sf::Vector2f(0.f, 0.f));
	return vector / length(vector);
}

std::string toString(sf::Keyboard::Key key)
{
    #define SFML_KEY_TO_STRING_CASE(KEY) case sf::Keyboard::KEY: return #KEY;
	
	switch(key)
	{
		SFML_KEY_TO_STRING_CASE(Unknown)
		SFML_KEY_TO_STRING_CASE(A)
		SFML_KEY_TO_STRING_CASE(B)
		SFML_KEY_TO_STRING_CASE(C)
		SFML_KEY_TO_STRING_CASE(D)
		SFML_KEY_TO_STRING_CASE(E)
		SFML_KEY_TO_STRING_CASE(F)
		SFML_KEY_TO_STRING_CASE(G)
		SFML_KEY_TO_STRING_CASE(H)
		SFML_KEY_TO_STRING_CASE(I)
		SFML_KEY_TO_STRING_CASE(J)
		SFML_KEY_TO_STRING_CASE(K)
		SFML_KEY_TO_STRING_CASE(L)
		SFML_KEY_TO_STRING_CASE(M)
		SFML_KEY_TO_STRING_CASE(N)
		SFML_KEY_TO_STRING_CASE(O)
		SFML_KEY_TO_STRING_CASE(P)
		SFML_KEY_TO_STRING_CASE(Q)
		SFML_KEY_TO_STRING_CASE(R)
		SFML_KEY_TO_STRING_CASE(S)
		SFML_KEY_TO_STRING_CASE(T)
		SFML_KEY_TO_STRING_CASE(U)
		SFML_KEY_TO_STRING_CASE(V)
		SFML_KEY_TO_STRING_CASE(W)
		SFML_KEY_TO_STRING_CASE(X)
		SFML_KEY_TO_STRING_CASE(Y)
		SFML_KEY_TO_STRING_CASE(Z)
		SFML_KEY_TO_STRING_CASE(Num0)
		SFML_KEY_TO_STRING_CASE(Num1)
		SFML_KEY_TO_STRING_CASE(Num2)
		SFML_KEY_TO_STRING_CASE(Num3)
		SFML_KEY_TO_STRING_CASE(Num4)
		SFML_KEY_TO_STRING_CASE(Num5)
		SFML_KEY_TO_STRING_CASE(Num6)
		SFML_KEY_TO_STRING_CASE(Num7)
		SFML_KEY_TO_STRING_CASE(Num8)
		SFML_KEY_TO_STRING_CASE(Num9)
		SFML_KEY_TO_STRING_CASE(Escape)
		SFML_KEY_TO_STRING_CASE(LControl)
		SFML_KEY_TO_STRING_CASE(LShift)
		SFML_KEY_TO_STRING_CASE(LAlt)
		SFML_KEY_TO_STRING_CASE(LSystem)
		SFML_KEY_TO_STRING_CASE(RControl)
		SFML_KEY_TO_STRING_CASE(RShift)
		SFML_KEY_TO_STRING_CASE(RAlt)
		SFML_KEY_TO_STRING_CASE(RSystem)
		SFML_KEY_TO_STRING_CASE(Menu)
		SFML_KEY_TO_STRING_CASE(LBracket)
		SFML_KEY_TO_STRING_CASE(RBracket)
		SFML_KEY_TO_STRING_CASE(SemiColon)
		SFML_KEY_TO_STRING_CASE(Comma)
		SFML_KEY_TO_STRING_CASE(Period)
		SFML_KEY_TO_STRING_CASE(Quote)
		SFML_KEY_TO_STRING_CASE(Slash)
		SFML_KEY_TO_STRING_CASE(BackSlash)
		SFML_KEY_TO_STRING_CASE(Tilde)
		SFML_KEY_TO_STRING_CASE(Equal)
		SFML_KEY_TO_STRING_CASE(Dash)
		SFML_KEY_TO_STRING_CASE(Space)
		SFML_KEY_TO_STRING_CASE(Return)
		SFML_KEY_TO_STRING_CASE(BackSpace)
		SFML_KEY_TO_STRING_CASE(Tab)
		SFML_KEY_TO_STRING_CASE(PageUp)
		SFML_KEY_TO_STRING_CASE(PageDown)
		SFML_KEY_TO_STRING_CASE(End)
		SFML_KEY_TO_STRING_CASE(Home)
		SFML_KEY_TO_STRING_CASE(Insert)
		SFML_KEY_TO_STRING_CASE(Delete)
		SFML_KEY_TO_STRING_CASE(Add)
		SFML_KEY_TO_STRING_CASE(Subtract)
		SFML_KEY_TO_STRING_CASE(Multiply)
		SFML_KEY_TO_STRING_CASE(Divide)
		SFML_KEY_TO_STRING_CASE(Left)
		SFML_KEY_TO_STRING_CASE(Right)
		SFML_KEY_TO_STRING_CASE(Up)
		SFML_KEY_TO_STRING_CASE(Down)
		SFML_KEY_TO_STRING_CASE(Numpad0)
		SFML_KEY_TO_STRING_CASE(Numpad1)
		SFML_KEY_TO_STRING_CASE(Numpad2)
		SFML_KEY_TO_STRING_CASE(Numpad3)
		SFML_KEY_TO_STRING_CASE(Numpad4)
		SFML_KEY_TO_STRING_CASE(Numpad5)
		SFML_KEY_TO_STRING_CASE(Numpad6)
		SFML_KEY_TO_STRING_CASE(Numpad7)
		SFML_KEY_TO_STRING_CASE(Numpad8)
		SFML_KEY_TO_STRING_CASE(Numpad9)
		SFML_KEY_TO_STRING_CASE(F1)
		SFML_KEY_TO_STRING_CASE(F2)
		SFML_KEY_TO_STRING_CASE(F3)
		SFML_KEY_TO_STRING_CASE(F4)
		SFML_KEY_TO_STRING_CASE(F5)
		SFML_KEY_TO_STRING_CASE(F6)
		SFML_KEY_TO_STRING_CASE(F7)
		SFML_KEY_TO_STRING_CASE(F8)
		SFML_KEY_TO_STRING_CASE(F9)
		SFML_KEY_TO_STRING_CASE(F10)
		SFML_KEY_TO_STRING_CASE(F11)
		SFML_KEY_TO_STRING_CASE(F12)
		SFML_KEY_TO_STRING_CASE(F13)
		SFML_KEY_TO_STRING_CASE(F14)
		SFML_KEY_TO_STRING_CASE(F15)
		SFML_KEY_TO_STRING_CASE(Pause)
	}
	
	return "";
}