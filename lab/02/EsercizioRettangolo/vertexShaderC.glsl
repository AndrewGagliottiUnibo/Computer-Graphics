#version 330 core

// Legge vertici e colori.
layout(location = 0) in vec3 aPos;
layout(location =1 ) in vec4 Color;

// Sputa fuori i colori per la prossima fase della pipeline.
out vec4 colore_frag;

void main() {	
	// Variabile che contiene le coordinate normalizzate, variabile predefinita.
	// Si preferisce usare le coordinate omogenee, quindi 4 coordinate per l'ambiguità tra punto e vettore.
	gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0);

	// Il colore che deve essere interpolato.
	colore_frag= Color;
}

