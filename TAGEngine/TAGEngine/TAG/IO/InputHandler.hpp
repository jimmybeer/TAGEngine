#pragma once

#include "Command.hpp"

#include <SFML/Window/Event.hpp>

#include <map>
#include <string>

class CommandQueue;

class InputHandler
{
public:
    enum Action
	{
			MoveLeft,
			MoveRight,
			MoveUp,
			MoveDown,
			ActionCount
	};
	
	InputHandler();
	void handleEvent(const sf::Event& event, CommandQueue& commands);
	void handleRealTimeInput(CommandQueue& commands);
	
	void assignKey(Action action, sf::Keyboard::Key key);
    sf::Keyboard::Key getAssignedKey(Action action) const;
	
protected:
    void initialiseActions();
	
private:
    static bool isRealTimeAction(Action action);
	
    std::map<sf::Keyboard::Key, Action> mKeyBinding;
	std::map<Action, Command> mActionBinding;
};