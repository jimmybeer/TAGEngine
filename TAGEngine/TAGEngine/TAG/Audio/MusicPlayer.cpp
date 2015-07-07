#include "MusicPlayer.hpp"

MusicPlayer::MusicPlayer()
 : mMusic()
 , mFilenames()
 , mVolume(100.f)
{}

void MusicPlayer::addTheme(int id, const std::string& filename)
{
    mFilenames[id] = filename;
}

void MusicPlayer::play(int theme)
{
    std::string filename = mFilenames[theme];
	
	if(!mMusic.openFromFile(filename))
	{
	    throw std::runtime_error("Music " + filename + " could not be loaded.");
	}
	
	mMusic.setVolume(mVolume);
	mMusic.setLoop(true);
    mMusic.play();
}

void MusicPlayer::stop()
{
    mMusic.stop();
}

void MusicPlayer::setVolume(float volume)
{
    mVolume = volume;
}

void MusicPlayer::setPaused(bool paused)
{
    if(paused)
	{
	    mMusic.pause();
	}
	else
	{
	    mMusic.play();
	}
}

