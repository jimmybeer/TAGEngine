namespace tag
{

template <typename T>
void setColor(T& object, const sf::Color& color)
{
    object.setColor(color);
}

template <typename T>
void setAlpha(T& object, sf::Uint8 alpha)
{
    sf::Color color = object.getColor();
	color.a = alpha;
	object.setColor(color);
}

}