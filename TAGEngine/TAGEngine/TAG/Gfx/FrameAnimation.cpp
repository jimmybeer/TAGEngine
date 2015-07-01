#include "FrameAnimation.hpp"

FrameAnimation::FrameAnimation()
 : mFrames()
 , mNormalized(false)
{}

void FrameAnimation::addFrame(float relativeDuration, const sf::IntRect& subrect)
{
    mFrames.push_back(detail::Frame(relativeDuration, subrect));
	mNormalized = false;
}

void FrameAnimation::addFrame(float relativeDuration, const sf::IntRect& subrect, sf::Vector2f origin)
{
    mFrames.push_back(detail::Frame(relativeDuration, subrect, origin));
	mNormalized = false;
}

void FrameAnimation::ensureNormalized() const
{
    if(mNormalized)
	    return;
	
	float sum = 0.f;
    for(const detail::Frame& frame : mFrames)
	    sum += frame.duration;
		
    for(const detail::Frame& frame : mFrames)
	    frame.duration /= sum;
		
	mNormalized = true;
}