#include "State.hpp"
#include "StateStack.hpp"

State::Context::Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts, InputHandler& inputHandler,
                        MusicPlayer& music, SoundPlayer& sounds)
 : window(&window)
 , textures(&textures)
 , fonts(&fonts)
 , inputHandler(&inputHandler)
 , music(&music)
 , sounds(&sounds)
{}

State::State(StateStack& stack, Context context)
 : mStack(&stack)
 , mContext(context)
{}

State::~State()
{
}

void State::requestStackPush(unsigned int stateID)
{
    mStack->pushState(stateID);
}

void State::requestStackPop()
{
    mStack->popState();
}

void State::requestStackClear()
{
    mStack->clearStates();
}

State::Context State::getContext() const
{
    return mContext;
}