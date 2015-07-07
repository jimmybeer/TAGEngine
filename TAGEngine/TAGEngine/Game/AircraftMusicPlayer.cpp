#include "AircraftMusicPlayer.hpp"
#include "ResourceIdentifiers.hpp"
#include "ResourcePath.hpp"

AircraftMusicPlayer::AircraftMusicPlayer() 
 : MusicPlayer()
{
    addTheme(Music::MenuTheme, resourcePath() + MUSIC + "MenuTheme.ogg");
	addTheme(Music::MissionTheme, resourcePath() + MUSIC + "MissionTheme.ogg");
}