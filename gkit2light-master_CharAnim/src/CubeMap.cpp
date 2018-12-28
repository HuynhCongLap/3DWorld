#include "CubeMap.h"

CubeMap::CubeMap()
{

}
void CubeMap::init()
{
       // . dessiner la cubemap a l'infini
    program_cubemap= read_program("tutos/cubemap.glsl");
    program_print_errors(program_cubemap);
    image= read_image_data("tutos/cubemap_debug.png");
    int size= image.width / 6;

    GLenum data_format;
    GLenum data_type= GL_UNSIGNED_BYTE;
    if(image.channels == 3)
        data_format= GL_RGB;
    else // par defaut
        data_format= GL_RGBA;

    // creer la texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    // creer les 6 faces
    // chaque face de la cubemap est un rectangle dans l'image originale :
    // largeur totale de l'image
    glPixelStorei(GL_UNPACK_ROW_LENGTH, image.width);
    // position du coin du rectangle dans l'image originale
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0*size);

    // transferer les pixels
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,
        GL_RGBA, size, size, 0,
        data_format, data_type, image.buffer());

    // position du coin de l'image en pixel
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 1*size);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0,
        GL_RGBA, size, size, 0,
        data_format, data_type, image.buffer());

    // position du coin de l'image en pixel
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 2*size);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0,
        GL_RGBA, size, size, 0,
        data_format, data_type, image.buffer());

    // position du coin de l'image en pixel
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 3*size);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0,
        GL_RGBA, size, size, 0,
        data_format, data_type, image.buffer());

    // position du coin de l'image en pixel
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 4*size);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0,
        GL_RGBA, size, size, 0,
        data_format, data_type, image.buffer());

    // position du coin de l'image en pixel
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 5*size);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0,
        GL_RGBA, size, size, 0,
        data_format, data_type, image.buffer());
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // filtrage "correct" sur les bords du cube...
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // nettoyage
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

    // etape 4 : vao pour dessiner la cubemap a l'infini
    glGenVertexArrays(1, &vao_null);
    glBindVertexArray(vao_null);
    // pas de buffer, c'est le vertex shader qui genere directement les positions des sommets

    glUseProgram(0);
    glBindVertexArray(0);
}

void CubeMap::drawCubeMap(Transform &view, Transform &projection)
{
        glBindVertexArray(vao_null);
        glUseProgram(program_cubemap);

        // texture
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

        // sampler2D declare par le fragment shader
        GLint location= glGetUniformLocation(program_cubemap, "texture0");
        glUniform1i(location, 0);
        // ou program_uniform(program, "texture0", 0);

        program_uniform(program_cubemap, "vpInvMatrix", Inverse(projection * view));
        program_uniform(program_cubemap, "camera_position", Inverse(view)(Point(0, 0, 0)));

        // dessine un triangle qui couvre tous les pixels de l'image
        glDrawArrays(GL_TRIANGLES, 0, 3);

    // nettoyage
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glUseProgram(0);
    glBindVertexArray(0);
}

CubeMap::~CubeMap()
{
    //dtor
}

