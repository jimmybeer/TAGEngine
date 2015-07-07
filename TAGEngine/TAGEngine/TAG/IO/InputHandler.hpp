#pragma once

#include "Command.hpp"

#include <SFML/Window/Event.hpp>

#include <map>
#include <string>

class CommandQueue;

class InputHandler
{
public:
    InputHandler();
    void handleEvent(const sf::Event& event, CommandQueue& commands);
    void handleRealTimeInput(CommandQueue& commands);
    
    void assignKey(int action, sf::Keyboard::Key key);
    sf::Keyboard::Key getAssignedKey(int action) const;
    
protected:
    virtual void initialiseActions();
    virtual bool isRealTimeAction(int action);
    
    std::map<sf::Keyboard::Key, int> mKeyBinding;
    std::map<int, Command> mActionBinding;
};