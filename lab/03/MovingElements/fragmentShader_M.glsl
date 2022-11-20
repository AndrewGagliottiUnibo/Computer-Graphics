#version 420 core

in vec4 ourColor;
out vec4 FragColor;  

void main(){
	// Viene assegnato ad ogni frammento il colore interpolato
	// Rimane invariato.
	FragColor=ourColor;
}