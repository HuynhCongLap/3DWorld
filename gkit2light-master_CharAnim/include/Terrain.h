#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>

#include "mat.h"
#include "image_io.h"
#include "mesh.h"
#include "program.h"
#include "uniforms.h"
#include "wavefront.h"
#include "Box.h"
#include "Shadow_map.h"
#include "CubeMap.h"
using namespace std;
class Terrain
{
    public:
        Terrain();
        virtual ~Terrain();
        // TERRAIN-------------
        void loadTerrain(const char* file); // load terrain from file
        void init_terrain();
        void draw_terrain(Orbiter &camera);
        void draw_simple_terrain(); // to get depth buffer for shadow
        //---BOUDING BOX------------------------
        void init_boxs();
        void draw_boxs();
        //----SHADOW---------------------------
        void init_shadow();
        void changeLightPosition(bool up_and_down);
        void drawLightSource();
        //--------CUBEMAP-----------------------------
        void drawCubeMap();
    protected:

    private:
        Orbiter m_camera;
    //------------------TERRAIN ATRIBUTE-----------------------------
        std::vector<vec3> m_cubePos ; // cube position
        Mesh m_terrain_mesh;
        unsigned int m_terrain_program, m_simple_terrain_program; // terrain shader

        unsigned int instanceVBO, terrain_vao, texcoord_buffer, normal_buffer, position_buffer;
        Transform m_model, m_view, m_projection;
        Point m_camera_pos;
     //----------------- BOUDING BOX -----------------------------------
        std::vector<Box> m_boxs;
        int scale_factor = 64;
    //--------------------SHADOW-----------------------------------------
        Shadow_map shadow;
        Point lightPos = Point(130,100,130);
        DrawParam gl;
        Transform t_light;
        Mesh m_light_mesh = read_mesh("data/cube.obj");
        //----------------CUBE MAP------------------------
        CubeMap m_cubemap;
        unsigned int sampler;
    //-------------------FRUSTRUM-------------------------------------

};

#endif // TERRAIN_H
