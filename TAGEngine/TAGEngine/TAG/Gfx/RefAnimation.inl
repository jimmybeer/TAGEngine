template <typename Animation>
RefAnimation<Animation>::RefAnimation(Animation& referenced)
 : mReferenced(&referenced)
{}

template <typename Animation>
template <typename Animated>
void RefAnimation<Animation>::operator() (Animated& animated, float progress)
{
    (*mReferenced)(animated, progress);
}

template <typename Animation>
RefAnimation<Animation> refAnimation(Animation& referenced)
{
    return RefAnimation<Animation>(referenced);
}