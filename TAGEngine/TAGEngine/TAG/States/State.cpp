#include "State.hpp"
#include "StateStack.hpp"

State::Context::Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts, InputHandler& inputHandler)
 : window(&window)
 , textures(&textures)
 , fonts(&fonts)
 , InputHandler(&inputHandler)
{}

State::State(StateStack& stack, Context context)
 : mStack(&stack)
 , mContext(context)
{}

State::~State()
{
}

void State::requestStackPush(States::ID stateID)
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