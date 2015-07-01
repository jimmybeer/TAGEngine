#include "FadeAnimation.hpp"
#include <cassert>

FadeAnimation::FadeAnimation(float inRatio, float outRatio)
 : mInRatio(inRatio)
 , mOutRatio(outRatio)
{
    assert(inRatio >= 0.f && inRatio <= 1.f);
	assert(outRatio >= 0.f && outRatio <= 1.f);
	assert(inRatio + outRatio <= 1.f);
}