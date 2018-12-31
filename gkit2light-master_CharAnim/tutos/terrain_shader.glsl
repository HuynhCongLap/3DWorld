
#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
layout(location= 1) in vec2 texcoord;
layout(location= 2) in vec3 normal;
layout(location= 3) in vec3 pos;

out vec2 vertex_texcoord;
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec4 screenPos; // position on screen
void main( )
{
    vec3 real_position = position + pos;
    vs_out.FragPos = vec3(model * vec4(real_position, 1.0));
    vs_out.Normal = mat3(transpose(inverse(model))) * normal;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position= projection * view * vec4(vs_out.FragPos, 1.0);
    screenPos = gl_Position;
    vertex_texcoord= texcoord;

}

#endif

#ifdef FRAGMENT_SHADER


out vec4 FragColor;

in vec2 vertex_texcoord;
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
} fs_in;
uniform sampler2D shadowMap;
uniform samplerCube texture0;

uniform sampler2D ice_texture;
uniform sampler2D land_texture;
uniform sampler2D mou_texture;
uniform sampler2D top_texture;
uniform sampler2D grass_side_texture;
uniform sampler2D top_grass;

uniform vec3 camera_pos_inverse;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform float adjust;
in vec4 screenPos;

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 10	

uniform PointLight pointLight[10];

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
void main( )
{
  if(screenPos.x/screenPos.w > 1 || screenPos.x/screenPos.w < -1 
                                 || screenPos.y/screenPos.w > 1 || screenPos.y/screenPos.w < -1)
	discard; // test if fragment out of screen or not
  // ambient
    vec3 color = vec3(1.0);
    vec4 textcolor = vec4(1.0);
    // calculate reflect
    vec3 m= reflect(normalize(fs_in.FragPos - camera_pos_inverse), normalize(fs_in.Normal));
    vec4 color1= texture(texture0, m);

    if(fs_in.FragPos.y > 17){
     	textcolor = texture(ice_texture, vertex_texcoord);
	color = vec3(textcolor);
    }
    else
       if(fs_in.FragPos.y > 14)
	{
		textcolor = texture(top_texture, vertex_texcoord);
		color = vec3(textcolor);
	}
	 else
       if(fs_in.FragPos.y > 10)
	{
		textcolor = texture(top_grass, vertex_texcoord);
		color = vec3(textcolor);
	}
	 else
       if(fs_in.FragPos.y > 7)
	{
		textcolor = texture(grass_side_texture, vertex_texcoord);
		color = vec3(textcolor);
	}
	 else
       if(fs_in.FragPos.y > 5)
	{
		textcolor = texture(mou_texture, vertex_texcoord);
		color = vec3(textcolor);
	}
	 else
       if(fs_in.FragPos.y > 1)
	{
		textcolor = texture(land_texture, vertex_texcoord);
		color = vec3(textcolor);
	}
	else
	{
  
	}

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = lightColor;
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    if(adjust > 2)
    {
	FragColor = color1;
	return;
    }
    lighting = lighting+vec3(adjust);
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        lighting += CalcPointLight(pointLight[i], normal, fs_in.FragPos, viewDir);
    FragColor= color1*vec4(lighting,1.0);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse;
    vec3 specular = light.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

#endif
