#pragma once

#include "SceneNode.hpp"

class SoundPlayer;

class SoundNode : public SceneNode
{
public:
    explicit SoundNode(SoundPlayer& player);
	void playSound(int sound, sf::Vector2f position);
	
	virtual unsigned int getCategory() const;
	
private:
    SoundPlayer& mSounds;
};