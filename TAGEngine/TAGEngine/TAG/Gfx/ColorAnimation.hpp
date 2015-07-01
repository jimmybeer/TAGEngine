#pragma once

#include "ColorGradient.hpp"
#include "UniformAccess.hpp"

// Changes an object's color smoothly over time.
// This class stores a ColorGradient which is applied to animated objects.
class ColorAnimation
{
public:
    explicit ColorAnimation(const ColorGradient& gradient);
	
	// Animates the object.
	// animated Object to colorize according to the color gradient.
	// progress Value in [0,1] determining the progress of the animation.
	// Animated Type of animated object. The function thor::setColor() is invoked for it.
	template <class Animated>
	void operator() (Animated& animated, float progress) const;
private:
    ColorGradient mGradient;
};

template <class Animated>
void ColorAnimation::operator() (Animated& target, float progress) const
{
    tag::setColor(target, mGradient.sampleColor(progress));
}