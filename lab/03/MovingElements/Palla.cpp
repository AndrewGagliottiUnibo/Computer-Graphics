//
#include <iostream>
#include "ShaderMaker.h"
#include "Lib.h"
#include "GestioneEventi.h"
#define  PI   3.14159265358979323846

static unsigned int programId, programId1;

mat4 Projection;
GLuint MatProj, MatModel, loctime, locres;
int nv_P;

int width = 800;
int height = 800;

// parametri della palla
float distacco_da_terra = 30; // distacco da terra 
double delta = 1.5; // forza di gravita
double 	dy = 20; //velocita verticale (pixel per frame)
double dx = 0; //velocita orizzontale (pixel per frame)

double accelerazione = 1; // forza di accelerazione data dalla tastiera
double decelerazione = 1; //decelerazione in assenza di input
float posx = float(width) / 2; //coordinate sul piano della posizione iniziale della palla
float  posy = float(height) * 0.2;

//Booleani posti a true se si usa il tasto a (spostamento a sinistra) e b (spostamento a destra)
bool pressing_left = false;
bool pressing_right = false;

typedef struct {
	GLuint VAO;
	GLuint VBO_G;
	GLuint VBO_C;
	int nTriangles;
	// Vertici
	vector<vec3> vertici;
	vector<vec4> colors;
	// Numero vertici
	int nv;
	//Matrice di Modellazione: Traslazione*Rotazione*Scala
	mat4 Model;
} Figura;

Figura Montagna = {};
Figura Cielo= {};
Figura Prato = {};
Figura Pala_Eolica= {};
Figura Sole = {};
Figura Palla = {};

vector<Figura> Scena;

float angolo = 0.0;

void crea_VAO_Vector(Figura* fig) {

	glGenVertexArrays(1, &fig->VAO);
	glBindVertexArray(fig->VAO);
	//Genero , rendo attivo, riempio il VBO della geometria dei vertici
	glGenBuffers(1, &fig->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, fig->vertici.size() * sizeof(vec3),fig->vertici.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//Genero , rendo attivo, riempio il VBO dei colori
	glGenBuffers(1, &fig->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, fig->colors.size() * sizeof(vec4), fig->colors.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

}

void costruisci_pala_eolica(vec4 color_top, vec4 color_bot, Figura* Pala_Eolica)
{
int i;
float stepA = (2 * PI) /( Pala_Eolica->nTriangles);
float t;

 
Pala_Eolica->vertici.push_back(vec3(0.0,0.0, 0.0));
Pala_Eolica->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));

for (i = 0; i <= Pala_Eolica->nTriangles; i+=2)
{
	t = (float)i * stepA;
	
	Pala_Eolica->vertici.push_back(vec3(cos(t), sin(t), 0.0));
	Pala_Eolica->colors.push_back(vec4(color_bot.r, color_bot.g, color_bot.b, color_bot.a));

	t = (float)(i+1) * stepA;

	Pala_Eolica->vertici.push_back(vec3(cos(t), sin(t), 0.0));
	Pala_Eolica->colors.push_back(vec4(color_bot.r, color_bot.g, color_bot.b, color_bot.a));
	
	Pala_Eolica->vertici.push_back(vec3(0.0, 0.0, 0.0));
	Pala_Eolica->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
}
nv_P = Pala_Eolica->vertici.size();

//Sostegno Pala Eolica
Pala_Eolica->vertici.push_back(vec3(-0.1, -1.0, 0.0));
Pala_Eolica->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
Pala_Eolica->vertici.push_back(vec3(0.9, -1.0, 0.0));
Pala_Eolica->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
Pala_Eolica->vertici.push_back(vec3(-0.1, 0.0, 0.0));
Pala_Eolica->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
Pala_Eolica->vertici.push_back(vec3(0.9, 0.0, 0.0));
Pala_Eolica->colors.push_back(vec4(color_bot.r, color_bot.g, color_bot.b, color_bot.a));
Pala_Eolica->nv = Pala_Eolica->vertici.size();

}

