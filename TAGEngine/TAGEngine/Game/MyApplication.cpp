#include "MyApplication.hpp"
#include "ResourceIdentifiers.hpp"

#include "State.hpp"
#include "StateIdentifiers.hpp"
#include "TitleState.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"
#include "PauseState.hpp"
#include "GameOverState.hpp"
#include "SettingsStates.hpp"

#include "Actions.hpp"
#include "MyCategory.hpp"
#include "Aircraft.hpp"
#include "InputHandler.hpp"

#include "ResourcePath.hpp"

#include <SFML/Window/Event.hpp>

struct AircraftMover
{
    AircraftMover(float vx, float vy)
	 : velocity(vx, vy)
	{}
	
	void operator() (Aircraft& aircraft, sf::Time) const
	{
	    aircraft.accelerate(velocity * aircraft.getMaxSpeed());
	}
	
	sf::Vector2f velocity;
};


MyApplication::MyApplication(int w, int h)
 : Application(w, h, "Generic Engine Test")
{	
    mTextures.load(Textures::TitleScreen, resourcePath() + "TitleScreen.png");
    mFonts.load(Fonts::Main, resourcePath() + "sansation.ttf");
    
    mInputHandler.assignKey(MoveLeft, sf::Keyboard::Left);
    mInputHandler.assignKey(MoveRight, sf::Keyboard::Right);
    mInputHandler.assignKey(MoveUp, sf::Keyboard::Up);
    mInputHandler.assignKey(MoveDown, sf::Keyboard::Down);
    mInputHandler.assignKey(Fire, sf::Keyboard::Space);
    mInputHandler.assignKey(LaunchMissile, sf::Keyboard::M);
    
    mInputHandler.assignAction(MoveLeft, derivedAction<Aircraft>(AircraftMover(-1, 0)), Category::PlayerAircraft, true);
    mInputHandler.assignAction(MoveRight, derivedAction<Aircraft>(AircraftMover(+1, 0)), Category::PlayerAircraft, true);
    mInputHandler.assignAction(MoveUp, derivedAction<Aircraft>(AircraftMover(0, -1)), Category::PlayerAircraft, true);
    mInputHandler.assignAction(MoveDown, derivedAction<Aircraft>(AircraftMover(0, +1)), Category::PlayerAircraft, true);
    mInputHandler.assignAction(Fire, derivedAction<Aircraft>([] (Aircraft& a, sf::Time) {a.fire();}), Category::PlayerAircraft);
    mInputHandler.assignAction(LaunchMissile, derivedAction<Aircraft>([] (Aircraft& a, sf::Time) {a.launchMissile();}), Category::PlayerAircraft);

    mStateStack.registerState<TitleState>(States::Title);
    mStateStack.registerState<MenuState>(States::Menu);
    mStateStack.registerState<GameState>(States::Game);
    mStateStack.registerState<PauseState>(States::Pause);
    mStateStack.registerState<SettingsState>(States::Settings);
    mStateStack.registerState<GameOverState>(States::GameOver);
    
    mSounds.addSound(SoundEffect::AlliedGunfire, resourcePath() + "AlliedGunFire.wav");
    mSounds.addSound(SoundEffect::EnemyGunfire, resourcePath() + "EnemyGunFire.wav");
    mSounds.addSound(SoundEffect::Explosion1, resourcePath() + "Explosion1.wav");
    mSounds.addSound(SoundEffect::Explosion2, resourcePath() + "Explosion2.wav");
    mSounds.addSound(SoundEffect::LaunchMissile, resourcePath() + "LaunchMissile.wav");
    mSounds.addSound(SoundEffect::CollectPickup, resourcePath() + "CollectPickup.wav");
    
    mMusic.addTheme(Music::MenuTheme, resourcePath() + "MenuTheme.ogg");
	mMusic.addTheme(Music::MissionTheme, resourcePath() + "MissionTheme.ogg");
    
	mStateStack.pushState(States::Title);
	
	mMusic.setVolume(0.5);
}