#include <iostream>
#include "ShaderMaker.h"
#include "Lib.h"
#define  PI   3.14159265358979323846

//Per Curve di hermite
#define PHI0(t)  (2.0*t*t*t-3.0*t*t+1)
#define PHI1(t)  (t*t*t-2.0*t*t+t)
#define PSI0(t)  (-2.0*t*t*t+3.0*t*t)
#define PSI1(t)  (t*t*t-t*t)

// viewport size
int width = 1280;
int height = 720;
unsigned int selected_obj = 0;
float posx_Proiettile, posy_Proiettile;
static unsigned int programId;
mat4 Projection;
GLuint MatProj, MatModel;
unsigned int lsceltavs, lsceltafs, loc_time, loc_res, loc_mouse;
vec2 res, mouse;
int posx = (float)width / 2.0, posy = (float)height / 4.0;
int pval = 140;
float angolo = 0;

typedef struct {
	GLuint VAO;
	GLuint VBO_G;
	GLuint VBO_C;
	GLuint EBO_indici;
	int nTriangles;
	// Vertici
	vector<vec3> vertici;
	vector<vec3> CP;
	vector<vec4> colors;
	vector<int> indici;
	// Numero vertici
	int nv;
	//Matrice di Modellazione: Traslazione*Rotazione*Scala
	mat4 Model;
	int sceltaVS;
	int sceltaFS;
	string name;
} Figura;


Figura Navicella, Prato, Cielo, Stella;

static vector<Figura> Scena;
void updateProiettile(int value) {

	//Ascissa del proiettile durante lo sparo
	posx_Proiettile = 0;
	//Ordinata del proettile durante lo sparo
	posy_Proiettile++;

	//L'animazione deve avvenire finchè  l'ordinata del proiettile raggiunge un certo valore fissato
	if (posy_Proiettile <= 400)
		glutTimerFunc(5, updateProiettile, 0);
	else
		posy_Proiettile = 0;

	//printf("posx posy %f %f \n", posx_Proiettile, posy_Proiettile);
	glutPostRedisplay();
}

void crea_VAO_Vector(Figura* fig) {

	glGenVertexArrays(1, &fig->VAO);
	glBindVertexArray(fig->VAO);
	//Genero , rendo attivo, riempio il VBO della geometria dei vertici
	glGenBuffers(1, &fig->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, fig->vertici.size() * sizeof(vec3), fig->vertici.data(), GL_STATIC_DRAW);
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


/// /////////////////////////////////// Disegna geometria //////////////////////////////////////
float dx(int i, float* t, float Tens, float Bias, float Cont, Figura* Fig) {
	if (i == 0)
		return  0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP[i + 1].x - Fig->CP[i].x) / (t[i + 1] - t[i]);
	if (i == Fig->CP.size() - 1)
		return  0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP[i].x - Fig->CP[i - 1].x) / (t[i] - t[i - 1]);

	if (i % 2 == 0)
		return  0.5 * (1 - Tens) * (1 + Bias) * (1 + Cont) * (Fig->CP.at(i).x - Fig->CP.at(i - 1).x) / (t[i] - t[i - 1]) + 0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP.at(i + 1).x - Fig->CP.at(i).x) / (t[i + 1] - t[i]);
	else
		return  0.5 * (1 - Tens) * (1 + Bias) * (1 - Cont) * (Fig->CP.at(i).x - Fig->CP.at(i - 1).x) / (t[i] - t[i - 1]) + 0.5 * (1 - Tens) * (1 - Bias) * (1 + Cont) * (Fig->CP.at(i + 1).x - Fig->CP.at(i).x) / (t[i + 1] - t[i]);
}

