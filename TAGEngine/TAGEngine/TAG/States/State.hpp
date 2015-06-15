#pragma once

#include "StateIdentifiers.hpp"
#include "ResourceIdentifiers.hpp"
#include "InputHandler.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include <memory>

namespace sf
{
	class RenderWindow;
}

class StateStack;

class State
{
public:
    typedef std::unique_ptr<State> Ptr;
	struct Context
	{
	    Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts, InputHandler& inputHandle);
		sf::RenderWindow window;
		TextureHolder* textures;
		FontHolder* fonts;
		InputHandler* InputHandler;
	};
	
	State(StateStack& stack, Context context);
	virtual ~State();
	
	virtual void draw() = 0;
	virtual bool update(sf::Time dt) = 0;
	virtual bool handleEvent(const sf::Event& event) = 0;
	
protected:
    void requestStackPush(States::ID stateID);
    void requestStackPop();
    void requestStackClear();

    Context getConstext() const;	
	
private:
    StateStack* mStack;
	Context mContext;
};