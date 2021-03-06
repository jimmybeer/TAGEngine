#include "Random.hpp"

#include <SFML/Config.hpp>

#include <random>
#include <ctime>
#include <cassert>

namespace tag
{
namespace
{
#ifdef TAG_USE_STD_RANDOM_ENGINE
    
    // Use Mersenne Twister as default standard random engine
    typedef std::mt19937 Engine;
    
#else
    
    // Random generator engine (Multiply With Carry)
    // Many thanks to Volkard Henkel for the implementation.
    class Engine
    {
    public:
        // Type definition for usage inside Std.Random
        typedef sf::Uint32 result_type;
        
        
    public:
        // Constructor
        explicit Engine(sf::Uint32 seedVal)
        {
            seed(seedVal);
        }
        
        // Return random number
        sf::Uint32 operator() ()
        {
            const sf::Uint64 a = 1967773755;
            
            x = a * (x & 0xffffffff) + (x >> 32);
            return static_cast<sf::Uint32>(x);
        }
        
        // set seed (compliant to Std.Random)
        void seed(sf::Uint32 seedVal = 0)
        {
            x = seedVal + !seedVal;
        }
        
        // Return minimal value (compliant to Std.Random)
        static constexpr sf::Uint32 min()
        {
            return 0;
        }
        
        // Return maximal value (compliant to Std.Random)
        static constexpr sf::Uint32 max()
        {
            return 0xffffffff;
        }
        
    private:
        sf::Uint64 x;
    };
    
#endif 
    
    // Function initializing the engine and its seed at startup time
    Engine createInitialEngine()
    {
        return Engine( static_cast<unsigned long>(std::time(nullptr)) );
    }
    
    // Pseudo random number generator engine
    Engine globalEngine = createInitialEngine();
    
} // namespace
    
    // ---------------------------------------------------------------------------------------------------------------------------
    
    
    int random(int min, int max)
    {
        assert(min <= max);
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(globalEngine);
    }
    
    unsigned int random(unsigned int min, unsigned int max)
    {
        assert(min <= max);
        std::uniform_int_distribution<unsigned int> distribution(min, max);
        return distribution(globalEngine);
    }
    
    float random(float min, float max)
    {
        assert(min <= max);
        std::uniform_real_distribution<float> distribution(min, max);
        return distribution(globalEngine);
    }
    
    float randomDev(float middle, float deviation)
    {
        assert(deviation >= 0.f);
        return random(middle-deviation, middle+deviation);
    }
    
    void setRandomSeed(unsigned long seed)
    {
        globalEngine.seed(seed);
    }

}