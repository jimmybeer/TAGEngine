#include "SettingsStates.hpp"
#include "Utility.hpp"
#include "ResourceIdentifiers.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

SettingsState::SettingsState(StateStack& stack, Context context)
: State(stack, context)
, mGUIContainer()
{
    mBackgroundSprite.setTexture(context.textures->get(Textures::TitleScreen));

    // Build key binding buttons and labels
    addButtonLabel(MyInputHandler::MoveLeft,		300.f, "Move Left", context);
    addButtonLabel(MyInputHandler::MoveRight,		350.f, "Move Right", context);
    addButtonLabel(MyInputHandler::MoveUp,			400.f, "Move Up", context);
    addButtonLabel(MyInputHandler::MoveDown,		450.f, "Move Down", context);
    addButtonLabel(MyInputHandler::Fire,			500.f, "Fire", context);
    addButtonLabel(MyInputHandler::LaunchMissile,	550.f, "Missile", context);

    updateLabels();

    auto backButton = std::make_shared<GUI::Button>(context);    
    backButton->setPosition(80.f, 375.f);
    backButton->setText("Back");
    backButton->setCallback(std::bind(&SettingsState::requestStackPop, this));

    mGUIContainer.pack(backButton);
}

void SettingsState::draw()
{
    sf::RenderWindow& window = *getContext().window;

    window.draw(mBackgroundSprite);
    window.draw(mGUIContainer);
}

bool SettingsState::update(sf::Time)
{
    return true;
}

bool SettingsState::handleEvent(const sf::Event& event)
{
    bool isKeyBinding = false;

    // Iterate through all key binding buttons to see if they are being pressed, waiting for the user to enter a key
    for (std::size_t action = 0; action < MyInputHandler::ActionCount; ++action)
    {
        if (mBindingButtons[action]->isActive())
        {
            isKeyBinding = true;
            if (event.type == sf::Event::KeyReleased)
            {
                getContext().inputHandler->assignKey(static_cast<MyInputHandler::Action>(action), event.key.code);
                mBindingButtons[action]->deactivate();
            }
            break;
        }
    }

    // If pressed button changed key bindings, update labels; otherwise consider other buttons in container
    if (isKeyBinding)
	{
    	updateLabels();
	}
    else
	{
	    mGUIContainer.handleEvent(event);
	}

    return false;
}

void SettingsState::updateLabels()
{
    InputHandler& inputHandler = *getContext().inputHandler;

    for (std::size_t i = 0; i < MyInputHandler::ActionCount; ++i)
    {
        sf::Keyboard::Key key = inputHandler.getAssignedKey(static_cast<MyInputHandler::Action>(i));
        mBindingLabels[i]->setText(toString(key));
    }
}

void SettingsState::addButtonLabel(MyInputHandler::Action action, float y, const std::string& text, Context context)
{
    mBindingButtons[action] = std::make_shared<GUI::Button>(context);
    mBindingButtons[action]->setPosition(80.f, y);
    mBindingButtons[action]->setText(text);
    mBindingButtons[action]->setToggle(true);

    mBindingLabels[action] = std::make_shared<GUI::Label>("", *context.fonts);
    mBindingLabels[action]->setPosition(300.f, y + 15.f);

    mGUIContainer.pack(mBindingButtons[action]);
    mGUIContainer.pack(mBindingLabels[action]);
}