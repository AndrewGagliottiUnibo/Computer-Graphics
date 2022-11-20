#version 330 core
in vec4 ourColor;
out vec4 FragColor;  
uniform float time;
uniform vec2 resolution;
 
 vec3 color;
 
void main()
{
// Convert screen coordinates to normalized device coordinates (NDC)
 
   vec2 ndc = vec2((gl_FragCoord.x / resolution.x - 0.5) * 2, (gl_FragCoord.y / resolution.y - 0.5) * 2);
   
   if(ndc.y> -0.9 && ndc.y<-0.8)
     {
      color = vec3(0,abs(sin(ndc.y*10.5+time)),abs(sin(ndc.y*time)));
      FragColor =  vec4(color,1.0);
    }
    //Costruisce un cerchio con centro in (-0.8,-0.8) e raggio 0.1
    else if (distance(ndc.xy, vec2(-0.8,-0.8)) < 0.1)
		FragColor= vec4(0.0,0.0,abs(sin(ndc.y*10.5+time)),1.0);
 
        
    else
      FragColor = ourColor;
}