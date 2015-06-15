#pragma once

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "InputHandler.hpp"
#include "StateStack.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Application
{
public:
    Application(int w, int h, const std::string& title = "TAG Application");
	void run();

protected:
	void registerStates();
	
	void focusGained();
	void focusLost();
	
private:
    void processInput();
	void update(sf::Time elapsedTime);
	void render();
	
	void updateStatistics(sf::Time elapseTime);
	
	static const sf::Time TimePerFrame;
	
	sf::RenderWindow mWindow;
	TextureHolder mTextures;
	FontHolder mFonts;
	InputHandler mInputHandler;
	
	StateStack mStateStack;
	
	sf::Text mStatisticsText;
	sf::Time mStatisticsUpdateTime;
	std::size_t mStatisticsNumFrames;
	bool showStats;
	
	bool mIsPaused;
};