float dy(int i, float* t, float Tens, float Bias, float Cont, Figura* Fig) {
	if (i == 0)
		return 0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP.at(i + 1).y - Fig->CP.at(i).y) / (t[i + 1] - t[i]);
	if (i == Fig->CP.size() - 1)
		return  0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP.at(i).y - Fig->CP.at(i - 1).y) / (t[i] - t[i - 1]);

	if (i % 2 == 0)
		return  0.5 * (1 - Tens) * (1 + Bias) * (1 + Cont) * (Fig->CP.at(i).y - Fig->CP.at(i - 1).y) / (t[i] - t[i - 1]) + 0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP.at(i + 1).y - Fig->CP.at(i).y) / (t[i + 1] - t[i]);
	else
		return  0.5 * (1 - Tens) * (1 + Bias) * (1 - Cont) * (Fig->CP.at(i).y - Fig->CP.at(i - 1).y) / (t[i] - t[i - 1]) + 0.5 * (1 - Tens) * (1 - Bias) * (1 + Cont) * (Fig->CP.at(i + 1).y - Fig->CP.at(i).y) / (t[i + 1] - t[i]);
}

void InterpolazioneHermite(float* t, Figura* Fig, vec4 color_top, vec4 color_bot) {
	float p_t = 0, p_b = 0, p_c = 0, x, y;
	float passotg = 1.0 / (float)(pval - 1);
	float passotg0 = 1.0 / 10.0;
	float tg = 0, tgmapp, ampiezza;
	int i = 0;
	int is = 0; //indice dell'estremo sinistro dell'intervallo [t(i),t(i+1)] a cui il punto tg appartiene

	Fig->vertici.push_back(vec3(-1.0, 5.0, 0.0));
	Fig->colors.push_back(vec4(1.0, 1.0, 0.0, 1.0));

	for (tg = 0; tg <= 1; tg += passotg) {
		if (tg > t[is + 1]) 
			is++;

		ampiezza = (t[is + 1] - t[is]);
		tgmapp = (tg - t[is]) / ampiezza;

		x = Fig->CP[is].x * PHI0(tgmapp) + dx(is, t, p_t, p_b, p_c, Fig) * PHI1(tgmapp) * ampiezza + Fig->CP[is + 1].x * PSI0(tgmapp) + dx(is + 1, t, p_t, p_b, p_c, Fig) * PSI1(tgmapp) * ampiezza;
		y = Fig->CP[is].y * PHI0(tgmapp) + dy(is, t, p_t, p_b, p_c, Fig) * PHI1(tgmapp) * ampiezza + Fig->CP[is + 1].y * PSI0(tgmapp) + dy(is + 1, t, p_t, p_b, p_c, Fig) * PSI1(tgmapp) * ampiezza;
		Fig->vertici.push_back(vec3(x, y, 0.0));
		Fig->colors.push_back(color_top);
	}
}

void costruisci_stella(Figura* fig) {

	int i;
	float stepA = (2 * PI) / fig->nTriangles;
	float t;

	fig->vertici.push_back(vec3(0.0, 0.0, 0.0));
	fig->colors.push_back(vec4(1.0, 0.8627, 0.0, 1.0000));

	for (i = 0; i <= fig->nTriangles; i++) {
		t = (float)i * stepA;

		if (i % 2 == 0)
			fig->vertici.push_back(vec3(cos(t), sin(t), 0.0));
		else
			fig->vertici.push_back(vec3(0.5 * cos(t), 0.5 * sin(t), 0.0));
		
		//Colore 
		fig->colors.push_back(vec4(1.0, 0.9, 0.9, 0.8));
	}

	fig->nv = fig->vertici.size();
}

 
void onMouse(int button, int state, int x, int y) {
	if (state != GLUT_DOWN)
		return;

	glReadPixels(x, height - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &selected_obj);

	selected_obj = selected_obj - 1;
	printf("Oggetto selezionato %d --> %s  \n", selected_obj,Scena[selected_obj].name.c_str());
}
 

