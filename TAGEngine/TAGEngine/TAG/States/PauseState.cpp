#include "PauseState.hpp"
#include "Button.hpp"
#include "Utility.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

PauseState::PauseState(StateStack& stack, Context context)
 : State(stack, context)
 , mBackgroundSprite()
 , mPausedText()
 , mGuiContainer()
{
    sf::Font& font = context.fonts->get(Fonts::Main);
	sf::Vector2f windowSize = context.window->getView().getSize();
	
	mPausedText.setFont(font);
	mPausedText.setString("Paused");
	mPausedText.setCharacterSize(70);
	centerOrigin(mPausedText);
	mPausedText.setPosition(0.5f * windowSize.x, 0.4f * windowSize.y);
	
	auto returnButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	returnButton->setPosition(0.5f * windowSize.x - 100, 0.4f * windowSize.y + 75);
	returnButton->setText("Return");
	returnButton->setCallback([this] ()
	{
	    requestStackPop();
	});
	
	auto menuButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	menuButton->setPosition(0.5f * windowSize.x - 100, 0.4f * windowSize.y + 125);
	menuButton->setText("Back to Menu");
	menuButton->setCallback([this] ()
	{
	    requestStackClear();
		requestStackPush(States::Menu);
	});
	
	mGuiContainer.pack(returnButton);
	mGuiContainer.pack(menuButton);
}

void PauseState::draw()
{
    sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());
	
	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(0,0,0,150));
	backgroundShape.setSize(window.getView().getSize());
	
	window.draw(backgroundShape);
	window.draw(mPausedText);
	window.draw(mGuiContainer);
}

bool PauseState::update(sf::Time)
{
    return false;
}

bool PauseState::handleEvent(const sf::Event& event)
{
    mGuiContainer.handleEvent(event);	
	return false;
}