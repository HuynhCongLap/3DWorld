#ifndef SPACESHIP_H
#define SPACESHIP_H


#include "mesh.h"
#include "wavefront.h"
#include "program.h"
#include "orbiter.h"
#include "uniforms.h"
class Spaceship
{
    public:
        Spaceship();
        void init();
        void draw(Transform mvp, Transform model, Transform view,unsigned int& texture);

        virtual ~Spaceship();

    protected:

    private:
    unsigned int program;
    Mesh mesh;

    unsigned int vao;
    unsigned int position_buffer;
    unsigned int normal_buffer;
    unsigned int texcoord_buffer;
    int vertex_count;
};

#endif // SPACESHIP_H
