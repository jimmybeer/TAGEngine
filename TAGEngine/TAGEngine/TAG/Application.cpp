#include "Application.hpp"
#include "State.hpp"
#include "Utility.hpp"
#include "Defaults.hpp"
#include "ResourcePath.hpp"

#include "GUISettings.hpp"

const sf::Time Application::TimePerFrame = sf::seconds(1.f/60.f);

Application::Application(int w, int h, const std::string& title)
: mWindow(sf::VideoMode(w, h), title, sf::Style::Close)
, mTextures()
, mFonts()
, mInputHandler()
, mMusic()
, mSounds()
, mStateStack(State::Context(mWindow, mTextures, mFonts, mInputHandler, mMusic, mSounds))
, mStatisticsText()
, mStatisticsUpdateTime()
, mStatisticsNumFrames(0)
, showStats(true)
, mIsPaused(false)
{
    mWindow.setKeyRepeatEnabled(false);
    mWindow.setVerticalSyncEnabled(true);
    
    configFonts();
    configGUI();
    
    mStatisticsText.setFont(mFonts.get(TagDefaults::Fonts::Main));
    mStatisticsText.setPosition(5.f, 5.f);
    mStatisticsText.setCharacterSize(10);
}

void Application::configFonts()
{
    mFonts.load(TagDefaults::Fonts::Main, resourcePath() + "sansation.ttf");
}

void Application::configGUI()
{
    GUISettings &settings = getGUISettings();
    
    settings.textures = &mTextures;
    settings.fonts = &mFonts;
    settings.sounds = &mSounds;
    
    settings.buttonTexture = TagDefaults::Textures::Buttons;
    settings.buttonFont = TagDefaults::Fonts::Main;
    settings.buttonPressedSound = TagDefaults::SoundEffect::Button;
    
    settings.labelFont = TagDefaults::Fonts::Main;
    
    settings.textures->load(settings.buttonTexture, resourcePath() + "Buttons.png");
    settings.sounds->addSound(settings.buttonPressedSound, resourcePath() + "Button.wav");
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

void Application::focusGained()
{
    mIsPaused = false;
}

void Application::focusLost()
{
    mIsPaused = true;
}