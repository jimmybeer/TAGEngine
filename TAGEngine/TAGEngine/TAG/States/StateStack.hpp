#pragma once

#include "State.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/System/NonCopyable.hpp>

#include <vector>
#include <map>
#include <functional>
#include <utility>

namespace sf
{
    class Event;
	class RenderWindow;
}

class StateStack : private sf::NonCopyable
{
public:
    enum Action
	{
	    Push,
		Pop,
		Clear
	};
	
	explicit StateStack(State::Context context);
    
	template <typename T>
	void registerState(unsigned int stateID);
	
	void update(sf::Time dt);
	void draw();
	void handleEvent(const sf::Event& event);
	
	void pushState(unsigned int stateID);
	void popState();
	void clearStates();
	
	bool isEmpty() const;

private:
    State::Ptr createState(unsigned int stateID);
	void applyPendingChanges();
	
	struct PendingChange
	{
	    explicit PendingChange(Action action, unsigned int stateID = 0);
	    Action action;
        unsigned int stateID;
	};
	
	std::vector<State::Ptr> mStack;
	std::vector<PendingChange> mPendingList;
	State::Context mContext;
	std::map<unsigned int, std::function<State::Ptr()>> mFactories;
};

template <typename T>
void StateStack::registerState(unsigned int stateID)
{
    mFactories[stateID] = [this] ()
	{
	    return State::Ptr(new T(*this, mContext));
	};
}