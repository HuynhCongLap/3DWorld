#include "Box.h"

Box::Box()
{
    //ctor
}

Box::Box(int scale_factor, Point box_position, int ID, int number_cube)
{
    m_scale_factor = scale_factor;
    m_number_cubes = number_cube;
    m_scale = Scale(m_scale_factor,m_scale_factor,m_scale_factor);
    m_box_position = box_position;
    m_translation = Translation(m_box_position.x,m_box_position.y,m_box_position.z);
    ID_INSTANCE = ID;
}

Box::~Box()
{
    //dtor
}

void Box::init()
{
    m_box_mesh = read_mesh("data/cube.obj");
}
void Box::draw_box(Orbiter &camera)
{

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    draw(m_box_mesh,m_translation*m_scale,camera);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}



