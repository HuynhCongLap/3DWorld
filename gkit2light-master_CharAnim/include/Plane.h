#ifndef PLANE_H
#define PLANE_H
#include "mat.h"

class Plane
{
    public:
    Vector normal,point;
	float d;


	Plane( Vector v1,  Vector v2,  Vector v3);
	Plane();
	~Plane();

	void set3Points( Vector v1,  Vector v2,  Vector v3);
	void setNormalAndPoint(Vector normal, Vector point);
	void setCoefficients(float a, float b, float c, float d);
	float distance(Vector p);

	void print();
};

#endif // PLANE_H
