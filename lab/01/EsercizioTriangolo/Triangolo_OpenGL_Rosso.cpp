#include <iostream>
#include "ShaderMaker.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

static unsigned int programId;
/*
	Le variabili globali di VAO e VBO.
*/
unsigned int VAO;
unsigned int VBO;


/*
	Vertici del triangolo nel sistema di coordinate normalizzate.
	0.0 indica che l'asse z non viene utilizzato. Si ricorda che 
	l'asse z è orientato con la direzione verso l'osservatore.
*/
float vertices[] = {
	-0.5f, -0.5f, 0.0f, // vertice in basso a sinistra
	0.5f, -0.5f, 0.0f,  // vertice in basso a destra
	0.0f,  0.5f, 0.0f	// vertice in alto 
};

/*
	Costruzione buffer della geometria: costruzione delle primitive.
*/
void INIT_VAO(void);

/*
	Codice di renderizzazione.
*/
void drawScene(void);

/*
	Inizializzazione contesto di esecuzione.
*/
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);

	/*
		Versione di OpenGL.
	*/
	glutInitContextVersion(4, 0);

	/*
		No fixed pipeline e nessuna delle versioni che sono ora deprecate.
	*/
	glutInitContextProfile(GLUT_CORE_PROFILE);

	/*
		Double buffer per la gestione delle animazioni.
						+
		I colori sono una quadrupla di valori: R + G + B + A.
		Sono necessari ulteriori trasformazioni però per la gestione della trasparenza.
	*/
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	/*
		Size della schermata e posizionamento.
	*/
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(400, 150);
	glutCreateWindow("Triangolo OpenGL");

	/*
		Gestione eventi: in questo caso è un semplice disegno.
		Il codice per la renderizzazione della geometria viene 
		richiamato qui.
	*/
	glutDisplayFunc(drawScene);

	/*
		Segui le specifiche della casa di produzione.
	*/
	glewExperimental = GL_TRUE;
	glewInit();
	
	/*
		Chiama la costruzione del buffer della geometria.
	*/
	INIT_VAO();

	/*
		Loop del programma: interrompibile solo manualmente e serve a refreshare 
		l'immagine che altrimenti non sarebbe mai consistente. In questo modo la
		drawScene disegna continuamente il modello a schermo.
	*/
	glutMainLoop();
}

void INIT_VAO(void) {
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader.glsl";
	char* fragmentShader = (char*)"fragmentShader.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);

	/*
		Genero un VAO con lo spazio per un solo VBO, questo perchè non ho
		voluto specificare dati sul colore.
	*/
	glGenVertexArrays(1, &VAO);
	
	/*
		Ne faccio il bind (lo collego, lo attivo).
	*/
	glBindVertexArray(VAO);

	/*
		Al suo interno genero un VBO dove inseriremo le informazioni sulle
		coordinate geometriche.
		Tutto ciò che viene inzializzato da qui in poi avviene all'interno
		della prima posizione del VAO.
	*/
	glGenBuffers(1, &VBO);

	/*
		Ne faccio il bind (lo collego, lo attivo, assegnandogli il tipo GL_ARRAY_BUFFER).
	*/
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	/*
		Carico i dati vertices sulla GPU con i seguenti campi:
		- tipo
		- dimensione
		- indirizzo
		- modalità di creazione delle immagini.

		Si usa la creazione statica perché non si va ad operare sul VAO una volta creato.
	*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/*
		Configurazione dell'attributo posizione: informo il vertex shader su: dove trova 
		le informazioni sulle posizioni e come le deve leggere dal buffer caricato sulla GPU.
		Gli argomenti sono:
		- Attributo 0.
		- Analizza 3 vertici.
		- Si usano i float.
		- I dati non devono essere normalizzati.
		- offset di lettura dei nuovi dati.
		- posizione di lettura di inizio dei vertici.
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/*
		Imposta il colore che verrà poi assegnato dalla glClear().
		Il buffer è infatti nero (sfondo schermata triangolo).
	*/
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void drawScene(void) {
	/*
		Inizializza con un colore il color frame buffer: il buffer potrebbe essere sporco
		e quindi vogliamo prevenire questa situazione che potrebbe causare problemi.
	*/
	glClear(GL_COLOR_BUFFER_BIT);

	/*
		Come si valutano i dati: in questo caso i vertici sono visti come
		appartenenti a dei triangoli. I vertici sono le coordinate della
		posizione all'interno dello spazio.
	*/
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// Serve per tante geometrie diverse
	// glBindVertexArray(VAO);

	/*
		Swap dei buffer per il rendering.
	*/
	glutSwapBuffers();
}


