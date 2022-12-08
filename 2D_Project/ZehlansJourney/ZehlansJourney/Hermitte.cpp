#include "Hermitte.h"
int pVal = 140;

float dx(int i, float *t, float Tens, float Bias, float Cont, Shape *Fig)
{

	if (i == 0)
	{
		return 0.5 * (1 - Tens) * (1 - Bias) *
			   (1 - Cont) * (Fig->controlPoints[i + 1].x - Fig->controlPoints[i].x) /
			   (t[i + 1] - t[i]);
	}

	if (i == Fig->controlPoints.size() - 1)
	{
		return 0.5 * (1 - Tens) * (1 - Bias) *
			   (1 - Cont) * (Fig->controlPoints[i].x - Fig->controlPoints[i - 1].x) /
			   (t[i] - t[i - 1]);
	}

	if (i % 2 == 0)
	{
		return 0.5 * (1 - Tens) * (1 + Bias) * (1 + Cont) *
				   (Fig->controlPoints.at(i).x - Fig->controlPoints.at(i - 1).x) /
				   (t[i] - t[i - 1]) +
			   0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) *
				   (Fig->controlPoints.at(i + 1).x - Fig->controlPoints.at(i).x) /
				   (t[i + 1] - t[i]);
	}
	else
	{
		return 0.5 * (1 - Tens) * (1 + Bias) * (1 - Cont) *
				   (Fig->controlPoints.at(i).x - Fig->controlPoints.at(i - 1).x) /
				   (t[i] - t[i - 1]) +
			   0.5 * (1 - Tens) * (1 - Bias) * (1 + Cont) *
				   (Fig->controlPoints.at(i + 1).x - Fig->controlPoints.at(i).x) /
				   (t[i + 1] - t[i]);
	}
}

float dy(int i, float *t, float Tens, float Bias, float Cont, Shape *Fig)
{

	if (i == 0)
	{
		return 0.5 * (1 - Tens) * (1 - Bias) *
			   (1 - Cont) * (Fig->controlPoints[i + 1].y - Fig->controlPoints[i].y) /
			   (t[i + 1] - t[i]);
	}

	if (i == Fig->controlPoints.size() - 1)
	{
		return 0.5 * (1 - Tens) * (1 - Bias) *
			   (1 - Cont) * (Fig->controlPoints[i].y - Fig->controlPoints[i - 1].y) /
			   (t[i] - t[i - 1]);
	}

	if (i % 2 == 0)
	{
		return 0.5 * (1 - Tens) * (1 + Bias) *
				   (1 + Cont) * (Fig->controlPoints.at(i).y - Fig->controlPoints.at(i - 1).y) /
				   (t[i] - t[i - 1]) +
			   0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) *
				   (Fig->controlPoints.at(i + 1).y - Fig->controlPoints.at(i).y) /
				   (t[i + 1] - t[i]);
	}
	else
	{
		return 0.5 * (1 - Tens) * (1 + Bias) *
				   (1 - Cont) * (Fig->controlPoints.at(i).y - Fig->controlPoints.at(i - 1).y) /
				   (t[i] - t[i - 1]) +
			   0.5 * (1 - Tens) * (1 - Bias) * (1 + Cont) *
				   (Fig->controlPoints.at(i + 1).y - Fig->controlPoints.at(i).y) /
				   (t[i + 1] - t[i]);
	}
}

void InterpolazioneHermitte(float *t, Shape *Fig, vec4 color_top, vec4 color_bot)
{
	float passotg = 1.0 / (pVal - 1);
	float tg, tgmapp, ampiezza, x, y;
	int i;
	int is = 0;

	// per ogni valore di tg vado a valutare le sue componenti x e y della curva interpolante
	for (tg = 0; tg <= 1; tg += passotg)
	{
		// individuo l'intervallo [t_is,t_is+1] a cui tg appartiene
		if (tg > t[is + 1])
		{
			is++;
		}

		ampiezza = t[is + 1] - t[is];

		// mappo tg nell'intervallo [0,1]
		tgmapp = (tg - t[is]) / ampiezza;

		x = Fig->controlPoints[is].x * PHI0(tgmapp) + dx(is, t, 0.0, 0.0, 0.0, Fig) * PHI1(tgmapp) * ampiezza + Fig->controlPoints[is + 1].x * PSI0(tgmapp) + dx(is + 1, t, 0.0, 0.0, 0.0, Fig) * PSI1(tgmapp) * ampiezza;
		y = Fig->controlPoints[is].y * PHI0(tgmapp) + dy(is, t, 0.0, 0.0, 0.0, Fig) * PHI1(tgmapp) * ampiezza + Fig->controlPoints[is + 1].y * PSI0(tgmapp) + dy(is + 1, t, 0.0, 0.0, 0.0, Fig) * PSI1(tgmapp) * ampiezza;

		Fig->vertices.push_back(vec3(x, y, 0.0));
		Fig->colors.push_back(color_top);
	}
}