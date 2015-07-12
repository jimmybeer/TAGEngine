#pragma once

#include "Component.hpp"
#include "Defaults.hpp"

#include <SFML/Graphics/Text.hpp>

#include <string>
#include <memory>

namespace GUI
{
    
    class Label : public Component
    {
    public:
        typedef std::shared_ptr<Label> Ptr;
        
        Label(const std::string& text);
        virtual bool isSelectable() const;
        void setText(const std::string& text);
        
        virtual void handleEvent(const sf::Event& event);
        
    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;
        
        sf::Text mText;
    };
    
}