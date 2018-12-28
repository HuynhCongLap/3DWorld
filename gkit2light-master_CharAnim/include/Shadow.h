#ifndef SHADOW_H
#define SHADOW_H

#include "mat.h"

class Shadow
{
    public:
        Shadow();
        virtual ~Shadow();
        void init();
        Transform lightProjection();
        Transform lightView();
        Transform lightSpaceMatrix();

    private:
    int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    unsigned int depthMap;
    float near_plane = 1.0f, far_plane= 7.5f;
    Point lightPos;
};

#endif // SHADOW_H
