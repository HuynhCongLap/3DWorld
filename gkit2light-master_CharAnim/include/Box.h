#ifndef BOX_H
#define BOX_H

#include "mesh.h"
#include "draw.h"
#include "wavefront.h"
#include <vector>
#include "vec.h"
class Box
{
    public:
        void init();
        void draw_box(Orbiter &camera);
        Box();
        Box(int scale_factor, Point box_position,int ID, int number_cube);
        virtual ~Box();

        int ID_INSTANCE;
        int m_number_cubes;
    protected:

    private:

        Mesh m_box_mesh;
        int m_scale_factor = 64;
        Transform m_scale;
        Transform m_translation;
        Point m_box_position;
        Point m_box_points[8];
};

#endif // BOX_H
