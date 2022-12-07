#pragma once
#include "Lib.h"

extern float raggio_sfera;
extern int selected_obj;
extern int width, height;
extern int last_mouse_pos_X, last_mouse_pos_Y;
extern bool moving_trackball;
extern float lastX, lastY;
extern bool firstMouse;
extern mat4 View, Projection;
extern float Theta;
extern float Phi;

vec3 get_ray_from_mouse(float mouse_x, float mouse_y)
{
	mouse_y = height - mouse_y;

	// mappiamo le coordinate di viewport del mouse [0,width], [0,height] in coordinate normalizzate [-1,1]
	float x = (2.0f * mouse_x) / width - 1.0f;
	float y = (2.0f * mouse_y) / height - 1.0f;
	float z = 1.0f;

	vec3 ray_nds = vec3(x, y, z);
	// Nel clip space hanno coordinate: nota bene la terza coordinata uguale a -1
	vec4 ray_clip = vec4(x, y, -1.0, 1.0);

	// Le coordinate nell' eye space si ottengono premoltiplicando per l'inversa della matrice Projection.
	vec4 ray_eye = inverse(Projection) * ray_clip;

	// Direzione del raggio
	ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	// le coordinate nel world space: si ottengono premoltiplicando per l'inversa della matrice di Vista
	vec3 ray_wor = vec3(inverse(View) * ray_eye);
	ray_wor = normalize(ray_wor);

	return ray_wor;
}

// Controlla se un raggio interseca una sfera. In caso negativo, restituisce false. Rigetta
// Le intersezioni dietro l'origine del raggio, e pone  intersection_distance all'intersezione piu' vicina.

bool ray_sphere(vec3 ray_origin_wor, vec3 ray_direction_wor, vec3 sphere_centre_wor, float sphere_radius, float *intersection_distance)
{
	// Calcoliamo O - C
	vec3 dist_sfera = ray_origin_wor - sphere_centre_wor;
	float b = dot(dist_sfera, ray_direction_wor);
	float cc = dot(dist_sfera, dist_sfera) - sphere_radius * sphere_radius;
	float delta = b * b - cc;

	if (delta < 0.0f) // Il raggio non interseca la sfera
	{
		return false;
	}

	// Calcolo i valori di t per cui il raggio interseca la sfera e restituisco il valore dell'intersezione
	// piu' vicina all'osservatore (la t piu' piccola)
	if (delta > 0.0f)
	{
		// calcola le due intersezioni
		float t_a = -b + sqrt(delta);
		float t_b = -b - sqrt(delta);
		*intersection_distance = t_b;

		// Caso di intersezioni dietro l'osservatore
		if (t_a < 0.0f)
		{
			if (t_b < 0.0f)
			{
				return false;
			}
		}

		return true;
	}
	// Caso in cui il raggio sia tangente alla sfera: un interesezione con molteplicita' doppia.
	if (delta == 0)
	{
		float t = -b + sqrt(delta);
		
		if (t < 0)
		{
			return false;
		}

		*intersection_distance = t;
		return true;
	}

	return false;
}

