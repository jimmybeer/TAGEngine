#pragma once

namespace sf
{
    class Texture;
	class Font;
}

namespace Textures
{
    enum ID
	{
	};
}

namespace Fonts
{
    enum ID
	{
	};
}

// Forward declaration and a few type definitions
template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures::ID> textureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;