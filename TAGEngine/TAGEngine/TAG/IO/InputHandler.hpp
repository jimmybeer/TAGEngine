#pragma once

#include "Command.hpp"
#include "SceneNode.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>

#include <map>
#include <string>
#include <functional>

class CommandQueue;

class InputHandler
{
public:
    using FuncType = std::function<void(SceneNode&, sf::Time)>;

    InputHandler();
    void handleEvent(const sf::Event& event, CommandQueue& commands);
    void handleRealTimeInput(CommandQueue& commands);
    
    void assignKey(unsigned int action, sf::Keyboard::Key key);
    void assignAction(unsigned int action, const FuncType& callback, unsigned int category, bool realtimeAction = false);
    sf::Keyboard::Key getAssignedKey(unsigned int action) const;
    
protected:
    virtual void initialiseActions();
    virtual bool isRealTimeAction(unsigned int action);
    
    std::map<sf::Keyboard::Key, unsigned int> mKeyBinding;
    std::map<unsigned int, Command> mActionBinding;
    std::map<unsigned int, bool> mRealtimeAction;
};