#version 420 core

// Interpolated values from the vertex shaders
in vec4 color_frag;
out vec4 color;

void main(){
	// Viene assegnato lo stesso colore ad ogni pixel
	// La pipeline da adesso in poi gestira l'interpolazione dei vertici
	// andando a colorare i vertici in modo che siano fra loro sfumati
	color = color_frag;
}