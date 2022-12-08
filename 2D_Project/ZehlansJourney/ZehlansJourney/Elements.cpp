#include "Elements.h"
#include "Shape.h"

// Constructor not used
Elements::Elements() {}

// Elements
Shape *background = new Shape();
Shape *star = new Shape[100];
Shape *blob = new Shape();
Shape *motherShip = new Shape();
Shape *bullet = new Shape();

Shape *Elements::getBackground()
{
	return background;
}

Shape *Elements::getStars(int index)
{
	return &star[index];
}

void Elements::setStars(Shape *star)
{
	star = star;
}

Shape *Elements::getBlob()
{
	return blob;
}

Shape *Elements::getMotherShip()
{
	return motherShip;
}

Shape *Elements::getBullet()
{
	return bullet;
}

void Elements::setBlob(Shape *blob)
{
	blob = blob;
}

void Elements::setBullet(Shape *bullet)
{
	bullet = bullet;
}

void Elements::setMotherShip(Shape *ship)
{
	ship = ship;
}
