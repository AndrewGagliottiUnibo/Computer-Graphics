#version 420 core

// Interpolated values from the vertex shaders

in vec4 ourColor;
out vec4 FragColor;

uniform vec2 resolution;
uniform float time;
vec3 color;

void main(){

	// Normalizzazione del frammento nelle coordinate del device
	// Dei frammenti da colorare, passiamo alle coordinate normalizzate per modificare i colori in base a come vogliamo noi
    // ndc = normalized device coordinates
	vec2 ndc = vec2((gl_FragCoord.x / resolution.x - 0.5) * 2, (gl_FragCoord.y / resolution.y - 0.5) * 2);
	
	// Time indica come il colore varia nel tempo
	if (ndc.y > -0.9 && ndc.y < -0.8) {
		color = vec3(0, abs(sin(ndc.y * 10.5 + time)), abs(sin(ndc.y * time)));
		FragColor = vec4(color, 1.0);
	} else if (distance(ndc.xy, vec2(-0.7, -0.7)) < 0.2) {

		// Se i colori appartengono ad un determinato cerchio allora li consideriamo in modo diverso
		// distance() è una funzione di GLSL che calcola la distanza tra due elementi.
		FragColor = vec4(0.0, 0.0, abs(sin(ndc.y * 10.5 + time)), 1.0);
	} else {
		// Viene assegnato ad ogni frammento il colore interpolato
		FragColor = ourColor;
	}
}