void costruisci_palla(vec4 color_top, vec4 color_bot, vec4 color_top_ombra, vec4 color_bot_ombra, Figura* palla)
{
	int i;
	float stepA = (2 * PI) / palla->nTriangles;
	float t;


	palla->vertici.push_back(vec3(0.0, 0.0, 0.0));
	palla->colors.push_back(vec4(color_bot.r, color_bot.g, color_bot.b, color_bot.a));

	for (i = 0; i <= palla->nTriangles; i++)
	{
		t = (float)i * stepA;

		palla->vertici.push_back(vec3(cos(t), sin(t), 0.0));
		//Colore 
		palla->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	}


	//Costruzione ombra 
	palla->vertici.push_back(vec3(0.0, 0.0, 0.0));
	palla->colors.push_back(vec4(color_bot_ombra.r, color_bot_ombra.g, color_bot_ombra.b, color_bot_ombra.a));

	for (i = 0; i <= palla->nTriangles; i++)
	{
		t = (float)i * stepA;

		palla->vertici.push_back(vec3(cos(t), sin(t), 0.0));
		//Colore 
		palla->colors.push_back(vec4(color_top_ombra.r, color_top_ombra.g, color_top_ombra.b, color_top_ombra.a));
	}
	palla->nv = palla->vertici.size();

}
void costruisci_Sole(vec4 color_top, vec4 color_bot, vec4 color_top_alone, vec4 color_bot_alone, Figura* Sole) {
	
	int i;
	float stepA = (2 * PI) / Sole->nTriangles;
	float t;


	Sole->vertici.push_back(vec3(0.0, 0.0, 0.0));
	Sole->colors.push_back(vec4(color_bot.r,color_bot.g,color_bot.b,color_bot.a));

	for (i = 0; i <= Sole->nTriangles; i++)
	{
		t = (float)i * stepA;
		
		Sole->vertici.push_back(vec3(cos(t), sin(t),0.0));
		//Colore 
		Sole->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
 	}

	//Costruzione Alone
	Sole->vertici.push_back(vec3(0.0, 0.0, 0.0));
	Sole->colors.push_back(vec4(color_bot_alone.r, color_bot_alone.g, color_bot_alone.b, color_bot_alone.a));
	for (i = 0; i <= Sole->nTriangles; i++)
	{
		t = (float)i * stepA;

		Sole->vertici.push_back(vec3(2.0*cos(t), 2.0*sin(t), 0.0));
		//Colore 
		 
		Sole->colors.push_back(vec4(color_top_alone.r, color_top_alone.g, color_top_alone.b, color_top_alone.a));

	}
	 

	//Costruzione matrice di Moellazione Sole, che rimane la stessa(non si aggiorna durante l'animazione)
	Sole->nv = Sole->vertici.size();
	Sole->Model = mat4(1.0);
	Sole->Model = translate(Sole->Model, vec3(float(width) * 0.5, float(height) * 0.8, 0.0));
	Sole->Model = scale(Sole->Model, vec3(30.0, 30.0, 1.0));
	
}

void costruisci_montagne(int numero_di_montagne, vec4 color_top, vec4 color_bot, Figura* v_montagna)
{
	int i = 0;

	float step = 1 / (float)v_montagna->nTriangles;
	float frequenza = PI * numero_di_montagne;
	float t;
	int n_vertici = 0;

	for (i = 0; i < v_montagna->nTriangles; i += 1)
	{
		t = i / (float)v_montagna->nTriangles * frequenza;
		v_montagna->vertici.push_back(vec3(i * step,0,0));
		v_montagna->colors.push_back(vec4(color_bot.r,color_bot.g,color_bot.b,color_bot.a));
		v_montagna->vertici.push_back(vec3(i * step, abs(sin(t)),0.0));
		
		v_montagna->colors.push_back(vec4(color_top.r,color_top.g, color_top.b,color_top.a));

		  
	}
	v_montagna->nv = v_montagna->vertici.size();


	//Costruzione matrice di Modellazione Montagne, che rimane la stessa(non si aggiorna)
	v_montagna->Model = mat4(1.0);
	v_montagna->Model = translate(v_montagna->Model, vec3(0.0, float(height) / 2, 0.0));
	v_montagna->Model = scale(v_montagna->Model, vec3(float(width), float(height) / 8, 1.0));
}

