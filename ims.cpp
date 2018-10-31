/**
* @file 	ims.cpp
* @author	Roman Janik (xjanik20)
* @date 	31.10.2018
* @brief	IMS projekt - Vyroba repkoveho oleje
*/

#include "simlib.h"

#define MIN *1
#define HOD *60
#define POCET_FILTRU 3


//deklarace zarizeni
Facility kontrolorKvality("kontrolorKvality");
Facility lis1Stupne("lis1Stupne");
Facility lis2Stupne("lis2Stupne");

Store Filtr("Filtr", POCET_FILTRU);

//deklarace statistik
Stat stPanenskyOlej;
Stat stStolniOlej;
Stat stVylisky;

//deklarace globalnich zdroju
double panenskyOlej = 0;
double stolniOlej = 0;
double vylisky = 0;

//deklarace udalosti
class Generator : public Event {
	void Behavior() {
		(new Kamion)->Activate();
		Activate(Time + 8 HOD);
	}
};

//deklarace procesu
class Kamion : public Process {
public:
	void Behavior() {
		Seize(kontrolorKvality); // kontrola kvality
		Wait(Uniform(25 MIN, 35 MIN));
		Release(kontrolorKvality);

		double rnd = Random();
		if (rnd <= 99.5) // repka je v poradku
		{
			for (int i = 0; i < 25; i++)
				(new Repka)->Activate(); // 1 tuna repkoveho semene
		}
	}
};


class Repka : public Process {
public:
	void Behavior() {
		// mam problem s tim odpadem 2%, kdyz Repka ma byt 1 tuna
	}
};

// sablona
class Vzor : public Process {
public:
	void Behavior() {

	}
};

int main() {
	SetOutput("x1.out");
	Print("Model vyroby repkoveho oleje\n");

	Init(0, 8 HOD);	// inicializace experimentu
	(new Generator)->Activate();	// aktivace generatoru pozadavku
	
	Run();	// simulace

	Print("Vydestilovano alkoholu celkem: ");
	Print(st_destilat.Sum());
	Print("\n");
	Print("Vydestilovano alkoholu prumerne: ");
	Print(st_destilat.MeanValue());
	Print("\n");
	Print("Vyprodukovano vypalku celkem: ");
	Print(st_vypalky.Sum());
	Print("\n");
}