void costruisci_cielo(vec4 color_top, vec4 color_bot, Figura* cielo) {

	cielo->vertici.push_back(vec3(0.0, 0.0, 0.0));
	cielo->colors.push_back(vec4(color_bot.r, color_bot.g, color_bot.b, color_top.a));
	cielo->vertici.push_back(vec3(1.0, 0.0, 0.0));
	cielo->colors.push_back(vec4(color_bot.r, color_bot.g, color_bot.b, color_bot.a));
	cielo->vertici.push_back(vec3(0.0, 1.0, 0.0));
	cielo->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	cielo->vertici.push_back(vec3(1.0, 1.0, 0.0));
	cielo->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));

	cielo->nv = cielo->vertici.size();

	//for (int i=0;i<4;i++)
	//	printf("Vertici piano %d-> %f, %f, %f \n", i, cielo->vertici[i].x, cielo->vertici[i].y, cielo->vertici[i].z);
	cielo->Model = mat4(1.0);
	cielo->Model = translate(cielo->Model, vec3(0.0, float(height) / 2, 0.0));
	cielo->Model = scale(cielo->Model, vec3(float(width), float(height) / 2, 1.0));
}

void costruisci_prato(vec4 color_top, vec4 color_bot, Figura* prato) {

	prato->vertici.push_back(vec3(0.0, 0.0, 0.0));
	prato->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	prato->vertici.push_back(vec3(0.0, 1.0, 0.0));
	prato->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	prato->vertici.push_back(vec3(1.0, 0.0, 0.0));
	prato->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	prato->vertici.push_back(vec3(1.0, 1.0, 0.0));
	prato->colors.push_back(vec4(color_bot.r, color_bot.g, color_bot.b, color_bot.a));

	prato->nv = prato->vertici.size();

	//for (int i = 0; i < 4; i++)
		//printf("Vertici piano %d-> %f, %f, %f \n", i, prato->vertici[i].x, prato->vertici[i].y, prato->vertici[i].z);
	prato->Model = mat4(1.0);
	prato->Model = scale(prato->Model, vec3(float(width), float(height) / 2, 1.0));
}


void costruisci_forma(vec4 color_top, vec4 color_bot, Figura* forma) {
	// Come si definisce una forma usando le curve di hermitte
	float* t;

	forma->CP.push_back(vec3(0.0, 0.0, 0.0));
	forma->CP.push_back(vec3(-2.0, 0.0, 0.0));
	forma->CP.push_back(vec3(-2.0, 3.0, 0.0));
	forma->CP.push_back(vec3(-2.0, 5.0, 0.0));
	forma->CP.push_back(vec3(-6.0, 6.0, 0.0));
	forma->CP.push_back(vec3(-5.0, 8.0, 0.0));
	forma->CP.push_back(vec3(0.0, 14.0, 0.0));
	forma->CP.push_back(vec3(5.0, 8.0, 0.0));
	forma->CP.push_back(vec3(6.0, 6.0, 0.0));
	forma->CP.push_back(vec3(2.0, 5.0, 0.0));
	forma->CP.push_back(vec3(2.0, 3.0, 0.0));
	forma->CP.push_back(vec3(2.0, 0.0, 0.0));
	forma->CP.push_back(vec3(0.0, 0.0, 0.0));

	t = new float[forma->CP.size()];
	int i;
	float step = 1.0 / (float)(forma->CP.size() - 1);

	for (i = 0; i < forma->CP.size(); i++)
		t[i] = i * step;

	InterpolazioneHermite(t, &Navicella, color_top, color_bot);
	forma->vertici.push_back(vec3(0.0, 0.0, 0.0));
	forma->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	forma->nv = forma->vertici.size();
}

