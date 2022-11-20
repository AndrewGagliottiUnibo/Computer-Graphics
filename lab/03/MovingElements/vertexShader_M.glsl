#version 330 core

layout (location = 0) in vec3 aPos;   
layout (location = 1) in vec4 aColor; 

// Creo le variabili uniformi per le coordinate di vista.
uniform mat4 Model;
uniform mat4 Projection;
out vec4 ourColor;

void main() {	
	
	// La variabile predefinita deve contenere le coordinate normalizzate.
	// View manca perche' la vista e' ortogonale e quindi manca il concetto di profondita'.
	gl_Position = Projection * Model * vec4(aPos.x,aPos.y,aPos.z,1.0);
	ourColor=aColor;
}

