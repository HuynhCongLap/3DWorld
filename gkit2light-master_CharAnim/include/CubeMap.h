#ifndef CUBEMAP_H
#define CUBEMAP_H
#include "orbiter.h"
#include "image_io.h"
#include "program.h"
#include "uniforms.h"
class CubeMap
{
    public:
        CubeMap();
        virtual ~CubeMap();
        void drawCubeMap(Transform &view, Transform &projection);
        void init();


    protected:

    private:
        unsigned int program_cubemap;
        ImageData image;
        unsigned int vao_null;
        unsigned int texture;

};

#endif // CUBEMAP_H