void mouse(int button, int state, int x, int y)
{
	switch (button)
	{
	// Con il tasto sinistro premuto si attiva la modalita' di trackball
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			moving_trackball = true;
		}
		
		if (state == GLUT_UP)
		{
			moving_trackball = false;
		}

		last_mouse_pos_X = x;
		last_mouse_pos_Y = y;
		break;

	// Con il tasto destro si selezionano gli oggetti nella scena
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			float xmouse = x;
			float ymouse = y;
			vec3 ray_wor = get_ray_from_mouse(xmouse, ymouse);

			selected_obj = -1;
			float closest_intersection = 10000000.0f;
			for (int i = 0; i < Scena.size(); i++)
			{
				float t_dist = 0.0f;
				// Interseco il raggio che esce dalla camera nella direzione del mouse con la sfera centrata nell'ancora di tutti gli oggetti
				// posizionati nel mondo per individuare se c'e' intersezione con l'oggetto
				if (ray_sphere(ViewSetup.position, ray_wor, Scena[i].ancora_world, raggio_sfera, &t_dist))
				{
					if (selected_obj == -1 || t_dist < closest_intersection)
					{
						selected_obj = i;
						closest_intersection = t_dist;
					}
				}
			}

			if (selected_obj > -1)
				printf("Oggetto selezionato %d -> %s \n", selected_obj, Scena[selected_obj].nome.c_str());
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

vec3 getTrackBallPoint(float x, float y)
{
	// Dalla posizione del mouse al punto proiettato sulla semisfera con centro l'origine e raggio 1

	float Delta, tmp;
	vec3 point;
	// map to [-1;1]
	point.x = (2.0f * x - width) / width;
	point.y = (height - 2.0f * y) / height;

	// Cooordinata z del punto di coordinate (x,y,z) che si muove sulla sfera virtuale con centro (0,0,0) e raggio r=1
	tmp = pow(point.x, 2.0) - pow(point.y, 2.0);
	Delta = 1.0f - tmp;
	
	if (Delta > 0.0f)
		point.z = sqrt(Delta);
	else
		point.z = 0;

	return point;
}

void mouseActiveMotion(int x, int y)
{
	float pi = glm::pi<float>();
	float velocity = 180.0 / pi;
	// Spostamento tramite  trackball del vettore posizione Camera
	if (!moving_trackball) {
		return;
	}

	vec3 corrente = normalize(getTrackBallPoint(x, y));
	vec3 precedente = normalize(getTrackBallPoint(last_mouse_pos_X, last_mouse_pos_Y));
	vec3 rotation_vec = cross(precedente, corrente);

	//Calcoliamo il coseno dell'angolo tra il segmento che collega l'origine e il click corrente e il segmento
	//che collega l'origine ed il click corrente
	float cosangolo = dot(precedente, corrente);

	// Il prodotto scalare tra due vettori normalizzati che hanno lunghezza 1,
	// puo' variare tra -1 ed 1.
	// Per problemi numerici dovuti al fatto che lavoriamo con i numeri finiti, per evitare che acos
	// dia risulato nan, nel caso in cui cosangolo risulti maggiore di 1 rimettiamolo ad 1
	if (cosangolo > 1)
		cosangolo = 1.0;

	// nel caso in cui cosangolo risulti minore di -1 rimettiamolo a -1
	if (cosangolo < -1)
		cosangolo = -1.0;

	// Calcoliamo l'angolo utilizzando l'arcocoseno
	float theta = (acos(cosangolo)) * velocity;

	// Costruiamo il quaternione a partire dall'angolo di rotazione e dalla direzione lungo cui ruotare
	quat rotation = angleAxis(theta, rotation_vec);

	// Trasformiamo il quaternione in matrice di rotazione
	mat4 rotation_matrix = toMat4(rotation);

	// Aggiorniamo direzione e posizione della telecamera
	ViewSetup.direction = ViewSetup.position - ViewSetup.target;
	ViewSetup.position = ViewSetup.target + rotation_matrix * ViewSetup.direction;

	// Memorizzo l' ultima posizione del mouse
	last_mouse_pos_X = x;
	last_mouse_pos_Y = y;

	glutPostRedisplay();
}

void mouseActiveMotionTrack(int x, int y)
{
	float pi = glm::pi<float>();
	float velocity = 180.0 / pi;
	// Spostamento tramite  trackball del vettore posizione Camera
	if (!moving_trackball)
	{
		return;
	}

	vec3 corrente = normalize(getTrackBallPoint(x, y));
	vec3 precedente = normalize(getTrackBallPoint(last_mouse_pos_X, last_mouse_pos_Y));
	vec3 rotation_vec = cross(precedente, corrente);

	// Per evitare le instabilita' dovute ad acos, si puo' stimare l'angolo di rotazione
	// nel seguente modo:
	float theta = length(precedente - corrente) * (180.0 / pi);

	// Costruiamo il quaternione a partire dall'angolo di rotazione e dalla direzione lungo cui ruotare
	quat rotation = angleAxis(theta, rotation_vec);

	// Trasformiamo il quaternione in matrice di rotazione
	mat4 rotation_matrix = toMat4(rotation);

	// Aggiorniamo direzione e posizione della telecamera
	ViewSetup.direction = ViewSetup.position - ViewSetup.target;
	ViewSetup.position = ViewSetup.target + rotation_matrix * ViewSetup.direction;

	// Memorizzo l' ultima posizione del mouse
	last_mouse_pos_X = x;
	last_mouse_pos_Y = y;

	glutPostRedisplay();
}

void my_passive_mouse(int xpos, int ypos)
{
	// serve ridimensionare l'offset tra due posizioni successive del mouse
	float alfa = 0.05; 
	ypos = height - ypos;
	
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos;

	xoffset *= alfa;
	yoffset *= alfa;

	// Aggiorno l'angolo Theta sommandogli l'offset della posizione x del mouse
	Theta += xoffset;

	// Aggiorno l'angolo Phi sommandogli l'offset della posizione y del mouse
	Phi += yoffset;	  

	// Facciamo si' che l'angolo di Phi vari tra -90 e 90.
	if (Phi > 89.0f)
	{
		Phi = 89.0f;
	}

	if (Phi < -89.0f)
	{
		Phi = -89.0f;
	}

	// Ricavo le coordinate x,y,z del punto che sulla sfera individuato dagli angoli Theta e Phi
	vec3 front;

	front.x = cos(radians(Theta)) * cos(radians(Phi));
	front.y = sin(radians(Phi));
	front.z = sin(radians(Theta)) * cos(radians(Phi));

	// Considero la direzione normalizzata (nota la quarta componente uguale a 0 indica una direzione
	// Aggiorno la direzione della telecamera
	ViewSetup.direction = vec4(normalize(front), 0.0);		

	// Aggiorno il punto in cui guarda la telecamera	 
	ViewSetup.target = ViewSetup.position + ViewSetup.direction; 

	glutPostRedisplay();
}
