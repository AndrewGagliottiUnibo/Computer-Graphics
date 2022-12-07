#version 330 core
layout (location = 0) in vec3 aPos;   // Attributo Posizione


 uniform mat4 Projection;
 uniform mat4 View;
 out vec3 TexCoords;

void main(){	

    // Arrivate le coordinate di texture, cioè le posizioni del cubo e si sfruttano gli shader per
    // rappresentarlo. Per escludere la traslazione basta considerare una matrice 3 * 3 creando quindi
    // un effetto statico e non relativo anche allo spostamento della camera nello spazio.
    TexCoords = aPos;//vec3(aPos.x,-aPos.yz) ;  // in caso le coordinate vengono flipped
    mat4 Static_View=mat4(mat3(View));  //MAtrice di vista a cui si toglie il contributo della traslazione
    gl_Position = Projection * Static_View * vec4(aPos, 1.0);
}


