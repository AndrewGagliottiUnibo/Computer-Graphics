#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec4 aColor; // the color variable has attribute position 1
layout (location = 2 ) in vec3 vertexNormal; // Attributo Normale 2 in coordinate dell'oggetto che devono essere trasformate in coordinate di vista

uniform mat4 Model;
uniform mat4 Projection;
uniform mat4 View;
uniform int sceltaVS;
uniform float time;
uniform vec3 viewPos;

//Struttura per la gestione di un punto luce
struct PointLight{
	vec3 position;
	vec3 color;
	float power;
 };

 //definizione di una varialie uniform che hala struttura PointLight
uniform PointLight light;

//Struttura per la gestione di un materiale
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

uniform Material material;
out vec4 ourColor; // output a color to the fragment shader
out vec3 N,V,L,R;
 
void main()
{
    ourColor = aColor; // set ourColor to the input color we got from the vertex data
     
    if (sceltaVS == 0)
    {
        gl_Position = Projection*View*Model*vec4(aPos, 1.0); 
    }

    // Modello di shading di Phong con shading interpolativo
    if (sceltaVS == 1) 
    {
        gl_Position = Projection*View*Model*vec4(aPos, 1.0); 

        // Trasformo il vertice in coordinate di vista, lavoro su aPos
        vec4 eyePos = View * Model * vec4(aPos, 1.0);

        // Trasformo la posizione della luce in coordinate di vista
        vec4 eyeLightPos = View * vec4(light.position, 1.0);

        // Trasformo le normali del vertice in esame nel sistema di coordinate di vista
        mat3 G = mat3(transpose(inverse(View * Model)));
        N = normalize(G * vertexNormal);

        // Calcoliamo la direzione della luce, la direzione di riflessione R
        // Ci serve la differenza tra il punto su cui si va a vedere l'influenza
        // della luce e la telecamera
        L = normalize((eyeLightPos - eyePos).xyz);

        //Calcoliamo la direzione di vista: tra la telecamera e il vertice da calcolare
        V = normalize(viewPos - (eyePos).xyz);

        // Calcoliamo la direzione riflessa di L rispetto alla normale
        R = reflect(-L, N);

        // Componente ambientale:
        vec3 ambient = light.power * material.ambient;

        // Componente diffusiva: coseno dell'angolo tra la direzione della luce e la normale
        float cos_ang_theta = max(dot(L, N), 0);
        vec3 diffuse = light.power * light.color * cos_ang_theta * material.diffuse;

        // Componente speculare:
        float cos_ang_alpha = pow(max(dot(V, R), 0), material.shininess);
        vec3 specular = light.power * light.color * cos_ang_alpha * material.specular;

        // Il colore infine
        ourColor = vec4(ambient + diffuse + specular, 1.0);
       
    }
}  

