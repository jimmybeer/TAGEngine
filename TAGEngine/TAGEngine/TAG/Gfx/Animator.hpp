#pragma once

#include <SFML/System/Time.hpp>

#include <map>
#include <functional>
#include <cassert>
#include <cmath>

// Class that stores the progress of an object's animation.
// The Animator class takes care of multiple animations which are registered by a ID. The animations can be played
// at any time. Animator updates their progress and applies it to animated objects.
template <class Animated, typename Id>
class Animator
{
public:
    // Functor to animate the objects.
	// void (Animated& animated, float progress)
	// animated is the object being animated.
	// a progress is a number in [0,1] determining the animation state.
	typedef std::function<void(Animated&, float)> AnimationFunction;
	
	Animator();
	
	// Copy Constructor
	Animator(const Animator& origin);
	
	// Copy assignment operator
	Animator& operator=(const Animator& origin);
	
	// Move constructor
	Animator(Animator&& source);
	
	// Move assignement operator
	Animator& operator=(Animator&& source);
	
	// Destructor
	~Animator();
	
	// Registers an animation with a given name.
	// id Value that identifies the animation (must not be registered yet).
	// animation Animation to add to the animator. The animation is copied; if you want to insert a reference
	//  instead, use the function refAnimation().
	// duration Duration of the animation.
	void addAnimation(const Id& id, const AnimationFunction& animation, sf::Time duration);
	
	// Plays the animation with the given name.
	// id Value that identifies the animation (must already be registered).
	// loop True if the animation is played repeatedly.
	void playAnimation(const Id& id, bool loop = false);
	
	// Interrupts the animation that is currently active.
	// The animated object remains in the state of the stopped animation. After this call, isPlayingAnimation()
	// returns false.
	void stopAnimation();
	
	// Checks whether an animation is currently playing.
	// Returns true after an animation has been started with playAnimation(), as long as it has not ended.
	// If no animation is playing, false is returned.
	bool isPlayingAnimation() const;
	
	// Returns the ID of the currently playing animation.
	// The behavior is undefined if no animation is playing. Therefore, you have to make sure by a preceding call
	// to isPlayingAnimation() whether it is safe to call getPlayingAnimation().
	Id getPlayingAnimation() const;
	
	// Updates the animator's progress. You should call this method each frame.
	// dt Frame time.
    void update(sf::Time dt);
	
	// Applies the stored animations to an object.
	// animated Object which is animated by the current animation. If no animation is active, the object is left
	// unchanged.
	void animate(Animated& animated) const;
	
private:
    typedef std::pair<AnimationFunction, sf::Time>	ScaledAnimation;
	typedef std::map<Id, ScaledAnimation>			AnimationMap;
	typedef typename AnimationMap::Iterator			AnimationMapIterator;
	
	void playAnimation(AnimationMapIterator animation, bool loop);
	
	template <typename T>
	void adopt(T& source);
	
	AnimationMap mAnimationMap;
	AnimationMapIterator mPlayingAnimation;
	float mProgress;
	bool mLoop;
};

#include "Animator.inl"