void costruisci_cielo(vec4 color_top, vec4 color_bot, Figura* cielo)
{


	cielo->vertici.push_back(vec3(0.0, 0.0,0.0));
	cielo->colors.push_back(vec4(color_bot.r, color_bot.g, color_bot.b, color_top.a));
	cielo->vertici.push_back(vec3(1.0, 0.0, 0.0));
	cielo->colors.push_back(vec4(color_bot.r, color_bot.g, color_bot.b, color_bot.a));
	cielo->vertici.push_back(vec3(0.0, 1.0, 0.0));
	cielo->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	cielo->vertici.push_back(vec3(1.0, 1.0, 0.0));
	cielo->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));

	cielo->nv = cielo->vertici.size();
	 

	//Costruzione matrice di Modellazione Sole, che rimane la stessa(non si aggiorna)
	cielo->Model = mat4(1.0);
	cielo->Model = translate(cielo->Model, vec3(0.0, float(height) / 2, 0.0));
	cielo->Model = scale(cielo->Model, vec3(float(width), float(height) / 2, 1.0));
}void costruisci_prato(vec4 color_top, vec4 color_bot, Figura* prato)
{


	prato->vertici.push_back(vec3(0.0, 0.0, 0.0));
	prato->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	prato->vertici.push_back(vec3(0.0, 1.0, 0.0));
	prato->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	prato->vertici.push_back(vec3(1.0, 0.0, 0.0));
	prato->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	prato->vertici.push_back(vec3(1.0, 1.0, 0.0));
	prato->colors.push_back(vec4(color_bot.r, color_bot.g, color_bot.b, color_bot.a));

	prato->nv = prato->vertici.size();

	//Costruzione matrice di Modellazione Prato, che rimane la stessa(non si aggiorna)
	prato->Model = mat4(1.0);
	prato->Model = scale(prato->Model, vec3(float(width), float(height) / 2, 1.0));
}


double lerp(double a, double b, double amount) {

	//Interpolazione lineare tra a e b secondo amount

	return (1 - amount) * a + amount * b;
}

void INIT_SHADER(void)
{
	GLenum ErrorCheckValue = glGetError();

	
	char* vertexShader = (char*)"vertexShader_M.glsl";
	char*  fragmentShader = (char*)"fragmentShader_M_new.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	
}

