
//! \file tuto5GL.glsl

#version 330

#ifdef VERTEX_SHADER

layout(location= 0) in vec3 position;
layout(location= 1) in vec2 texcoord;
layout(location= 2) in vec3 normal;

uniform mat4 mvpMatrix;
uniform mat4 mMatrix;

out vec3 vertex_position;
out vec3 vertex_normal;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    vertex_position= vec3(mMatrix * vec4(position, 1));
    vertex_normal= mat3(mMatrix) * normal;
}
#endif


#ifdef FRAGMENT_SHADER
uniform vec3 camera_position;
in vec3 vertex_position;
in vec3 vertex_normal;
uniform samplerCube texture0;
out vec4 fragment_color;

void main( )
{ 
    vec3 m= reflect(normalize(vertex_position - camera_position), normalize(vertex_normal));
    vec4 color= texture(texture0, m);
    fragment_color= color;
}
#endif
