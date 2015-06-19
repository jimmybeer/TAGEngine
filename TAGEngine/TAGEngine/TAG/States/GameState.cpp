#include "GameState.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

GameState::GameState(StateStack& stack, Context context)
 : State(stack, context)
 , mWorld(*context.window, *context.fonts)
 , mInputHandler(*context.inputHandler)
{
    mInputHandler.setMissionStatus(InputHandler::MissionRunning);
}

void GameState::draw()
{
    mWorld.draw();
}

bool GameState::update(sf::Time dt)
{
    mWorld.update(dt);
	
	if(!mWorld.hasAlivePlayer())
	{
	    mInputHandler.setMissionStatus(InputHandler::MissionFailure);
		requestStackPush(States::GameOver);
	}
	else if(mWorld.hasPlayerReachedEnd())
	{
	    mInputHandler.setMissionStatus(InputHandler::MissionSuccess);
		requestStackPush(States::GameOver);
	}
	    
	CommandQueue& commands = mWorld.getCommandQueue();
	mInputHandler.handleRealTimeInput(commands);
	
	return true;
}

bool GameState::handleEvent(const sf::Event& event)
{
    // Game input handling
	CommandQueue& commands = mWorld.getCommandQueue();
	mInputHandler.handleEvent(event, commands);
	
	// Escape pressed, trigger pause screen
	if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
	{
	    requestStackPush(States::Pause);
	}
	return true;
}