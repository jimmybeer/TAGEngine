#include "Application.hpp"
#include "State.hpp"
#include "StateIdentifiers.hpp"
#include "Utility.hpp"
#include "TitleState.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"
#include "SettingsStates.hpp"
#include "ResourcePath.hpp"

#include <iostream>

const sf::Time Application::TimePerFrame = sf::seconds(1.f/60.f);

Application::Application(int w, int h, const std::string& title)
 : mWindow(sf::VideoMode(w, h), title, sf::Style::Close)
 , mTextures()
 , mFonts()
 , mInputHandler()
 , mStateStack(State::Context(mWindow, mTextures, mFonts, mInputHandler))
 , mStatisticsText()
 , mStatisticsUpdateTime()
 , mStatisticsNumFrames(0)
 , showStats(true)
 , mIsPaused(false)
{
    mWindow.setKeyRepeatEnabled(false);
    
    mFonts.load(Fonts::Main, resourcePath() + "sansation.ttf");
    mTextures.load(Textures::TitleScreen, resourcePath() + "TitleScreen.png");
    mTextures.load(Textures::ButtonNormal, resourcePath() + "ButtonNormal.png");
    mTextures.load(Textures::ButtonSelected, resourcePath() + "ButtonSelected.png");
    mTextures.load(Textures::ButtonPressed, resourcePath() + "ButtonPressed.png");
    
    mStatisticsText.setFont(mFonts.get(Fonts::Main));
    mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(10);
	
	registerStates();
	mStateStack.pushState(States::Title);
}

void Application::run()
{
    sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while(mWindow.isOpen())
	{
	    sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while(timeSinceLastUpdate > TimePerFrame)
		{
		    timeSinceLastUpdate -= TimePerFrame;
			
			processInput();
			if(!mIsPaused)
			{
			    update(TimePerFrame);
			}
			
			// Check inside this loop, because stack might be empty before update() call
			if(mStateStack.isEmpty())
			{
			    mWindow.close();
			}
		}
		
		updateStatistics(elapsedTime);
		render();
	}
}

void Application::processInput()
{
    sf::Event event;
	while(mWindow.pollEvent(event))
	{
        std::cout << "Event: " << event.type << std::endl;
        
	    mStateStack.handleEvent(event);
		
	    switch(event.type)
		{			
			case sf::Event::Closed :
			    mWindow.close();
		        break;
				
			case sf::Event::GainedFocus :
			    focusGained();
				break;
			
			case sf::Event::LostFocus :
			    focusLost();
				break;
		}
	}
}

void Application::update(sf::Time dt)
{
    mStateStack.update(dt);
}

void Application::render()
{
    mWindow.clear();
	
	mStateStack.draw();
	
	if(showStats)
	{
	    mWindow.setView(mWindow.getDefaultView());
		mWindow.draw(mStatisticsText);
	}
	
	mWindow.display();
}

void Application::updateStatistics(sf::Time elapsedTime)
{
    mStatisticsUpdateTime += elapsedTime;
	mStatisticsNumFrames += 1;
	
	if(mStatisticsUpdateTime >= sf::seconds(1.0f))
	{
	    mStatisticsText.setString("FPS: " + toString(mStatisticsNumFrames));
		
		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}
}

void Application::registerStates()
{
    mStateStack.registerState<TitleState>(States::Title);
    mStateStack.registerState<MenuState>(States::Menu);
    mStateStack.registerState<GameState>(States::Game);
    //mStateStack.registerState<PauseState>(States::Pause);
    mStateStack.registerState<SettingsState>(States::Settings);
}

void Application::focusGained()
{
    mIsPaused = false;
}

void Application::focusLost()
{
	mIsPaused = true;
}