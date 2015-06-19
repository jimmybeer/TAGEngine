#pragma once

namespace sf
{
    class Texture;
	class Font;
    class Shader;
}

namespace Textures
{
    enum ID
	{
	    Entities,
		Jungle,
		TitleScreen,
		Buttons,
		Explosions,
		Particle,
		FinishLine,
	};
}

namespace Fonts
{
    enum ID
	{
        Main,
	};
}

namespace Shaders
{
    enum ID
	{
	    BrightnessPass,
		DownSamplePass,
		GaussianBlurPass,
		AddPass
	};
}

// Forward declaration and a few type definitions
template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;
typedef ResourceHolder<sf::Shader, Shaders::ID> ShaderHolder;