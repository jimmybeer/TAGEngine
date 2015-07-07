#pragma once

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Audio/Music.hpp>

#include <map>
#include <string>

class MusicPlayer : private sf::NonCopyable
{
public:
    MusicPlayer();
    
    void addTheme(unsigned int id, const std::string& filename);
	
	void play(unsigned int theme);
	void stop();
    
	void setPaused(bool paused);
	void setVolume(float volume);
private:
    sf::Music mMusic;
	std::map<unsigned int, std::string> mFilenames;
	float mVolume;
};