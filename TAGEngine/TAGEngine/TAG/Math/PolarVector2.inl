#include "PolarVector2.hpp"

namespace tag
{

    template <typename T>
    PolarVector2<T>::PolarVector2()
    : r()
    , phi()
    {
    }
    
    template <typename T>
    PolarVector2<T>::PolarVector2(T radius, T angle)
    : r(radius)
    , phi(angle)
    {
    }
    
    template <typename T>
    PolarVector2<T>::PolarVector2(const sf::Vector2<T>& vector)
    : r(length(vector))
    , phi(vector == sf::Vector2<T>() ? 0.f : polarAngle(vector))
    {
    }
    
    template <typename T>
    PolarVector2<T>::operator sf::Vector2<T>() const
    {
        return sf::Vector2<T>(
                              r * TrigonometricTraits<T>::cos(phi),
                              r * TrigonometricTraits<T>::sin(phi));
    }
    
    template <typename T>
    T length(const PolarVector2<T>& vector)
    {
        return vector.r;
    }
    
    template <typename T>
    T polarAngle(const PolarVector2<T>& vector)
    {
        return vector.phi;
    }

}