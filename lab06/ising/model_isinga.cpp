#include "model_isinga.h"

#include <cmath>

ModelIsinga::ModelIsinga(int rozmiar, int energia) 
	: L{rozmiar}
	, energia_docelowa_ukladu{energia}
	, tryb_kanoniczny{false}
	, energia_poczatkowa_ukladu{0}
	, energia_duszka{0}
	, magnetyzacja{0}
	, srednia_energia_ukladu{0.0f}
	, srednia_energia_duszka{0.0f}
	, srednia_magnetyzacja{0.0f}
	, temperatura{0.0f}
	, generator{rozmiar} 
{
	siatka = new int* [L];
	for (int i = 0; i < L; i++)
		siatka[i] = new int[L];
}	

ModelIsinga::ModelIsinga(int rozmiar, float temp) 
	: L{rozmiar}
	, energia_docelowa_ukladu{0}
	, tryb_kanoniczny{true}
	, energia_poczatkowa_ukladu{0}
	, energia_duszka{0}
	, magnetyzacja{0}
	, srednia_energia_ukladu{0.0f}
	, srednia_energia_duszka{0.0f}
	, srednia_magnetyzacja{0.0f}
	, temperatura{temp}
	, generator{rozmiar}
{
	siatka = new int* [L];
	for (int i = 0; i < L; i++)
		siatka[i] = new int[L];
}

ModelIsinga::~ModelIsinga() 
{
	if (siatka)
	{
        for (int i = 0; i < L; i++)
            delete[] siatka[i];
        delete[] siatka;
	}
}

void ModelIsinga::ustaw_same_jedynki() 
{
	for(int i = 0; i < L; i++)
		for(int j = 0; j < L; j++)
			siatka[i][j] = 1;
}

int ModelIsinga::deltaE(int x, int y) 
{
	// Periodyczne warunki brzegowe.
	int gorny = (x != 0)   ? x-1 : L-1;
	int dolny = (x != L-1) ? x+1 : 0;
	int prawy = (y != L-1) ? y+1 : 0;
	int lewy  = (y != 0)   ? y-1 : L-1;

	int E_pocz = siatka[x][y] * ( siatka[x][lewy] + siatka[x][prawy] + siatka[gorny][y] + siatka[dolny][y] );
	int E_konc = -E_pocz;

	return E_pocz - E_konc;
}

void ModelIsinga::sprobuj_odwrocic_spin_losowego_atomu() 
{
	int i = generator.losuj_wspolrzedna();
	int j = generator.losuj_wspolrzedna();
	int dE = deltaE(i, j);

	if (tryb_kanoniczny)
	{
		if (dE <= 0 || generator.losuj_z_zakresu_0_1() < std::exp(-static_cast<float>(dE) / temperatura))
		{
			siatka[i][j] = -siatka[i][j];
			energia_poczatkowa_ukladu += dE;
			magnetyzacja += 2*siatka[i][j];
		}
	}
	else if (dE <= energia_duszka) 
	{				
		siatka[i][j] = -siatka[i][j];
		energia_duszka -= dE;
		energia_poczatkowa_ukladu  += dE;			
		magnetyzacja += 2*siatka[i][j];
	}
}

void ModelIsinga::doprowadzenie_do_stanu_rownowagi(int liczba_krokow) 
{
	magnetyzacja = L * L;
	energia_poczatkowa_ukladu  = -2 * L * L;
	energia_duszka = tryb_kanoniczny ? 0 : energia_docelowa_ukladu - energia_poczatkowa_ukladu;
	ustaw_same_jedynki();

	for (int k = 0; k < liczba_krokow; ++k) 
	{
		sprobuj_odwrocic_spin_losowego_atomu();
	}
}

// Wyznacza średnią energię duszka, energię wewnętrzną układu, magnetyzację oraz temperaturę
void ModelIsinga::zliczanie_srednich(int liczba_krokow) 
{
	int energia_duszka_do_sredniej = 0;
	int energia_ukladu_do_sredniej = 0;	
	int magnetyzacja_tot = 0;
	
	// Pętla liczby_kroków
	for (int k = 0; k < liczba_krokow; ++k) 
	{
		// Pętla statystycznie po każdym spinie
		for (int l = 0; l < L*L; ++l) 
		{
			sprobuj_odwrocic_spin_losowego_atomu();
		}
		energia_duszka_do_sredniej += energia_duszka;
		energia_ukladu_do_sredniej += energia_poczatkowa_ukladu;
		magnetyzacja_tot += abs(magnetyzacja);
	}

	// Obliczanie średnich
	srednia_energia_duszka = (float)energia_duszka_do_sredniej / liczba_krokow;
	srednia_energia_ukladu = (float)energia_ukladu_do_sredniej / liczba_krokow;
	srednia_magnetyzacja = magnetyzacja_tot / (float)liczba_krokow / ((float)L*(float)L);
	if (tryb_kanoniczny)
	{
		srednia_energia_duszka = 0.0f;
	}
	else
	{
		temperatura = 4.0 / (log(1 + 4.0/srednia_energia_duszka));
	}
}

float ModelIsinga::podaj_srednia_energie_duszka() 
{
	return srednia_energia_duszka;
}

float ModelIsinga::podaj_srednia_energie_ukladu() 
{
	return srednia_energia_ukladu;
}

float ModelIsinga::podaj_srednia_magnetyzacje() 
{
	return srednia_magnetyzacja;
}

float ModelIsinga::podaj_temperature() 
{
	return temperatura;
}
