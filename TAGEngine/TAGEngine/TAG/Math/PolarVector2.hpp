#pragma once

#include "VectorAlgebra2D.hpp"
#include "Trigonometry.hpp"

namespace tag
{
// Vector in polar coordinate system
// 2D vector which stores its components in polar instead of cartesian coordinates.
template <typename T>
struct PolarVector2
{
	T	r;				///< Radius
	T	phi;			///< Angle in degrees

	// Default constructor
	// Creates a zero vector. The angle phi measures 0 degrees.
	PolarVector2();

	// Constructs a polar vector with specified radius and angle.
	// radius The radial component (length of the vector).
	// angle The angular component in degrees.
	PolarVector2(T radius, T angle);

	// Constructs a polar vector from a cartesian SFML vector.
	// vector Cartesian (x,y) vector being converted to polar coordinates.
	//  Zero vectors are allowed and result in a polar vector with r=0 and phi=0.
	PolarVector2(const sf::Vector2<T>& vector);

	// Converts the polar vector into a cartesian SFML vector.
	// Returns Equivalent (x,y) vector in cartesian coordinates.
	operator sf::Vector2<T> () const;
};

// Type definition for float polar vectors
typedef PolarVector2<float> PolarVector2f;

// PolarVector2
// Returns the length of a polar vector.
// The returned value is vector.r.
template <typename T>
T length(const PolarVector2<T>& vector);

// PolarVector2
// Returns the angle of a polar vector in degrees.
// The returned value is vector.phi.
template <typename T>
T polarAngle(const PolarVector2<T>& vector);

}

#include "PolarVector2.inl"