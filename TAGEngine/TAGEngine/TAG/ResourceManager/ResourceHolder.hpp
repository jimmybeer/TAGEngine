#pragma once

#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>

namespace sf
{
    class Texture;
    class Font;
    class Shader;
    class SoundBuffer;
}

template <typename Resource, typename Identifier>
class ResourceHolder
{
public:
    void						load(Identifier id, const std::string& filename);
    
    template <typename Parameter>
    void						load(Identifier id, const std::string& filename, const Parameter& secondParam);
    
    Resource&					get(Identifier id);
    const Resource&				get(Identifier id) const;
    
    
private:
    void						insertResource(Identifier id, std::unique_ptr<Resource> resource);
    
    
private:
    std::map<Identifier, std::unique_ptr<Resource>>	mResourceMap;
};

#include "ResourceHolder.inl"

// Forward declaration and a few type definitions
template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, unsigned int> TextureHolder;
typedef ResourceHolder<sf::Font, unsigned int> FontHolder;
typedef ResourceHolder<sf::Shader, unsigned int> ShaderHolder;
typedef ResourceHolder<sf::SoundBuffer, unsigned int> SoundBufferHolder;