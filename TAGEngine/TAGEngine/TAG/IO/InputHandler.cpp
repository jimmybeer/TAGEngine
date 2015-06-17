#include "InputHandler.hpp"
#include "CommandQueue.hpp"
#include "SceneNode.hpp"

#include <map>
#include <string>
#include <algorithm>
#include <iostream>

struct DebugAction
{
	DebugAction(const std::string& text)
	: mText(text)
	{
	}

	void operator() (SceneNode& sn, sf::Time) const
	{
		sn.debugAction(mText);
	}

	std::string mText;
};

InputHandler::InputHandler()
{
    // set initial key bindings
	mKeyBinding[sf::Keyboard::Left] = MoveLeft;
	mKeyBinding[sf::Keyboard::Right] = MoveRight;
	mKeyBinding[sf::Keyboard::Up] = MoveUp;
	mKeyBinding[sf::Keyboard::Down] = MoveDown;
	
    initialiseActions();
	
    for(auto& pair : mActionBinding)
	{
        pair.second.category = 0;
	}
}

void InputHandler::handleEvent(const sf::Event& event, CommandQueue& commands)
{
    if(event.type == sf::Event::KeyPressed)
	{
	    // Check if pressed key appears in the key binding, trigger command if so
		auto found = mKeyBinding.find(event.key.code);
		if(found != mKeyBinding.end() && !isRealTimeAction(found->second))
		{
		    commands.push(mActionBinding[found->second]);
		}
	}
}

void InputHandler::handleRealTimeInput(CommandQueue& commands)
{
    // Traverse all assigned keys and check if they are pressed
	for(auto pair: mKeyBinding)
    {
	    if(sf::Keyboard::isKeyPressed(pair.first) && isRealTimeAction(pair.second))
		{
		    commands.push(mActionBinding[pair.second]);
		}
	}
}

void InputHandler::assignKey(Action action, sf::Keyboard::Key key)
{
    // Remove all keys that already map to action
	for(auto itr = mKeyBinding.begin(); itr != mKeyBinding.end(); )
	{
	    if(itr->second == action)
		{
		    mKeyBinding.erase(itr++);
		}
		else
		{
		   ++itr;
		}
	}
	
	// Insert new binding
	mKeyBinding[key] = action;
}

sf::Keyboard::Key InputHandler::getAssignedKey(Action action) const
{
    for(auto& pair: mKeyBinding)
	{
	    if(pair.second == action)
		{
		    return pair.first;
		}
	}
	
	return sf::Keyboard::Unknown;
}

void InputHandler::initialiseActions()
{
    mActionBinding[MoveLeft].action	 = derivedAction<SceneNode>(DebugAction("MoveLeft Triggered"));
    mActionBinding[MoveRight].action = derivedAction<SceneNode>(DebugAction("MoveRight Triggered"));
    mActionBinding[MoveUp].action    = derivedAction<SceneNode>(DebugAction("MoveUp Triggered"));
    mActionBinding[MoveDown].action  = derivedAction<SceneNode>(DebugAction("MoveDown Triggered"));
}

bool InputHandler::isRealTimeAction(Action action)
{
    switch (action)
    {
        case MoveLeft:
        case MoveRight:
        case MoveDown:
        case MoveUp:
            return true;

        default:
            return false;
    }
}