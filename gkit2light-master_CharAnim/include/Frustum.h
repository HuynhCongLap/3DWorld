#ifndef FRUSTUM_H
#define FRUSTUM_H
#include "Plane.h"
#include "Box.h"
class Frustum
{
    private:

	enum {
		TOP = 0,
		BOTTOM,
		LEFT,
		RIGHT,
		NEARP,
		FARP
	};
    public:
        enum {OUTSIDE, INTERSECT, INSIDE};
        Frustum();
        virtual ~Frustum();
        Plane pl[6];


        Vector ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
        float nearD, farD, ratio, angle,tang;
        float nw,nh,fw,fh;

        void setCamInternals(float angle, float ratio, float nearD, float farD);
	    void setCamDef(Vector p, Vector l, Vector u);
        int boxInFrustum(Box b); // visibility Test

    protected:

    private:
};

#endif // FRUSTUM_H
