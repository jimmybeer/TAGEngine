#pragma once

#include "ResourceHolder.hpp"
#include "SoundPlayer.hpp"

class GUISettings
{
public:
    GUISettings();
    
    TextureHolder* textures;
    FontHolder* fonts;
    SoundPlayer* sounds;

    unsigned int buttonFont;
    unsigned int buttonTexture;
    unsigned int buttonPressedSound;
    
    unsigned int labelFont;

};

GUISettings& getGUISettings();