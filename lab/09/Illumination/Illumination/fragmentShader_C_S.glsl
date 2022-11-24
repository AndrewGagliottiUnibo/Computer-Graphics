#version 330 core
in vec4 ourColor;
out vec4 FragColor;  

in vec3 N, L,R,V;
uniform float time;
uniform vec2 resolution;
uniform int sceltaFS;
 
struct PointLight{
	vec3 position;
	vec3 color;
	float power;
 };

 //definizione di una varialie uniform che ha la struttura PointLight
uniform PointLight light;
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

 #define pi 3.14159235659
 
void main()
{
    if(sceltaFS==0 || sceltaFS == 1)
    // Shading interpolativo
        FragColor = vec4(ourColor);
}