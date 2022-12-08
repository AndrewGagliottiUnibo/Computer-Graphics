#version 330 core
in vec4 ourColor;
out vec4 FragColor;  
 
uniform int sceltaFS;
uniform float time;
uniform vec2 res;
uniform vec2 mouse;
#define PI 3.1415

void main()
{
	if (sceltaFS==0)
    {
        FragColor = vec4(ourColor);
    }
	
	if (sceltaFS == 1)
	{
		FragColor = vec4(sin(time / 800.0) * 0.5, 0.0, tan(time / 800.0), 0.1 );
	}
}