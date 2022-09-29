#version 330 core

// Si aspetta una variabile di input con le posizioni dei vertici.
// La variabile � un vettore di 3 punti denominato aPos.
layout(location = 0) in vec3 aPos;

// Per la sfumatura creo la gestione del colore.
// Lo devo dare in output alla fase di rasterizzazione per poter essere
// poi elaborato successivamente.
layout(location = 1) in vec4 aColor;
out vec4 color_frag;

void main() {	
	// Il quarto parametro serve per indicare che si sta rappresentando
	// un punto e non un vettore. Per rappresentare un vettore si usa 0.0.
	// Da qui si passa alla fase successiva ma in questo caso non si opera nulla:
	// le coordinate sono gi� a posto e normalizzate per poter passare alla fase di
	// rastering
	gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0);	

	// gestisco i colori
	color_frag = aColor;
}


