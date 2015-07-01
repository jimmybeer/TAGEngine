#pragma once

#include <cmath>

namespace tag
{

// struct TrigonometricTraits
// Trigonometric traits template
// This template can be specialized in order to implement trigonometric constants and functions
// for arbitrary types. Some of the definitions are required by the vector algebra functions. 
// Predefined specializations for float, double and long double.
//
// To define your own traits, specialize the template inside namespace. The full specialization
// shall contain the following public static methods. You don't have to define all of them, depending on
// the functionality you need. For example, computing the vector length requires sqrt(), while the polar
// angle requires arcTan2().
//
// T sin(T deg)
// T cos(T deg)
// T tan(T deg)
// T arcSin(T value)
// T arcCos(T value)
// T arcTan2(T valY, T valX)
// T sqrt(T value)
// T %pi()
// T radToDeg(T rad)
// T degToRad(T deg)
///
/// Attention All trigonometric functions take and return degrees, NOT radians.
template <typename T>
struct TrigonometricTraits
{
};// Trigonometric traits: Specialization for float
    template <>
    struct TrigonometricTraits<float>
    {
        typedef float Type;
        
        static Type sin(Type deg)					{ return std::sin(degToRad(deg));			}
        static Type cos(Type deg)					{ return std::cos(degToRad(deg));			}
        static Type tan(Type deg)					{ return std::tan(degToRad(deg));			}
        static Type arcSin(Type value)				{ return radToDeg(std::asin(value));		}
        static Type arcCos(Type value)				{ return radToDeg(std::acos(value));		}
        static Type arcTan2(Type valY, Type valX)	{ return radToDeg(std::atan2(valY, valX));	}
        static Type sqrt(Type value)				{ return std::sqrt(value);					}
        
        static Type pi()							{ return 3.141592653589793238462643383f;	}
        static Type radToDeg(Type rad)				{ return 180 / pi() * rad;					}
        static Type degToRad(Type deg)				{ return pi() / 180 * deg;					}
    };
    
    // Trigonometric traits: Specialization for double
    template <>
    struct TrigonometricTraits<double>
    {
        typedef double Type;
        
        static Type sin(Type deg)					{ return std::sin(degToRad(deg));			}
        static Type cos(Type deg)					{ return std::cos(degToRad(deg));			}
        static Type tan(Type deg)					{ return std::tan(degToRad(deg));			}
        static Type arcSin(Type value)				{ return radToDeg(std::asin(value));		}
        static Type arcCos(Type value)				{ return radToDeg(std::acos(value));		}
        static Type arcTan2(Type valY, Type valX)	{ return radToDeg(std::atan2(valY, valX));	}
        static Type sqrt(Type value)				{ return std::sqrt(value);					}
        
        static Type pi()							{ return 3.141592653589793238462643383;		}
        static Type radToDeg(Type rad)				{ return 180 / pi() * rad;					}
        static Type degToRad(Type deg)				{ return pi() / 180 * deg;					}
    };
    
    // Trigonometric traits: Specialization for long double
    template <>
    struct TrigonometricTraits<long double>
    {
        typedef long double Type;
        
        static Type sin(Type deg)					{ return std::sin(degToRad(deg));			}
        static Type cos(Type deg)					{ return std::cos(degToRad(deg));			}
        static Type tan(Type deg)					{ return std::tan(degToRad(deg));			}
        static Type arcSin(Type value)				{ return radToDeg(std::asin(value));		}
        static Type arcCos(Type value)				{ return radToDeg(std::acos(value));		}
        static Type arcTan2(Type valY, Type valX)	{ return radToDeg(std::atan2(valY, valX));	}
        static Type sqrt(Type value)				{ return std::sqrt(value);					}
        
        static Type pi()							{ return 3.141592653589793238462643383l;	}
        static Type radToDeg(Type rad)				{ return 180 / pi() * rad;					}
        static Type degToRad(Type deg)				{ return pi() / 180 * deg;					}
    };

/// Converts radians to degrees.
///
template <typename T>
T toDegree(T radian)
{
	return TrigonometricTraits<T>::radToDeg(radian);
}

/// @onverts degrees to radians.
///
template <typename T>
T toRadian(T degree)
{
	return TrigonometricTraits<T>::degToRad(degree);
}

///  The number %Pi (3.1415...)
///  Pi has the same value as TrigonometricTraits<float>::pi().
extern const float Pi;

}