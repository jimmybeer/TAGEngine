#include "MenuState.hpp"
#include "Button.hpp"
#include "Utility.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

MenuState::MenuState(StateStack& stack, Context context)
 : State(stack, context)
 , mGuiContainer()
{
    sf::Texture& texture = context.textures->get(Textures::TitleScreen);
	sf::Font& font = context.fonts->get(Fonts::Main);
	
	mBackgroundSprite.setTexture(texture);
	
	auto playButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	playButton->setPosition(100, 250);
	playButton->setText("Play");
	playButton->setCallback([this] ()
	{
	    requestStackPop();
		requestStackPush(States::Game);
	});
	
	auto settingsButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	settings->setPosition(100, 300);
	settings->setText("Settings");
	settings->setCallback([this] ()
	{
	    requestStackPush(States::Settings);
	});
	
	auto exitButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	exitButton->setPosition(100, 350);
	exitButton->setText("Exit");
	exitButton->setCallback([this] ()
    {
	    requestStackPop();
	});
	
	mGuiContainer.pack(playButton);
	mGuiContainer.pack(settingsButton);
	mGuiContainer.pack(exitButton);
}

void MenuState::draw()
{
    sf::RenderWindow& window = *getContext().window;
	
	window.setView(window.getDefaultView()); 
	
	window.draw(mBackgroundSprite);
	window.draw(mGuiContainer);
}

bool MenuState::update(sf::Time dt)
{
    return true;
}

bool MenuState::handleEvent(const sf::Event& event)
{
    mGuiContainer.handleEvent(event);
	return false;
}