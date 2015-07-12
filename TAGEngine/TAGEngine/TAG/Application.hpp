#pragma once

#include "ResourceHolder.hpp"
#include "InputHandler.hpp"
#include "StateStack.hpp"
#include "MusicPlayer.hpp"
#include "SoundPlayer.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Application
{
public:
    Application(int w, int h, const std::string& title = "TAG Application");
    void run();
    
protected:    
    virtual void focusGained();
    virtual void focusLost();
    
    virtual void configFonts();
    virtual void configGUI();
    
    StateStack mStateStack;
    InputHandler mInputHandler;
    
    MusicPlayer mMusic;
    SoundPlayer mSounds;
    
    TextureHolder mTextures;
    FontHolder mFonts;
private:
    void processInput();
    void update(sf::Time elapsedTime);
    void render();
    
    void updateStatistics(sf::Time elapseTime);
    
    static const sf::Time TimePerFrame;
    
    sf::RenderWindow mWindow;
    
    sf::Text mStatisticsText;
    sf::Time mStatisticsUpdateTime;
    std::size_t mStatisticsNumFrames;
    bool showStats;
    
    bool mIsPaused;
};