#pragma once

#include "UniformAccess.hpp"

// Lets an object fade in or out.
// Changes the alpha value of animated objects at the beginning and/or end of the animation.
class FadeAnimation
{
public:
    // Constructor
	// inRatio The part of time during which the object is faded @b in. Must be in the interval [0, 1].
	// outRatio The part of time during which the object is faded @b out. Must be in the interval [0, 1-inRatio].
	// Example: Let's say you want an object to fade in during the first quarter of its animation, and to fade out
	//  during the second half of its animation. Therefore, inRatio = 0.25 and outRatio = 0.5.
	FadeAnimation(float inRatio, float outRatio);
	
	// Animates the object.
	// animated Object to fade in and/or out.
	// progress Value in [0,1] determining the progress of the animation.
	// Animated Type of animated object. The function thor::setAlpha() is invoked for it.
	template <class Animated>
	void operator() (Animated& animated, float progress) const;
private:
    float mInRatio;
	float mOutRatio;
};

template <class Animated>
void FadeAnimation::operator() (Animated& target, float progress) const
{
    if(progress < mInRatio)
	{
	    tag::setAlpha(target, static_cast<sf::Uint8>(256 * progress / mInRatio));
	}
	else if(progress > 1.f - mOutRatio)
	{
	    tag::setAlpha(target, static_cast<sf::Uint8>(256 * (1.f - progress) / mOutRatio));
	}
}