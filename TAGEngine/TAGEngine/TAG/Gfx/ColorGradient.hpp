#pragma once

#include <SFML/Graphics/Color.hpp>

#include <map>

// Class to implement color gradients
// This class can represent color gradients and calculate the interpolated color at a given point. A color gradient
// consists of a list of positions (float values in [0, 1]) and corresponding colors. At each position, a color is
// sampled exactly; between two positions, a color is interpolated linearly.
class ColorGradient
{
public:
    // Default constructor
	// Creates an empty, invalid color gradient. To initialize it correctly, set the colors via
	// ColorGradient::operator[].
	ColorGradient();
	
	// Inserts a color to the gradient.
	// position Number in [0, 1] that specifies a position in the gradient.
	// A map-like syntax allows you to specify the color at a certain position. For example, the following code
	// creates a gradient consisting of two parts: Red to blue and blue to cyan. The first partial gradient (red to blue)
	// takes up 80% of the whole gradient.
	// 
	//  ColorGradient gradient;
	//  gradient[0.0f] = sf::Color::Red;
	//  gradient[0.8f] = sf::Color::Blue;
	//  gradient[1.0f] = sf::Color::Cyan;
	// 
	// At least both positions 0.f and 1.f must be set before a gradient is valid.
	sf::Color& operator[] (float position);
	
	// Interpolates a color in the gradient.
    // position Number in [0, 1] that specifies a position in the gradient.
	// When you pass 0 (1), the color at the very beginning (end) is returned.
	sf::Color sampleColor(float position) const;
	
private:
    std::map<float, sf::Color> mColors;
};

// ColorGradient
// Blends the colors @a firstColor and @a secondColor, according to the given interpolation.
// firstColor The first color to blend.
// secondColor The second color to blend.
// interpolation Specifies how much of every color is taken. If it has value 0, the first color is
// returned; with value 1, the second color is returned. Every value in ]0,1[ leads to a mixture. Other values
// are not allowed.
sf::Color blendColors(const sf::Color& firstColor, const sf::Color& secondColor, float interpolation);