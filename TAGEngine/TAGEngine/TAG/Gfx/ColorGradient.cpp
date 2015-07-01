#include "ColorGradient.hpp"

#include <iterator>
#include <cassert>

ColorGradient::ColorGradient()
 : mColors()
{}

sf::Color& ColorGradient::operator[] (float position)
{
    assert(position >= 0.f && position <= 1.f);
	
	return mColors[position];
}

sf::Color ColorGradient::sampleColor(float position) const
{
    // Make sure the position 0 and 1 are set.
    assert(mColors.count(0.f) && mColors.count(1.f));
	
	// Find first entry >= position, return color if entry == position.
	auto nextColor = mColors.lower_bound(position);
	if(nextColor->first == position)
	    return nextColor->second;
		
	// Find first entry < position
	auto prevColor = std::prev(nextColor);
	
	// Interpolate color between 2 entries.
	float interpolation = (position - prevColor->first) / (nextColor->first - prevColor->first);
	return blendColors(prevColor->second, nextColor->second, interpolation);
}

sf::Color blendColors(const sf::Color& firstColor, const sf::Color& secondColor, float interpolation)
{
    assert(interpolation >= 0.f && interpolation <= 1.f);
	
	float firstPart = 1.f - interpolation;
	
	return sf::Color(
	    static_cast<sf::Uint8>(firstPart * firstColor.r + interpolation * secondColor.r),
	    static_cast<sf::Uint8>(firstPart * firstColor.g + interpolation * secondColor.g),
	    static_cast<sf::Uint8>(firstPart * firstColor.b + interpolation * secondColor.b),
	    static_cast<sf::Uint8>(firstPart * firstColor.a + interpolation * secondColor.a)
        );
}
