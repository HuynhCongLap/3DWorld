#ifndef SHADOW_MAP_H
#define SHADOW_MAP_H

#include "mat.h"
#include "/usr/include/GL/glew.h"
class Shadow_map
{
    public:
        Shadow_map();
        virtual ~Shadow_map();
        void init();
        void getLightPoint(Point light_pos);
        Transform lightProjection();
        Transform lightView();
        Transform lightSpaceMatrix();

        int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
        unsigned int depthMapFBO;
        unsigned int depthMap;
        float near_plane = 1.0f, far_plane= 500.0f;
        Point m_lightPos;
};

#endif // SHADOW_H
