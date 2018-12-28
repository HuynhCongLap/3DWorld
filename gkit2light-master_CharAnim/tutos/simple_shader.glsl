
#version 330
#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
layout(location= 1) in vec2 texcoord;
layout(location= 2) in vec3 normal;
layout(location= 3) in vec3 pos;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;


void main( )
{
    vec3 real_position = position + pos;
    vec3 FragPos = vec3(model * vec4(real_position, 1.0));
    gl_Position= lightSpaceMatrix * vec4(FragPos, 1.0);

}
#endif


#ifdef FRAGMENT_SHADER
void main()
{
    // gl_FragDepth = gl_FragCoord.z;
}

#endif
