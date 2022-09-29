#version 330 core

// Si aspetta una variabile di input con le posizioni dei vertici.
// La variabile è un vettore di 3 punti denominato aPos.
layout(location = 0) in vec3 aPos;

void main() {	
	// Il quarto parametro serve per indicare che si sta rappresentando
	// un punto e non un vettore. Per rappresentare un vettore si usa 0.0.
	// Da qui si passa alla fase successiva ma in questo caso non si opera nulla:
	// le coordinate sono già a posto e normalizzate per poter passare alla fase di
	// rastering
	gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0);	
}

