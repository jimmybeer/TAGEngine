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
    
    void assignKey(unsigned int action, sf::Keyboard::Key key);
    sf::Keyboard::Key getAssignedKey(unsigned int action) const;
    
protected:
    virtual void initialiseActions();
    virtual bool isRealTimeAction(unsigned int action);
    
    std::map<sf::Keyboard::Key, unsigned int> mKeyBinding;
    std::map<unsigned int, Command> mActionBinding;
};