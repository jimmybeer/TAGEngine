#pragma once

#include "InputHandler.hpp"
#include "Command.hpp"

#include <SFML/Window/Event.hpp>

#include <map>
#include <string>

class MyInputHandler : public InputHandler
{
public:
    enum Action
	{
			MoveLeft,
			MoveRight,
			MoveUp,
			MoveDown,
			Fire,
			LaunchMissile,
			ActionCount
	};
	
	enum MissionStatus
	{
	    MissionRunning,
		MissionSuccess,
		MissionFailure
	};
	
	MyInputHandler();
	
	void setMissionStatus(MissionStatus status);
	MissionStatus getMissionStatus()const;
	
protected:
    void initialiseActions();
    bool isRealTimeAction(unsigned int action);
	
	MissionStatus mCurrentMissionStatus;
};