#pragma once

#include <SFML/System/Time.hpp>

#include <functional>
#include <cassert>

class SceneNode;

// NOTE: Reserved Command Categories:
//    0 = NONE
//    1 = SceneNode
struct Command
{
    std::function<void(SceneNode&, sf::Time)> action;
	unsigned int category;
};

// Utility function to allow assignment of functions from objects
// derived from SceneNode without the constant need to downcast.
template <typename GameObject, typename Function>
std::function<void(SceneNode&, sf::Time)> derivedAction(Function fn)
{
    return [=] (SceneNode& node, sf::Time dt)
	{
	    // Check if cast is safe
		assert(dynamic_cast<GameObject*>(&node) != nullptr);
		
		// Downcast node and invoke function on it
		fn(static_cast<GameObject&>(node), ft);
	};
}