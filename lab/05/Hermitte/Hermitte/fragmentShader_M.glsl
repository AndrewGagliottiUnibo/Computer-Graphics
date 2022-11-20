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
	
	if (sceltaFS == 3)
	{
		vec2 position = ( gl_FragCoord.xy / res.xy ) + mouse / 4.0;

		float color = 0.0;
		color += sin( position.x * cos( time / 15.0 ) * 80.0 ) + sin( position.y * sin( time / 15.0 ) * 10.0 );
		color += sin( position.y * sin( time / 10.0 ) * 40.0 ) + cos( position.x * sin( time / 25.0 ) * 40.0 );
		color += sin( position.x * sin( time / 5.0 ) * 10.0 ) + sin( position.y * sin( time / 35.0 ) * 80.0 );
		color *= sin( time / 10.0 ) * 0.5;

		FragColor = vec4(color, color * 0.5, sin( color + time / 3.0 ) * 0.75 , 1.0 );
	}
}