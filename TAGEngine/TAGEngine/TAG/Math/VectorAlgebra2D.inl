namespace tag
{

template <typename T>
T length(const sf::Vector2<T>& vector)
{
	return TrigonometricTraits<T>::sqrt(squaredLength(vector));
}

template <typename T>
T squaredLength(const sf::Vector2<T>& vector)
{
	return dotProduct(vector, vector);
}

template <typename T>
void setLength(sf::Vector2<T>& vector, T newLength)
{
	assert(vector != sf::Vector2<T>());
	vector *= newLength / length(vector);
}

template <typename T>
sf::Vector2<T> unitVector(const sf::Vector2<T>& vector)
{
	assert(vector != sf::Vector2<T>());
	return vector / length(vector);
}

template <typename T>
T polarAngle(const sf::Vector2<T>& vector)
{
	assert(vector != sf::Vector2<T>());
	return TrigonometricTraits<T>::arcTan2(vector.y, vector.x);
}

template <typename T>
void setPolarAngle(sf::Vector2<T>& vector, T newAngle)
{
	// No assert here, because turning a zero vector is well-defined (yields always zero vector)

	T vecLength = length(vector);

	vector.x = vecLength * TrigonometricTraits<T>::cos(newAngle);
	vector.y = vecLength * TrigonometricTraits<T>::sin(newAngle);
}

template <typename T>
void rotate(sf::Vector2<T>& vector, T angle)
{
	// No assert here, because turning a zero vector is well-defined (yields always zero vector)

	T cos = TrigonometricTraits<T>::cos(angle);
	T sin = TrigonometricTraits<T>::sin(angle);

	// Don't manipulate x and y separately, otherwise they're overwritten too early
	vector = sf::Vector2<T>( 
		cos * vector.x - sin * vector.y,
		sin * vector.x + cos * vector.y);
}

template <typename T>
sf::Vector2<T> rotatedVector(const sf::Vector2<T>& vector, T angle)
{
	// No assert here, because turning a zero vector is well-defined (yields always zero vector)

	sf::Vector2<T> copy = vector;
	rotate(copy, angle);
	return copy;
}

template <typename T>
sf::Vector2<T> perpendicularVector(const sf::Vector2<T>& vector)
{
	return sf::Vector2<T>(-vector.y, vector.x);
}

// ---------------------------------------------------------------------------------------------------------------------------


template <typename T>
T signedAngle(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
{
	assert(lhs != sf::Vector2<T>() && rhs != sf::Vector2<T>());
	return TrigonometricTraits<T>::arcTan2(crossProduct(lhs, rhs), dotProduct(lhs, rhs));
}

template <typename T>
T dotProduct(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

template <typename T>
T crossProduct(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
{
	return lhs.x * rhs.y - lhs.y * rhs.x;
}
    
template <typename T>
T crossProduct(const sf::Vector2<T>& a, const sf::Vector2<T>& b, const sf::Vector2<T>& c)
{
    sf::Vector2f BA = a - b;
    sf::Vector2f BC = c - b;
    return (BA.x * BC.y - BA.y * BC.x);
}

template <typename T>
sf::Vector2<T> cwiseProduct(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
{
	return sf::Vector2<T>(lhs.x * rhs.x, lhs.y * rhs.y);
}

template <typename T>
sf::Vector2<T> cwiseQuotient(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
{
	assert(rhs.x != 0 && rhs.y != 0);
	return sf::Vector2<T>(lhs.x / rhs.x, lhs.y / rhs.y);
}

template <typename T>
sf::Vector2<T> projectedVector(const sf::Vector2<T>& vector, const sf::Vector2<T>& axis)
{
	assert(axis != sf::Vector2<T>());
	return dotProduct(vector, axis) / squaredLength(axis) * axis;
}

}