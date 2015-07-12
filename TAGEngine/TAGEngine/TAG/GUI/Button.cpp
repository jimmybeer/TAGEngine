#include "Button.hpp"
#include "Utility.hpp"
#include "GUISettings.hpp"
#include "Defaults.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace GUI
{
    
    Button::Button()
    : mCallback()
    , mSprite(getGUISettings().textures->get(getGUISettings().buttonTexture))
    , mText("", getGUISettings().fonts->get(getGUISettings().buttonFont), 16)
    , mIsToggle(false)
    {
        mButtonWidth = mSprite.getTexture()->getSize().x;
        mButtonHeight = (mSprite.getTexture()->getSize().y/3);
        
        changeTexture(Normal);
        
        sf::FloatRect bounds = mSprite.getLocalBounds();
        mText.setPosition(bounds.width / 2.f, bounds.height / 2.f);
    }
    
    Button::Button(const TextureHolder& textures)
    : mCallback()
    , mSprite(textures.get(TagDefaults::Textures::Buttons))
    , mText("", getGUISettings().fonts->get(getGUISettings().buttonFont), 16)
    , mIsToggle(false)
    {
        mButtonWidth = mSprite.getTexture()->getSize().x;
        mButtonHeight = (mSprite.getTexture()->getSize().y/3);
        
        changeTexture(Normal);
        
        sf::FloatRect bounds = mSprite.getLocalBounds();
        mText.setPosition(bounds.width / 2.f, bounds.height / 2.f);
    }

    
    void Button::setCallback(Callback callback)
    {
        mCallback = std::move(callback);
    }
    
    void Button::setText(const std::string& text)
    {
        mText.setString(text);
        centerOrigin(mText);
    }
    
    void Button::setToggle(bool flag)
    {
        mIsToggle = flag;
    }
    
    bool Button::isSelectable() const
    {
        return true;
    }
    
    void Button::select()
    {
        Component::select();
        
        changeTexture(Selected);
    }
    
    void Button::deselect()
    {
        Component::deselect();
        
        changeTexture(Normal);
    }
    
    void Button::activate()
    {
        Component::activate();
        
        // If we are a toggle then we should show that the button is pressed and thus "toggle"
        if(mIsToggle)
        {
            changeTexture(Pressed);
        }
        
        if(mCallback)
        {
            mCallback();
        }
        
        // If we are not a toggle then deactivate the button since we are just momentarily activated.
        if(!mIsToggle)
        {
            deactivate();
        }
        
        getGUISettings().sounds->play(getGUISettings().buttonPressedSound);
    }
    
    void Button::deactivate()
    {
        Component::deactivate();
        
        if(mIsToggle)
        {
            // Reset texture to right one depending on if we are selected or not.
            if(isSelected())
            {
                changeTexture(Selected);
            }
            else
            {
                changeTexture(Normal);
            }
        }
    }
    
    void Button::handleEvent(const sf::Event&)
    {}
    
    void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform *= getTransform();
        target.draw(mSprite, states);
        target.draw(mText, states);
    }
    
    void Button::changeTexture(Type buttonType)
    {
        sf::IntRect textureRect(0, mButtonHeight*buttonType, mButtonWidth, mButtonHeight);
        mSprite.setTextureRect(textureRect);
    }
    
}