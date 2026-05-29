#include <iostream>
#include "model_isinga.h"

using namespace std;

/** 
 * @file
 * Główna funkcja programu. 
 * Tworzy model próbki ferromagnetycznej o danej wielkości, wprowadza do niego 
 * zadaną ilość energii (układ mikrokanoniczny) i oblicza statystyki 
 * (temperatura, magnetyzacja itp.)
 * Obecna postać ma charakter czysto demonstracyjny, 
 * ponieważ uwzględnia tylko jedną wielkość siatki i jedną wartość energii docelowej.
 * <b>
 * Przy realizacji zadań z instrukcji należy użyć zagnieżdżonej pętli po wielkościach
 * siatki i energiach docelowych (Zadanie 1 – układ mikrokanoniczny), względnie po
 * wielkościach siatki i temperaturach (Zadanie 2 – układ kanoniczny).
 * </b>
 */
int main(int argc, char *argv[])
{
	cout << "Symulacja modelu Isinga w Zespole Mikrokanonicznym" << endl;
	
	ModelIsinga ising(10, -184);
	ising.doprowadzenie_do_stanu_rownowagi(1000);
	ising.zliczanie_srednich(1000);

	cout << "Srednia energia ukladu = " << ising.podaj_srednia_energie_ukladu() << endl;
	cout << "Srednia energia duszka = " << ising.podaj_srednia_energie_duszka() << endl;
	cout << "Srednia magnetyzacja   = " << ising.podaj_srednia_magnetyzacje() << endl;
	cout << "Temperatura            = " << ising.podaj_temperature() << endl;
	
	return 0;
}
