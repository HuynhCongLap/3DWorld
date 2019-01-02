#include "Plane.h"

Plane::Plane()
{
    //ctor
}

Plane::Plane(Vector v1,  Vector v2,  Vector v3)
{
    set3Points(v1,v2,v3);
}

void Plane::set3Points( Vector v1,  Vector v2,  Vector v3) {


	Vector aux1, aux2;

	aux1 = v1 - v2;
	aux2 = v3 - v2;

	normal = cross(aux2,aux1);
	normal = normalize(normal);

	point= v2;
	d = -(dot(normal,point));
}

void Plane::setNormalAndPoint(Vector normal, Vector point) {

	this->normal = normalize(normal);
	d = -(dot(this->normal,point));
}

void Plane::setCoefficients(float a, float b, float c, float d) {

	// set the normal vector
	normal = normalize(Vector(a,b,c));
	//compute the lenght of the vector
	float l = length(normal);
	this->d = d/l;
}




float Plane::distance(Vector p) {

	return (d + dot(normal,p));
}

Plane::~Plane()
{
    //dtor
}
