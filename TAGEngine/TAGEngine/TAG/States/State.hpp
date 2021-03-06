#pragma once

#include "ResourceHolder.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include <memory>

namespace sf
{
	class RenderWindow;
}

class StateStack;
class InputHandler;
class MusicPlayer;
class SoundPlayer;

class State
{
public:
    typedef std::unique_ptr<State> Ptr;
	struct Context
	{
        Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts, InputHandler& inputHandler,
		        MusicPlayer& music, SoundPlayer& sounds);
				
		sf::RenderWindow* window;
		TextureHolder* textures;
		FontHolder* fonts;
		InputHandler* inputHandler;
		MusicPlayer* music;
		SoundPlayer* sounds;
	};
	
	State(StateStack& stack, Context context);
	virtual ~State();
	
	virtual void draw() = 0;
	virtual bool update(sf::Time dt) = 0;
	virtual bool handleEvent(const sf::Event& event) = 0;
	
protected:
    void requestStackPush(unsigned int stateID);
    void requestStackPop();
    void requestStackClear();

    Context getContext() const;	
	
private:
    StateStack* mStack;
	Context mContext;
};