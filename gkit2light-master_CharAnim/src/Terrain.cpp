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
    float scale = 20;

    for(int y= 0; y +1 < data.height(); y++)
    for(int x= 0; x +1 < data.width(); x++)
    {
         if(data(x,y).r > max_height)
            max_height = data(x,y).r;
    }
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
                m_cubePos.push_back(vec3(ix,int(data(ix, iy).r/max_height*layer),iy));
                cube_number++;
            }
            m_boxs.push_back(Box(scale_factor, Point(x,0,y),count,cube_number));
            count += cube_number;
            cout<<"Cube numbers: "<<cube_number<<endl;
            cube_number = 0;
        }
    }

    static_light_pos.push_back(m_cubePos[23000]);
    static_light_pos.push_back(m_cubePos[10455]);
    static_light_pos.push_back(m_cubePos[34800]);
    static_light_pos.push_back(m_cubePos[997]);
    static_light_pos.push_back(m_cubePos[30980]);
    static_light_pos.push_back(m_cubePos[39168]);
    static_light_pos.push_back(m_cubePos[35072]);
    static_light_pos.push_back(m_cubePos[39168+64*2+32]); //
    static_light_pos.push_back(m_cubePos[52870]);
}

void Terrain::init_terrain() // bind VAO and VBO
{
    volume.setCamInternals(45,window_width()/window_height(),0.1,500);
    m_model = Scale(1,1,1);
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
    glGenerateMipmap(GL_TEXTURE_2D);
    materials[0] = read_texture(0, "data/blocks/snow.png");
    materials[1] = read_texture(0, "data/blocks/wool_colored_lime.png");
    materials[2] = read_texture(0, "data/blocks/concrete_green.png");
    materials[3] = read_texture(0, "data/blocks/concrete_brown.png");
    materials[4] = read_texture(0, "data/blocks/grass_side.png");
    materials[5] = read_texture(0, "data/blocks/dirt.png");
    glBindTexture(GL_TEXTURE_2D,0);

    m_space_ship.init();
}
float ran()
{
    return ((float)rand()/(float)(RAND_MAX));
}
void Terrain::draw_terrain(Orbiter &camera)
{
    m_model = Scale(Scale_box,Scale_box,Scale_box);
    m_camera = camera;
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
    program_uniform(m_terrain_program, "adjust",m_adjust);
    program_uniform(m_terrain_program, "layer",layer);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap.texture);
    glBindSampler(0, sampler);
    // texture
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, shadow.depthMap);
    glBindSampler(1, sampler);

    for(int i=0; i<6; i++)
    {
        glActiveTexture(GL_TEXTURE0+2+i);
        glBindTexture(GL_TEXTURE_2D, materials[i]);
        glBindSampler(2+i, sampler);
    }

    program_uniform(m_terrain_program, "texture0", 0);
    program_uniform(m_terrain_program, "shadowMap",1);
    program_uniform(m_terrain_program, "ice_texture",2);
    program_uniform(m_terrain_program, "land_texture",3);
    program_uniform(m_terrain_program, "mou_texture",4);
    program_uniform(m_terrain_program, "top_texture",5);
    program_uniform(m_terrain_program, "grass_side_texture",6);
    program_uniform(m_terrain_program, "top_grass",7);

    program_uniform(m_terrain_program,"pointLight[0].position", m_space_ship_pos);
    program_uniform(m_terrain_program,"pointLight[0].ambient", vec3(ran(), ran(), ran()));
    program_uniform(m_terrain_program,"pointLight[0].diffuse", vec3(ran(), ran(), ran()));
    program_uniform(m_terrain_program,"pointLight[0].specular", vec3(ran(), ran(), ran()));
    program_uniform(m_terrain_program,"pointLight[0].constant", float(1));
    program_uniform(m_terrain_program,"pointLight[0].linear", float(0.09));
    program_uniform(m_terrain_program,"pointLight[0].quadratic", float(0.032));
    for(int i=1; i<nb_light; i++){
        light_pos[11] =  i+'0';
        light_ambient[11] =  i+'0';
        light_diffuse[11] =  i+'0';
        light_specular[11] =  i+'0';
        light_constant[11] =  i+'0';
        light_lightnear[11] =  i+'0';
        light_quadratic[11] =  i+'0';

        Point l_pos = Point(static_light_pos[i-1].x,static_light_pos[i-1].y-2,static_light_pos[i-1].z);
        program_uniform(m_terrain_program,light_pos, l_pos);
        float cos1 = abs(cos(i+global_time()/1000));
        float sin1 = abs(sin(i+global_time()/1000));
        program_uniform(m_terrain_program,light_ambient, vec3( cos1, sin1*cos1,sin1));
        program_uniform(m_terrain_program,light_diffuse, vec3(cos1, sin1*cos1,sin1));
        program_uniform(m_terrain_program,light_specular, vec3(0.5, 0.5, 0.5));

        program_uniform(m_terrain_program,light_constant, float(1.5));

        program_uniform(m_terrain_program,light_lightnear, float(0.03));

        program_uniform(m_terrain_program,light_quadratic, float(0.01));

    }
    glBindVertexArray(terrain_vao);
    for(int i=0 ; i<m_boxs.size(); i++) // Test visibility
    {
     if(volume.boxInFrustum(m_boxs[i])) //De-comment this line to see the visibility test
     glDrawArraysInstancedBaseInstance(GL_TRIANGLES,0, m_terrain_mesh.vertex_count(), m_boxs[i].m_number_cubes,m_boxs[i].ID_INSTANCE);
    }
    glUseProgram(0);
    glBindVertexArray(0);
    glActiveTexture(0);

    m_space_ship.draw(m_projection*m_view*m_space_ship_model,m_space_ship_model,m_view,m_cubemap.texture);

}

void Terrain::draw_simple_terrain()
{

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

void  Terrain::adjust_light(bool add) // adjust the light of terrain
{
    if(add)
        m_adjust+=0.1; // add the light of reflect to terrain
    else
        m_adjust-=0.1;
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





