#include "Spaceship.h"

Spaceship::Spaceship()
{

}

void Spaceship::init()
{
    program= read_program("tutos/spaceship.glsl");
    program_print_errors(program);

    mesh= read_mesh("data/UFO.obj");
  // vertex format : position + normal,
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // position buffer
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertex_buffer_size(),mesh.vertex_buffer(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // texcoord buffer
    glGenBuffers(1, &texcoord_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, texcoord_buffer);
    glBufferData(GL_ARRAY_BUFFER, mesh.texcoord_buffer_size(), mesh.texcoord_buffer(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // normal
    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, mesh.normal_buffer_size(), mesh.normal_buffer(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    // nettoyage
    //mesh.release();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void Spaceship::draw(Transform mvp, Transform model, Transform view, unsigned int& texture)
{


    glBindVertexArray(vao);
    glUseProgram(program);

    program_uniform(program, "mvpMatrix", mvp);
    program_uniform(program, "mMatrix", model);
    program_uniform(program, "camera_position", Inverse(view)(Point(0, 0, 0)));

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    program_uniform(program, "texture0", 0);

    glDrawArrays(GL_TRIANGLES, 0, mesh.vertex_count());

}
Spaceship::~Spaceship()
{
    //dtor
}
