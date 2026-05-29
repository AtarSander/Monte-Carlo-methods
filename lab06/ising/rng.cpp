#include "rng.h"

RNG::RNG(int bok_siatki) 
	: zrodlo_losowe{26052023}
	, rozklad_lokalizacji{0, bok_siatki-1}
	, rozklad_jednostajny_0_1{0, 1}
{ }

int RNG::losuj_wspolrzedna()   { return rozklad_lokalizacji(zrodlo_losowe); }

float RNG::losuj_z_zakresu_0_1() { return rozklad_jednostajny_0_1(zrodlo_losowe); }
