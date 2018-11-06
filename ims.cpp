/**
* @file 	ims.cpp
* @author	Roman Janik (xjanik20)
* @author	Karel Klecka(xkleck04)
* @date 	31.10.2018
* @brief	IMS projekt - Vyroba repkoveho oleje
*/

#include "simlib.h"
#include <time.h>

#define MIN *1
#define HOD *60
#define POCET_FILTRU 3

#define KAPACITA_LISU 1000 //kg
#define SANCE_NA_KVALITNI_REPKU 0.995

#define MIN_PROCENT_ODPADU 1
#define MAX_PROCENT_ODPADU 2



//deklarace zarizeni
Facility kontrolorKvality("kontrolorKvality");
Facility lis1Stupne("lis1Stupne");
Facility lis2Stupne("lis2Stupne");

Store Filtr("Filtr", POCET_FILTRU);

Store Lis1("Lis prvniho stupne", 1);
Store Lis2("Lis druheho stupne", 1);

Store RafinacniJednotka("Rafinacni jednotka", 1);

//deklarace statistik
Stat stPanenskyOlej;
Stat stStolniOlej;
Stat stVylisky;

//deklarace globalnich zdroju
double panenskyOlej = 0;
double stolniOlej = 0;
double vylisky = 0;

int frontaLis = 0;

//deklarace procesu

class PanenskyOlej : public Process {
	public: PanenskyOlej() : Process(1) { }

	void Behavior() {
		Enter(Filtr, 1);
		Wait( Uniform(10 MIN, 12 MIN));
		Leave(Filtr, 1);

		Seize(kontrolorKvality);
		Wait(Uniform(10 MIN, 20 MIN));
		Release(kontrolorKvality);
	}
};

class StolniOlej : public Process { // 100 l oleje
	public: StolniOlej() : Process(0) { }

	void Behavior() {
		Enter(Filtr, 1);
		Wait( Uniform(10 MIN, 12 MIN));
		Leave(Filtr, 1);

		Enter(RafinacniJednotka, 1);
		Wait( Uniform(10 MIN, 12 MIN));
		Leave(RafinacniJednotka, 1);

		Seize(kontrolorKvality);
		Wait(Uniform(10 MIN, 20 MIN));
		Release(kontrolorKvality);
	}
};

class Vylisky : public Process {

	void Behavior() {

	}
};


class Repka : public Process {
	void Behavior() {

	Enter(Lis1, 1);
	Wait( Normal(30, 1));
	(new PanenskyOlej)->Activate();
	Leave(Lis1, 1);

	Enter(Lis2, 1);
	Wait( Normal(40, 1));
	(new StolniOlej)->Activate();
	(new StolniOlej)->Activate();
	(new Vylisky)->Activate();
	Leave(Lis2, 1);

	}
};

class Kamion : public Process {
public:
	void Behavior() {
		Seize(kontrolorKvality); // kontrola kvality
		Wait(Uniform(25 MIN, 35 MIN));
		Release(kontrolorKvality);

		if (Random() <= SANCE_NA_KVALITNI_REPKU) // repka je v poradku //99.5
		{
			for (int i = 0; i < 25; i++)
				(new Repka)->Activate(); // 1 tuna repkoveho semene
		}
	}
};


//deklarace udalosti
class Generator : public Event {
	void Behavior() {
		(new Kamion)->Activate();
		Activate(Time + 8 HOD);
	}
};

int main() {
	RandomSeed(time(NULL));
	SetOutput("x1.out");
	Print("Model vyroby repkoveho oleje\n");

	Init(0, 8 HOD);	// inicializace experimentu
	(new Generator)->Activate();	// aktivace generatoru pozadavku
	
	Run();	// simulace

	stStolniOlej.Output();

	// tisk vysledku
	/*Print("Vydestilovano alkoholu celkem: ");
	Print(st_destilat.Sum());
	Print("\n");
	Print("Vydestilovano alkoholu prumerne: ");
	Print(st_destilat.MeanValue());
	Print("\n");
	Print("Vyprodukovano vypalku celkem: ");
	Print(st_vypalky.Sum());
	Print("\n");*/
}