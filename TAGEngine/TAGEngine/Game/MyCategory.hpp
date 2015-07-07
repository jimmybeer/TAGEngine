#pragma once

#include "Category.hpp"

// Entity/scene node category, used to dispatch commands
namespace Category
{

    const static unsigned int SceneAirLayer = 1 << CatShift;
    const static unsigned int PlayerAircraft		= 1 << (CatShift+1);
    const static unsigned int AlliedAircraft		= 1 << (CatShift+2);
    const static unsigned int EnemyAircraft		= 1 << (CatShift+3);
    const static unsigned int Pickup				= 1 << (CatShift+4);
    const static unsigned int AlliedProjectile	= 1 << (CatShift+5);
    const static unsigned int EnemyProjectile		= 1 << (CatShift+6);
		
    const static unsigned int Aircraft = PlayerAircraft | AlliedAircraft | EnemyAircraft;
    const static unsigned int Projectile = AlliedProjectile | EnemyProjectile;
}