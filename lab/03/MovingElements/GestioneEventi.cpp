#include "Lib.h"
extern bool pressing_left, pressing_right, moving;
extern double 	dx;
extern double 	dy; //velocita verticale (pixel per frame)
extern double velocitaMassima; // velocita di movimento orizzontale massima
extern double delta;
extern float distacco_da_terra; //negativo, perchè lo misuro nel sistema di riferimento locale della palla
extern double accelerazione ; // forza di accelerazione data dalla tastiera
extern double decelerazione; //decelerazione in assenza di input
extern float posx ; //coordinate sul piano della posizione iniziale della palla
extern float  posy;
extern int  width;
extern float angolo;

void keyboardPressedEvent(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'a':
		pressing_left = true;
		break;

	case 'd':
		pressing_right = true;
		break;

	case 27:
		exit(0);
		break;

	default:
		break;
	}
}

void keyboardReleasedEvent(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'a':
		pressing_left = false;
		break;

	case 'd':
		pressing_right = false;
		break;

	default:
		break;
	}
}

void update(int a) {
	bool moving = false;
	//Movimento della palla in orizzontale

	if (pressing_left)
	{
		dx -= accelerazione;
		moving = true;
	}

	if (pressing_right)
	{
		dx += accelerazione;
		moving = true;
	}



	if (!moving) {   //Se non mi sto muovendo con i tasti a e d decremento od incremento la velocita' iniziale fino a portarla
					 // a zero e la palla continua a rimbalzare sul posto

		if (dx > 0)
		{
			dx -= 1;
			if (dx < 0)
				dx = 0;
		}

		if (dx < 0)
		{
			dx += 1;
			if (dx > 0)
				dx = 0;
		}
	}

	//Aggioramento della posizione in x della pallina, che regola il movimento orizzontale
	posx += dx;

	//Se la pallina assume una posizione in x minore di 0 o maggiore di width dello schermo
	//facciamo rimbalzare la pallina ai bordi dello schermo
	if (posx < 0) {
		posx = 0;
		dx = -dx * 0.8;
	}

	if (posx > width) {
		posx = width;
		dx = -dx * 0.8;
	}

	// Gestione del rimbalzo e quindi dell'altezza da terra
	//Rimbalzo
	dy -= delta;
	distacco_da_terra -= dy;
	printf("dy %f  Distacca da terra %f \n", dy, distacco_da_terra);
	 
   if (distacco_da_terra > 30)
   {
	   distacco_da_terra = 30;
	   dy = 30;   //Una volta giunta a terra la pallina ottiene un impulso positivo che la ritornare su
   }
	
	glutPostRedisplay();
	glutTimerFunc(24, update, 0);
}


void update_pala(int a) {
	angolo+=5;
	glutPostRedisplay();
	glutTimerFunc(24, update_pala, 0);
}