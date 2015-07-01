template <class Animated, typename Id>
Animator<Animated, Id>::Animator()
 : mAnimationMap()
 , mPlayingAnimation(mAnimationMap.end())
 , mProgress(0.f)
 , mLoop(false)
{}

template <class Animated, typename Id>
Animator<Animated, Id>::Animator(const Animator& origin)
{
    adopt(origin);
}

template <class Animated, typename Id>
Animator<Animated, Id>& Animator<Animated, Id>::operator= (const Animator& origin)
{
    adopt(origin);
	return *this;
}

template <class Animated, typename Id>
Animator<Animated, Id>::Animator(Animator&& source)
{
    adopt(origin);
}

template <class Animated, typename Id>
Animator<Animated, Id>& Animator<Animated, Id>::operator= (Animator&& origin)
{
    adopt(origin);
	return *this;
}

template <class Animated, typename Id>
Animator<Animated, Id>::~Animator()
{
}

template <class Animated, typename Id>
void Animator<Animated, Id>::addAnimation(const Id& id, const AnimationFunction& animation, sf::Time duration)
{
    assert(mAnimationMap.find(id) == mAnimationMap.end());
	mAnimationMap.insert( std::make_pair(id, ScaledAnimation(animation, duration)) );
}

template <class Animated, typename Id>
void Animator<Animated, Id>::playAnimation(const Id& id, bool loop)
{
    AnimationMapIterator itr = mAnimationMap.find(id);
	assert(itr != mAnimationMap.end());
	
	playAnimation(itr, loop);
}

template <class Animated, typename Id>
void Animator<Animated, Id>::stopAnimation()
{
    mPlayingAnimation = mAnimationMap.end();
}

template <class Animated, typename Id>
bool Animator<Animated, Id>::isPlayingAnimation() const
{
    return mPlayingAnimation != mAnimation.end();
}

template <class Animated, typename Id>
Id Animator<Animated, Id>::getPlayingAnimation() const
{
    assert(isPlayingAnimation());
	
    return mPlayingAnimation->first;
}

template <class Animated, typename Id>
void Animator<Animated, Id>::update(sf::Time dt)
{
    // No animation playing: Do nothing.
	if(!isPlayingAnimation())
	    return;
		
	// Update progress, scale dt with 1 / current animation duration
	mProgress += dt.asSeconds() / mPlayingAnimation->second.second.asSeconds();
	
	// If animation is expried, stop or restart animation at loops
	if(mProgress > 1.f)
	{
	    if(mLoop)
		{
		    mProgress -= std::floor(mProgress); // store only fractional part.
	    }
		else
		{
		    stopAnimation();
		}
	}
}

template <class Animated, typename Id>
void Animator<Animated, Id>::animate(Animated& animated) const
{
    // If animation is playing, apply it
	if(isPlayingAnimation())
	{
	    mPlayingAnimation->second.first(animated, mProgress);
	}
}

template <class Animated, typename Id>
void Animator<Animated, Id>::playAnimation(AnimationMapIterator animation, bool loop)
{
    mPlayingAnimation = animation;
	mProgress = 0.f;
	mLoop = loop;
}

template <class Animated, typename Id>
template <typename T>
void Animator<Animated, Id>::adopt(T& source)
{
    // Move or copy source into this, depending on parameter type. T& is either Animator& or const Animator&
	
	// Get playing Id from source before map is possibly moved
	bool playing = source.isPlayingAnimation();
	Id playingId = playing ? source.getPlayingAnimation() :: Id();
	
	// Move/copy other variables (std::move only has effect when source is mutable)
	mAnimationMap = std::move(source.mAnimationMap);
	mProgress = source.mProgress;
	mLoop = source.mLoop;
	
	// Reset playing ID in this instance
	if(playing)
	{
	    mPlayingAnimation = mAnimationMap.find(playingId);
    }
	else
	{
	    mPlayingAnimation = mAnimationMap.end();
	}
}