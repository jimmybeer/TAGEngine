#pragma once

// Entity/scene node category, used to dispatch commands
namespace Category
{
    const static unsigned int None           = 0;
    const static unsigned int ParticleSystem = 1 << 0;
    const static unsigned int SoundEffect    = 1 << 1;
    
    const static unsigned int CatShift = 2;
}