#include "MyInputHandler.hpp"
#include "CommandQueue.hpp"
#include "SceneNode.hpp"
#include "Aircraft.hpp"
#include "MyCategory.hpp"

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

MyInputHandler::MyInputHandler()
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

void MyInputHandler::setMissionStatus(MissionStatus status)
{
    mCurrentMissionStatus = status;
}

MyInputHandler::MissionStatus MyInputHandler::getMissionStatus() const
{
    return mCurrentMissionStatus;
}

void MyInputHandler::initialiseActions()
{
    mActionBinding[MoveLeft].action	      = derivedAction<Aircraft>(AircraftMover(-1, 0));
    mActionBinding[MoveRight].action      = derivedAction<Aircraft>(AircraftMover(+1, 0));
    mActionBinding[MoveUp].action         = derivedAction<Aircraft>(AircraftMover(0, -1));
    mActionBinding[MoveDown].action        = derivedAction<Aircraft>(AircraftMover(0, +1));
    mActionBinding[Fire].action           = derivedAction<Aircraft>([] (Aircraft& a, sf::Time) {a.fire();});
    mActionBinding[LaunchMissile].action  = derivedAction<Aircraft>([] (Aircraft& a, sf::Time) {a.launchMissile();});
}

bool MyInputHandler::isRealTimeAction(int action)
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