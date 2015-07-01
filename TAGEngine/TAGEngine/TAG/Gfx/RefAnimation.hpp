#pragma once

// Class to reference another animation.
// This class can be used to create an animation that has no functionality on its own, but references another
// existing animation. By using it, you can avoid copies and change the original animation object, even after it has been
// added to an Animator. On the other side, you must make sure the original animation stays alive while being referenced.
// Usually, you create objects of this class by calling refAnimation() "refAnimation()".
template <typename Animation>
class RefAnimation
{
public:
    explicit RefAnimation(Animation& referenced);
	
	// Animates the object.
	// Forwards the call to the referenced animation.
	template <typename Animated>
	void operator() (Animated& animated, float progress);
	
private:
    Animation* mRefrenced;
};

// RefAnimation
// Creates an animation that references another one.
// referenced Animation to reference. Must remain valid during usage.
// Creates an animation object that has no functionality on its own, but references another existing animation.
// By using it, you can avoid copies and change the original animation object, even after it has been added to a
// thor::Animator. On the other side, you must make sure the original animation stays alive while being referenced.
// Example:
// 
//  // Create animation
//  FrameAnimation anim;
//
//  // Insert reference into animator
//  Animator<...> animator;
//  animator.addAnimation(..., refAnimation(anim), ...);
//
//  // Later changes will affect animation inside animator
//  anim.addFrame(...);
//
template <typename Animation>
RefAnimation<Animation> refAnimation(Animation& referenced);

#include "RefAnimation.inl"