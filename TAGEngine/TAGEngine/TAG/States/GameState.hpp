#pragma once

#include "State.hpp"
#include "World.hpp"
#include "InputHandler.hpp"

class GameState : public State
{
public:
    GameState(StateStack& stack, Context context);
	
	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);
	
private:
    World mWorld;
	InputHandler& mInputHandler;
};