void INIT_VAO(void)
{
	vec4 col_top = { 0.6471, 0.3020,1.0,1.0 };
	vec4 col_bottom = { 0.0, 0.4980,1.0,1.0 };
	costruisci_cielo(col_top, col_bottom, &Cielo);
	crea_VAO_Vector(&Cielo);

	Scena.push_back(Cielo);


	col_top = vec4{ 0.1333, 0.5451, 0.1333, 1.0000 };
	col_bottom = vec4{ 0.6784, 1.0,0.1843, 1.0000 };
	costruisci_prato(col_top, col_bottom, &Prato);
	crea_VAO_Vector(&Prato);
	Scena.push_back(Prato);

	 Montagna.nTriangles = 120;
	 col_bottom = vec4{ 0.5451, 0.2706, 0.0745, 1.0000 };
	 col_top = vec4{ 1.0,0.4980, 0.0353,1.0000 };
	 costruisci_montagne(6, col_top, col_bottom, &Montagna);
	 crea_VAO_Vector(&Montagna);
	 Scena.push_back(Montagna);
	 Sole.nTriangles = 30;
	 col_top = vec4{ 1.0, 0.9, 0.9, 0.8 };
	 col_bottom = vec4{ 1.0,0.8627,0.0, 1.0000 };
	 vec4 col_top_alone = { 1.0, 1.0, 1.0, 0.0 };
	 vec4 col_bottom_alone = { 1.0,0.8627,0.0, 1.0000 };
	 costruisci_Sole(col_top, col_bottom, col_top_alone, col_bottom_alone, &Sole);
	 crea_VAO_Vector(&Sole);
	 Scena.push_back(Sole);

	 
	 Pala_Eolica.nTriangles = 8;
	 col_top = { 0.0,1.0,0.0,1.0 };
	 col_bottom = { 0.5, 0.9, 0.05, 0.8 };
	 costruisci_pala_eolica(col_top, col_bottom, &Pala_Eolica);
	 crea_VAO_Vector(&Pala_Eolica);
	 Scena.push_back(Pala_Eolica);

	
	  Palla.nTriangles = 30;
	  col_top = { 1.0,0.0,0.0,1.0 };
	  col_bottom = { 1.0, 0.8, 0.0, 1.0 };
	  vec4 col_top_ombra = { 0.49,0.49,0.49,0.0 };
	  vec4 col_bottom_ombra = { 0.0, 0.0, 0.0, 0.6 };
	 costruisci_palla(col_top, col_bottom, col_top_ombra, col_bottom_ombra, &Palla);
	 crea_VAO_Vector(&Palla);
	 Scena.push_back(Palla);

	 //Costruzione della matrice di Proiezione
	 Projection = ortho(0.0f, float(width), 0.0f, float(height));
	 MatProj = glGetUniformLocation(programId, "Projection");
	 MatModel = glGetUniformLocation(programId, "Model");
	 loctime = glGetUniformLocation(programId, "time");
	 locres = glGetUniformLocation(programId, "resolution");
	
	 
	
	


}
void drawScene(void)
{

	int k;
	glClearColor(0.0,0.0,0.0, 0.5);
	glClear(GL_COLOR_BUFFER_BIT);
	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection));
 
	//Disegno Cielo
	
	glUseProgram(programId);
	float time = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
	
	vec2 resolution = vec2((float)width, (float)height);
	 
	glUniform1f(loctime, time);
	glUniform2f(locres, resolution.x,resolution.y);
	for (k=0;k<4;k++)
	{
		
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[k].Model));
		glBindVertexArray(Scena[k].VAO);
		 
		if (k<3)
			glDrawArrays(GL_TRIANGLE_STRIP, 0, Scena[k].nv);
		else
			glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[k].nv);
		glBindVertexArray(0);
	}
	
 
 

	//Disegno 6 PALE EOLICHE
	glBindVertexArray(Scena[4].VAO);
	for (int i = 1; i < 7; i++)
	{
		//Disegno il sostegno
		//Definisco la matrice di Trasformazione per il sostegno che rimane fisso durante l'animazione
		 
		Scena[4].Model = mat4(1.0);
		Scena[4].Model = translate(Scena[4].Model, vec3(float(width) * 0.15 * i, float(height) * 0.65, 0.0));
		Scena[4].Model = scale(Scena[4].Model, vec3(3.0, 100.0, 1.0));
	 
		
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[4].Model));


		glDrawArrays(GL_TRIANGLE_STRIP, Scena[4].nv - 4, 4);
		 
		//Disegna la Pala
		 //Definisco la matrice di trasformazione per la Pala che ruota
		Scena[4].Model = mat4(1.0);
		Scena[4].Model = translate(Scena[4].Model, vec3(float(width) * 0.15 * i, float(height) * 0.65, 0.0));
		Scena[4].Model = scale(Scena[4].Model, vec3(40.0, 40.0, 1.0));
		Scena[4].Model = rotate(Scena[4].Model, radians(angolo), vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[4].Model));
			   
		glDrawArrays(GL_TRIANGLES, 0, nv_P);
		
	}

	float distacco_da_terra_n = -distacco_da_terra;
	float shadow_scale = lerp(1, 0, (float)distacco_da_terra_n / 255);
	if (shadow_scale < 0)
		shadow_scale = 0;
	float bwidth = distacco_da_terra_n < 0 ? lerp(80, 100, (float)abs(distacco_da_terra_n) / 20) : 80; // larghezza effettiva in pixel della palla
	float bheight = distacco_da_terra_n < 0 ? 80 + distacco_da_terra_n : 80; // altezza effettiva in pixel della palla


	glBindVertexArray(0);
 
	//Palla ed Ombra
	glBindVertexArray(Scena[5].VAO);
	//matrice di Trasformazione della Palla
	Scena[5].Model = mat4(1.0);
	Scena[5].Model = translate(Scena[5].Model, vec3(posx - bwidth / 2, posy + bheight + distacco_da_terra_n, 0.0f));
	Scena[5].Model = scale(Scena[5].Model, vec3(float(bwidth) / 2, float(bheight) / 2, 1.0));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[5].Model));
	//Disegna Corpo della palla  
	 glDrawArrays(GL_TRIANGLE_FAN, 0, (Scena[5].nTriangles) + 2);
	
	 //matrice di Trasformazione della Palla
	 Scena[5].Model = mat4(1.0);
	 Scena[5].Model = translate(Scena[5].Model, vec3(posx - bwidth / 2 * shadow_scale, posy + 10 + 10 * (1 - shadow_scale), 0.0f));
	 Scena[5].Model = scale(Scena[5].Model, vec3(float(bwidth) * shadow_scale, (50.0 * shadow_scale), 1.0));
	 glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[5].Model));
	 //Disegna Ombra
	 glDrawArrays(GL_TRIANGLE_FAN, (Scena[5].nTriangles) + 2, (Scena[5].nTriangles) + 2);
	
	glBindVertexArray(0);
	glutSwapBuffers();



}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Scena OpenGL");
	glutDisplayFunc(drawScene);

	glutKeyboardFunc(keyboardPressedEvent);

	//Evento tastiera tasto rilasciato
	glutKeyboardUpFunc(keyboardReleasedEvent);


	//gestione animazione
	glutTimerFunc(66, update, 0);
	glutTimerFunc(66, update_pala, 0);

	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glutMainLoop();
}
