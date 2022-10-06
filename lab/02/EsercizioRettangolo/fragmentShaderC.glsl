#version 420 core

// Interpolated values from the vertex shaders
in vec4 colore_frag;
out vec4 color;

void main() {
//Viene assegnato lo stesso colore ad ogni pixel
	color = colore_frag;
}