void INIT_VAO(void) {

	Cielo.sceltaFS = 0;
	Cielo.sceltaVS = 0;
	Cielo.name = "Cielo";
	vec4 col_top = { 0.6471, 0.3020,1.0,1.0 };
	vec4 col_bottom = { 0.0, 0.4980,1.0,1.0 };
	costruisci_cielo(col_top, col_bottom, &Cielo);
	crea_VAO_Vector(&Cielo);

	Stella.nTriangles = 10;
	Stella.sceltaFS = 0;
	Stella.sceltaVS = 0;
	Stella.name = "Stella";
	costruisci_stella(&Stella);
	crea_VAO_Vector(&Stella);

	Prato.sceltaFS = 0;
	Prato.sceltaVS = 0;
	Prato.name = "Prato";
	col_top = vec4{ 0.1333, 0.5451, 0.1333, 1.0000 };
	col_bottom = vec4{ 0.6784, 1.0,0.1843, 1.0000 };
	costruisci_prato(col_top, col_bottom, &Prato);
	crea_VAO_Vector(&Prato);

	Navicella.sceltaFS = 0;
	Navicella.sceltaVS = 0;
	Navicella.name = "Navicella";
	col_bottom = vec4{ 0.5451, 0.2706, 0.0745, 1.0000 };
	col_top = vec4{ 1.0,0.4980, 0.0353,1.0000 };
	costruisci_forma(col_top, col_bottom, &Navicella);
	crea_VAO_Vector(&Navicella);

	Scena.push_back(Cielo);
	Scena.push_back(Prato);
	Scena.push_back(Stella);
	Scena.push_back(Navicella);

	Projection = ortho(0.0f, float(width), 0.0f, float(height));
	MatProj = glGetUniformLocation(programId, "Projection");
	MatModel = glGetUniformLocation(programId, "Model");

	lsceltavs = glGetUniformLocation(programId, "sceltaVS");
	lsceltafs = glGetUniformLocation(programId, "sceltaFS");

	loc_time = glGetUniformLocation(programId, "time");

	loc_res = glGetUniformLocation(programId, "res");

	loc_mouse = glGetUniformLocation(programId, "mouse");
}
 
void mouseClick(int x, int y) {
	mouse.x =(float) x;
	mouse.y =  (float) y;

	glutPostRedisplay();
}
 
void INIT_SHADER(void) {
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_M.glsl";
	char* fragmentShader = (char*)"fragmentShader_M.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);
}

void drawScene(void) {

	glViewport(0, 0, width, height);
	int i;
	res.x = (float)width;
	res.y = (float)height;

	float time = glutGet(GLUT_ELAPSED_TIME);
	glUniform1f(loc_time, time);
	//ci assicuriamo che il buffer dello stencil contenga solo zeri
	glClearStencil(0);		
	glClearColor(0.0, 1.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);// GL_STENCIL_BUFFER_BIT);

	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection));
	glUniform2f(loc_res, res.x, res.y);
	glUniform2f(loc_mouse, mouse.x, mouse.y);

	//Disegno gli oggetti di sfondo che hanno matrice di modellazione fissa
	for (i = 0; i < 2; i++)	{
		// Ad ogni indice gli assegno un valore di riconoscimento
		glStencilFunc(GL_ALWAYS, i+1, -1);
		glUniform1i(lsceltavs, Scena[i].sceltaVS);
		glUniform1i(lsceltafs, Scena[i].sceltaFS);
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[i].Model));
		glBindVertexArray(Scena[i].VAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, Scena[i].nv);
	}

	//glViewport(250, 250, 200, 200);
	//Definisco la matrice di Modellazione della navicella
	Scena[3].Model = mat4(1.0);
	Scena[3].Model = translate(Scena[3].Model, vec3(posx, posy, 0.0));
	Scena[3].Model = rotate(Scena[3].Model, radians(angolo), vec3(0.0, 0.0, 1.0));
	Scena[3].Model = scale(Scena[3].Model, vec3(20.0, 20.0, 1.0));
	//Definisco la matrice di Modellazione del proiettile
	Scena[2].Model = translate(Scena[3].Model, vec3(posx_Proiettile, 15 + posy_Proiettile, 0));
	
	//Disegno gli oggetti di sfondo che hanno matrice di modellazione che viene aggiornata durante l'esecuzione
	//Disegno prima la navicella e poi il proiettile
	for (i = 2; i <= 3; i++) {
		// Subito prima di disegnare l'oggetto stesso, impostiamo la funzione stencil per superare sempre il test stencil (GL_ALWAYS) e il valore di riferimento su i + 1
		//(perché 0 è già utilizzato per lo sfondo).Impostiamo la maschera per avere tutti i bit abilitati(si può usare  ~0 o 0xff invece di - 1 ).
		//Quando si rilegge dal buffer dello stencil, se si clicca sullo sfondo, si ha un valore di stencil uguale a zero, altrimenti si clicca su un oggetto.
		glStencilFunc(GL_ALWAYS, i+1 , -1);
		glUniform1i(lsceltavs, Scena[i].sceltaVS);
		glUniform1i(lsceltafs, Scena[i].sceltaFS);
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[i].Model));
		glBindVertexArray(Scena[i].VAO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[i].nv);
		glBindVertexArray(0);
	}

	glutSwapBuffers();
}

