#include "Terrain.h"

Terrain::Terrain()
{
    //ctor
}

Terrain::~Terrain()
{
    release_program(m_terrain_program);
    m_terrain_mesh.release();
    glDeleteVertexArrays(1, &terrain_vao);
}

//------------------TERRAIN------------------------------
void Terrain::loadTerrain(const char* filename)
{
    Image data = read_image(filename);
    float scale = 50;


    int count = 0;
    int cube_number = 0;
    for(int y= 0; y +1 < data.height(); y +=scale_factor)
    {
        for(int x= 0; x +1 < data.width(); x+= scale_factor)
        {
            cout<<"instance ID: "<<count<<endl;


            for(int iy= y; iy  < y +scale_factor && iy +1 < data.height(); iy++)
            for(int ix= x; ix  < x +scale_factor && ix +1 < data.width(); ix++)
            {
                m_cubePos.push_back(vec3(ix,data(ix, iy).r * scale,iy));
                cube_number++;
            }
            m_boxs.push_back(Box(scale_factor, Point(x,0,y),count,cube_number));
            count += cube_number;
            cout<<"Cube numbers: "<<cube_number<<endl;
            cube_number = 0;
        }
    }
}

void Terrain::init_terrain() // bind VAO and VBO
{
    m_cubemap.init();
    m_terrain_mesh = read_mesh("data/cube.obj");
    m_terrain_program = read_program("tutos/terrain_shader.glsl");
    program_print_errors(m_terrain_program);

    m_simple_terrain_program = read_program("tutos/simple_shader.glsl");
    program_print_errors(m_simple_terrain_program);

    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m_cubePos.size(), &m_cubePos[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //---------------------------------------------
    glGenVertexArrays(1, &terrain_vao);
    glBindVertexArray(terrain_vao);

    // position buffer
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_terrain_mesh.vertex_buffer_size(), m_terrain_mesh.vertex_buffer(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // texcoord buffer
    glGenBuffers(1, &texcoord_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, texcoord_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_terrain_mesh.texcoord_buffer_size(), m_terrain_mesh.texcoord_buffer(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // normal
    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_terrain_mesh.normal_buffer_size(), m_terrain_mesh.normal_buffer(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // instance position
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(3, 1);
    glEnableVertexAttribArray(3);

     // nettoyage
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindTexture(GL_TEXTURE_2D,0);
}

void Terrain::draw_terrain(Orbiter &camera)
{
    m_camera = camera;
    m_model = Identity();
    m_view  = m_camera.view();
    m_projection = m_camera.projection(window_width(),window_height(),45);

    glUseProgram(m_terrain_program);

    program_uniform(m_terrain_program, "model",m_model);
    program_uniform(m_terrain_program, "view",m_view);
    program_uniform(m_terrain_program, "projection",m_projection);
    program_uniform(m_terrain_program, "viewPos",m_view);
    program_uniform(m_terrain_program, "camera_pos_inverse",Inverse(m_view)(Point(0, 0, 0)));
    program_uniform(m_terrain_program, "lightSpaceMatrix",shadow.lightSpaceMatrix());

    program_uniform(m_terrain_program, "lightPos",lightPos);
    program_uniform(m_terrain_program, "lightColor",Point(1.0f, 1.0f, 1.0f));
    program_uniform(m_terrain_program, "objectColor",Point(1.0f, 0.5f, 0.31f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap.texture);

    // texture
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, shadow.depthMap);

    program_uniform(m_terrain_program, "shadowMap",1);
    program_uniform(m_terrain_program, "texture0", 0);
    glBindVertexArray(terrain_vao);
    for(int i=0 ; i<m_boxs.size(); i++) // Test visibility
     glDrawArraysInstancedBaseInstance(GL_TRIANGLES,0, m_terrain_mesh.vertex_count(), m_boxs[i].m_number_cubes,m_boxs[i].ID_INSTANCE);

    glUseProgram(0);
    glBindVertexArray(0);
    glActiveTexture(0);

    //program_uniform(m_terrain_program, "mvpMatrix",m_projection*m_view*m_model);
}

void Terrain::draw_simple_terrain()
{
    m_model = Identity();
    glUseProgram(m_simple_terrain_program);
    program_uniform(m_simple_terrain_program, "model",m_model);
    program_uniform(m_simple_terrain_program, "lightSpaceMatrix", shadow.lightSpaceMatrix());
    glViewport(0, 0, shadow.SHADOW_WIDTH, shadow.SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow.depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(terrain_vao);
    for(int i=0 ; i<m_boxs.size(); i++) // Test visibility
     glDrawArraysInstancedBaseInstance(GL_TRIANGLES,0, m_terrain_mesh.vertex_count(), m_boxs[i].m_number_cubes,m_boxs[i].ID_INSTANCE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // reset viewport
    glViewport(0, 0, window_width(), window_height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(0);
    glBindVertexArray(0);
}

//-------------------SHADOW------------------------
void Terrain::init_shadow()
{
    shadow.init();
    shadow.getLightPoint(lightPos);
}

void Terrain::changeLightPosition(bool up_and_down)
{
    if(up_and_down)
        lightPos.y++;
    else
        lightPos.y--;
}

void Terrain::drawLightSource()
{
    t_light = Translation(Vector(lightPos));
	draw(m_light_mesh,t_light,m_camera);
}

//--------CUBEMAP-----------------------------
void Terrain::drawCubeMap()
{
    m_cubemap.drawCubeMap(m_view,m_projection);
}

//---BOUDING BOX------------------------
void Terrain::init_boxs()
{
    for(int i=0; i<m_boxs.size(); i++){
        m_boxs[i].init();
    }
    cout<<"Number of boxs: "<<m_boxs.size()<<endl;


}
void Terrain::draw_boxs()
{
    for(int i=0; i<m_boxs.size(); i++){
        m_boxs[i].draw_box(m_camera);
    }
}




