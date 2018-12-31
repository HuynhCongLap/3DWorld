#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>

#include "mat.h"
#include "image_io.h"
#include "mesh.h"
#include "program.h"
#include "uniforms.h"
#include "wavefront.h"
#include "texture.h"
#include "Box.h"
#include "Shadow_map.h"
#include "CubeMap.h"
#include "Spaceship.h"
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
        void adjust_light(bool add);
        //---BOUDING BOX------------------------
        void init_boxs();
        void draw_boxs();
        //----SHADOW---------------------------
        void init_shadow();
        void changeLightPosition(bool up_and_down);
        void drawLightSource();
        //--------CUBEMAP-----------------------------
        void drawCubeMap();
        //-----------Spaceship-----------------------
        void getSpaceShipPositition(Point pos)
        {
            m_space_ship_pos = pos;
            m_space_ship_model = Translation(pos.x,pos.y,pos.z)*RotationY(-global_time()/10)*Scale(0.01,0.005,0.01);

        }
    protected:

    private:
        Orbiter m_camera;
    //------------------TERRAIN ATRIBUTE-----------------------------
        float max_height = 0;
        int layer = 20; // number of layer in terrain, like in minecraft, 255 layer base on altitude
        std::vector<vec3> m_cubePos ; // cube position
        Mesh m_terrain_mesh;
        unsigned int m_terrain_program, m_simple_terrain_program; // terrain shader

        unsigned int instanceVBO, terrain_vao, texcoord_buffer, normal_buffer, position_buffer;
        Transform m_model, m_view, m_projection;
        Point m_camera_pos;
        float m_adjust = 0.1; // use to adjust the light of terrain
        GLuint materials[6];
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
        //-----------------SPACESHIP----------------------
        Transform m_space_ship_model;
        Spaceship m_space_ship;
        Point m_space_ship_pos;
        int nb_light = 10;
        vector <Point> static_light_pos;
        //-----------------------------------------------
         char light_pos[30] = "pointLight[0].position";
         char light_ambient[30] = "pointLight[0].ambient";
         char light_diffuse[30] = "pointLight[0].diffuse";
         char light_specular[30] = "pointLight[0].specular";
         char light_constant[30] = "pointLight[0].constant";
         char light_lightnear[30] = "pointLight[0].lightnear";
         char light_quadratic[30] =  "pointLight[0].quadratic";
    //-------------------FRUSTRUM-------------------------------------

};

#endif // TERRAIN_H
