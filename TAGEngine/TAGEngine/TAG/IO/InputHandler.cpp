#include "InputHandler.hpp"
#include "CommandQueue.hpp"
#include "SceneNode.hpp"

#include <map>
#include <string>
#include <algorithm>

using namespace std::placeholders;

InputHandler::InputHandler()
{}

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

void InputHandler::assignKey(int action, sf::Keyboard::Key key)
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

sf::Keyboard::Key InputHandler::getAssignedKey(int action) const
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
{}

bool InputHandler::isRealTimeAction(int action)
{
    return false;
}