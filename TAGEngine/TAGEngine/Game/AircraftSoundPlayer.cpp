#include "AircraftSoundPlayer.hpp"
#include "ResourceIdentifiers.hpp"
#include "ResourcePath.hpp"

AircraftSoundPlayer::AircraftSoundPlayer()
 : SoundPlayer()
{
    addSound(SoundEffect::AlliedGunfire, resourcePath() + SFX + "AlliedGunFire.wav");
    addSound(SoundEffect::EnemyGunfire, resourcePath() + SFX + "EnemyGunFire.wav");
    addSound(SoundEffect::Explosion1, resourcePath() + SFX + "Explosion1.wav");
    addSound(SoundEffect::Explosion2, resourcePath() + SFX + "Explosion2.wav");
    addSound(SoundEffect::LaunchMissile, resourcePath() + SFX + "LaunchMissile.wav");
    addSound(SoundEffect::CollectPickup, resourcePath() + SFX + "CollectPickup.wav");
    addSound(SoundEffect::Button, resourcePath() + SFX + "Button.wav");
}