#include "GUISettings.hpp"

GUISettings::GUISettings()
 : textures(nullptr)
 , fonts(nullptr)
 , sounds(nullptr)
{}

GUISettings& getGUISettings()
{
    static GUISettings settings;
    return settings;
}