void resize(GLsizei w, GLsizei h) {

	float AspectRatio_mondo = (float)(width) / (float)(height);
	
	//Se l'aspect ratio del mondo è diversa da quella della finestra devo mappare in modo diverso per evitare distorsioni del disegno
	if (AspectRatio_mondo > w / h) {
		glViewport(0, 0, w, w / AspectRatio_mondo);
	}
	else {
		glViewport(0, 0, h * AspectRatio_mondo, h);
	}

	glutPostRedisplay();
}

void update(int par) {
	glutTimerFunc(66, update, 0);
	glutPostRedisplay();
}

void keyboardPressedEvent(unsigned char key, int x, int y) {
	switch (key) {

	case ' ':
		updateProiettile(0);
		break;
	case 'a':
		posx = posx + 1;
		break;
	case 'd':
		posx = posx - 1;
		break;
	case 'r':
		angolo = angolo - 1.0;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void shader_menu_function(int option) {
	Scena[selected_obj].sceltaFS = option;
}

void main_menu_func(int option) {
	switch (option)	{

	case 1: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 2: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	default:
		break;
	}
}

void buildOpenGLMenu() {
	int shaderSubMenu = glutCreateMenu(shader_menu_function);

	glutAddMenuEntry("Fragment Shader 0", 0);
	glutAddMenuEntry("Fragment Shader 1", 1);
	glutAddMenuEntry("Fragment Shader 2", 2);
	glutAddMenuEntry("Fragment Shader 3", 3);
	glutAddMenuEntry("Fragment Shader 4", 4);
	glutAddMenuEntry("Fragment Shader 5", 5);

	glutCreateMenu(main_menu_func); // richiama main_menu_func() alla selezione di una voce menu
	glutAddMenuEntry("Opzioni", -1); //-1 significa che non si vuole gestire questa riga

	glutAddMenuEntry("Wireframe", 1);
	glutAddMenuEntry("Face fill", 2);

	glutAddSubMenu("Fragment Shaders", shaderSubMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Fungo OpenGL");
	glutDisplayFunc(drawScene);
	//gestione animazione
	glutTimerFunc(66, update, 0);
	glutMouseFunc(onMouse);
	glutPassiveMotionFunc(mouseClick);
	glutTimerFunc(5, updateProiettile, 0);
	glutKeyboardFunc(keyboardPressedEvent);
	//glutReshapeFunc(resize);

	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
	buildOpenGLMenu();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Abilitamo il buffer dello stencil
	glEnable(GL_STENCIL_TEST);
	//Usiamo glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE) per assicurarci che il buffer dello stencil venga scritto ogni volta che viene scritto il buffer del colore  e sostituiremo il valore esistente con un nuovo valore fisso .
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glutMainLoop();
}