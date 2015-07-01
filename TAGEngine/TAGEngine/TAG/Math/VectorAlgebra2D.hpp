#pragma once

#include "Trigonometry.hpp"

#include <SFML/System/Vector2.hpp>

#include <cassert>

// Algebraic operations on two-dimensional vectors (dot product, vector length and angles, ...)
// Here is an overview over the functionality:
//
// Operation		Translation		Scale			Rotation
// Get				v				length(v)		polarAngle(v)
// Set, absolute	v = w			setLength(v,l)	setPolarAngle(v,a)
// Set, relative	v += w			v *= l			rotate(v,a)
// Copy, relative	v + w			v * l			rotatedVector(v,a)

// Other functions:
// dotProduct(v,w)		unitVector(v)			squaredLength(v)
// crossProduct(v,w)	perpendicularVector(v)	signedAngle(v,w)
// cwiseProduct(v,w)	cwiseQuotient(v,w)		projectedVector(v,w)

namespace tag
{

// Returns the length of the 2D vector.
template <typename T>
T length(const sf::Vector2<T>& vector);

/// Returns the square of a vector's length.
/// Suitable for comparisons, more efficient than length().
template <typename T>
T squaredLength(const sf::Vector2<T>& vector);

///  Adapts a vector so that its length is |a newLength| after this operation.
///  If a newLength is less than zero, the vector's direction changes.
/// a vector is no zero vector.
template <typename T>
void setLength(sf::Vector2<T>& vector, T newLength);

// Returns a vector with same direction as the argument, but with length 1.
// a vector is no zero vector.
template <typename T>
sf::Vector2<T> unitVector(const sf::Vector2<T>& vector);

// Returns the polar angle.
// The vector (1,0) corresponds 0 degrees, (0,1) corresponds 90 degrees.
/// Returns Angle in degrees in the interval [-180,180].
// a vector is no zero vector.
template <typename T>
T polarAngle(const sf::Vector2<T>& vector);

// Sets the polar angle of the specified vector.
// The vector (1,0) corresponds 0 degrees, (0,1) corresponds 90 degrees.
template <typename T>
void setPolarAngle(sf::Vector2<T>& vector, T newAngle);

// Rotates the vector by the given angle (in degrees).
// The vector (1,0) corresponds 0 degrees, (0,1) corresponds 90 degrees.
template <typename T>
void rotate(sf::Vector2<T>& vector, T angle);

// Returns a copy of the vector, rotated by angle degrees.
// The vector (1,0) corresponds 0 degrees, (0,1) corresponds 90 degrees.
template <typename T>
sf::Vector2<T> rotatedVector(const sf::Vector2<T>& vector, T angle);

// Returns a perpendicular vector.
// Returns vector turned by 90 degrees counter clockwise; (x,y) becomes (-y,x).
//  For example, the vector (1,0) is transformed to (0,1).
template <typename T>
sf::Vector2<T> perpendicularVector(const sf::Vector2<T>& vector);

// ---------------------------------------------------------------------------------------------------------------------------


// Computes the signed angle from lhs to rhs.
// returns Angle in degrees in the interval [-180,180]. The angle determines how much you have to turn lhs
//  until it points to the same direction as rhs.
// Neither lhs nor rhs is a zero vector.
template <typename T>
T signedAngle(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs);

// Computes the dot product of two 2D vectors.
template <typename T>
T dotProduct(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs);

// Computes the cross product of two 2D vectors (Z component only).
// Treats the operands as 3D vectors, computes their cross product and returns the result's Z component
//  (X and Y components are always zero).
template <typename T>
T crossProduct(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs);

// Returns the component-wise product of lhs and rhs.
// Computes (lhs.x*rhs.x, lhs.y*rhs.y). Component-wise multiplications are mainly used for scales.
template <typename T>
sf::Vector2<T>				cwiseProduct(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs);

/// @brief Returns the component-wise quotient of @a lhs and @a rhs.
/// @details Computes <i>(lhs.x/rhs.x, lhs.y/rhs.y)</i>. Component-wise divisions are mainly used for scales.
/// @pre Neither component of @a rhs is zero.
template <typename T>
sf::Vector2<T> cwiseQuotient(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs);

// Returns the projection of vector onto axis.
// vector Vector to project onto another.
// axis Vector being projected onto. Need not be a unit vector, but may not have length zero.
template <typename T>
sf::Vector2<T> projectedVector(const sf::Vector2<T>& vector, const sf::Vector2<T>& axis);

}

#include "VectorAlgebra2D.inl"