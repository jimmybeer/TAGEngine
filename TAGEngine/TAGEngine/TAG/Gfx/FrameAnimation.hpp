#pragma once

#include <SFML/Graphics/Rect.hpp>

#include <vector>
#include <cassert>

namespace detail
{
    // Class that stores a single frame of FrameAnimation
	struct Frame
	{
	    Frame(float duration, const sf::IntRect& subrect)
		 : duration(duration)
		 , subrect(subrect)
		 , origin()
		 , applyOrigin(false)
		{}
		
		Frame(float duration, const sf::IntRect& subrect, sf::Vector2f origin)
		 : duration(duration)
		 , subrect(subrect)
		 , origin(origin)
		 , applyOrigin(true)
		{}
		
		mutable float duration;
		sf::IntRect subrect;
		sf::Vector2f origin;
		bool applyOrigin;
	};
}

// Changes a sprite's subrect over time.
// This class stores multiple frames that represent the sub-rectangle of a texture. The resulting animation consists
// of a sequence of frames that are drawn one after another.
class FrameAnimation
{
public:
    FrameAnimation();
	
	// Adds a frame to the animation, changes only the sub-rect.
	// relativeDuration Duration of the frame relative to the other durations.
	// subrect Rectangle of the sf::Texture that is used for the new frame.
    void addFrame(float relativeDuration, const sf::IntRect& subrect);
	
	// Adds a frame to the animation, changes sub-rect and sprite origin.
	// relativeDuration Duration of the frame relative to the other durations.
	// subrect Rectangle of the sf::Texture that is used for the new frame.
	// origin Position of the coordinate system origin. Is useful when frames have rectangles of different sizes.
	void addFrame(float relativeDuration, const sf::IntRect& subrect, sf::Vector2f origin);
	
	// Animates the object.
	// animated Object to animate.
	// progress Value in [0,1] determining the progress of the animation.
	// Animated Class with member functions void setTextureRect(sf::IntRect) and
	//  void setOrigin(sf::Vector2f), for example sf::Sprite.
	template <class Animated>
	void operator() (Animated&, float progress) const;
	
private:
    void ensureNormalized() const;
	
	std::vector<detail::Frame> mFrames;
	mutable bool mNormalized;
};

template <class Animated>
void FrameAnimation::operator() (Animated& target, float progress) const
{
    assert(!mFrames.empty());
	assert(progress >= 0.f && progress <= 1.f);
	
	ensureNormalized();
    for(const detail::Frame& frame : mFrames)
	{
	    progress -= frame.duration;
		
		if(progress < 0.f)
		{
		    target.setTextureRect(frame.subrect);
			if(frame.applyOrigin)
			{
			    target.setOrigin(frame.origin);
			}
			break;
		}
	}
}