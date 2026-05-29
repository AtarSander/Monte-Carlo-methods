#ifndef RNG_H
#define RNG_H

/**
	@author Dominik Kasprowicz
   @version 2.1
   @file
*/

#include<random>

class RNG 
{
   /**
    * @brief Algorytm generacji liczb pseudolosowych.
    */
   std::mt19937_64 zrodlo_losowe;

   /**   
    * @brief Rozkład równomierny dyskretny, przydatny do losowania współrzędnych \e x lub \e y w obrębie siatki.
    */ 
   std::uniform_int_distribution<int> rozklad_lokalizacji;

    /**
     * @brief Rozkład równomierny ciągły z zakresu [0, 1).
     */
   std::uniform_real_distribution<float> rozklad_jednostajny_0_1;

public:
   /**
    * @brief Tworzy generator liczb pseudolosowych przeznaczony do współpracy z siatką kwadratową o zadanej wielkości.
    * Generator wykorzystuje algorytm Mersenne Twister 19937.
    * @param bok_siatki Długość boku kwadratowej siatki, z którą będzie współpracował generator. 
    */
   RNG(int bok_siatki); 

   /**   
    * @brief Losuje liczbę \a całkowitą o rozkładzie równomiernym na przedziale [0, bok_siatki - 1].
    * @return Wylosowana liczba.
    */ 
   int losuj_wspolrzedna();

   /**   
    * @brief Losuje liczbę \a rzeczywistą o rozkładzie równomiernym na przedziale [0, 1).
    * Metoda przydatna przy implementacji operacji, które mają zostać wykonane z określonym prawdopodobieństwem.
    * @return Wylosowana liczba.   
    */ 
   float losuj_z_zakresu_0_1();
};

#endif // RNG_H
