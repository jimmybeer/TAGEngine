#include "InputHandler.hpp"
#include "CommandQueue.hpp"
#include "SceneNode.hpp"
#include "Aircraft.hpp"

#include <map>
#include <string>
#include <algorithm>

using namespace std::placeholders;

struct AircraftMover
{
    AircraftMover(float vx, float vy)
	 : velocity(vx, vy)
	{}
	
	void operator() (Aircraft& aircraft, sf::Time) const
	{
	    aircraft.accelerate(velocity * aircraft.getMaxSpeed());
	}
	
	sf::Vector2f velocity;
};

InputHandler::InputHandler()
 : mCurrentMissionStatus(MissionRunning)
{
    // set initial key bindings
	mKeyBinding[sf::Keyboard::Left] = MoveLeft;
	mKeyBinding[sf::Keyboard::Right] = MoveRight;
	mKeyBinding[sf::Keyboard::Up] = MoveUp;
	mKeyBinding[sf::Keyboard::Down] = MoveDown;
	mKeyBinding[sf::Keyboard::Space] = Fire;
	mKeyBinding[sf::Keyboard::M] = LaunchMissile;
	
    initialiseActions();
	
    for(auto& pair : mActionBinding)
	{
        pair.second.category = Category::PlayerAircraft;
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

void InputHandler::setMissionStatus(MissionStatus status)
{
    mCurrentMissionStatus = status;
}

InputHandler::MissionStatus InputHandler::getMissionStatus() const
{
    return mCurrentMissionStatus;
}

void InputHandler::initialiseActions()
{
    mActionBinding[MoveLeft].action	      = derivedAction<Aircraft>(AircraftMover(-1, 0));
    mActionBinding[MoveRight].action      = derivedAction<Aircraft>(AircraftMover(+1, 0));
    mActionBinding[MoveUp].action         = derivedAction<Aircraft>(AircraftMover(0, -1));
    mActionBinding[MoveDown].action         = derivedAction<Aircraft>(AircraftMover(0, +1));
    mActionBinding[Fire].action           = derivedAction<Aircraft>([] (Aircraft& a, sf::Time) {a.fire();});
    mActionBinding[LaunchMissile].action  = derivedAction<Aircraft>([] (Aircraft& a, sf::Time) {a.launchMissile();});
}

bool InputHandler::isRealTimeAction(Action action)
{
    switch (action)
    {
        case MoveLeft:
        case MoveRight:
        case MoveDown:
        case MoveUp:
		case Fire:
            return true;

        default:
            return false;
    }
}