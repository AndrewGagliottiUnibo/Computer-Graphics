#pragma once
#include "Shape.h"

class Elements
{
public:
	Elements();

	// Background elements
	Shape *getBackground();
	Shape *getStars(int index);
	void setStars(Shape *star);

	// Enemy entities
	Shape *getBlob();
	Shape *getMotherShip();
	Shape *getBullet();
	void setBlob(Shape *blob);
	void setBullet(Shape *bullet);
	void setMotherShip(Shape *ship);

private:
protected:
};
