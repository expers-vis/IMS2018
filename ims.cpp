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
#define POCET_FILTRU 1

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

		stPanenskyOlej(100);
        Print("Day: %03d, %02d:%02d : Panensky olej\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60);
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

		stStolniOlej(100);
        Print("Day: %03d, %02d:%02d : Stolni olej\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60);
	}
};

class Vylisky : public Process {

	void Behavior() {
		stVylisky(700);
	}
};


class Repka : public Process {
	void Behavior() {

	Enter(Lis1, 1);
	Wait( Normal(29 MIN, 1 MIN));
	(new PanenskyOlej)->Activate();
	Leave(Lis1, 1);

	Enter(Lis2, 1);
	Wait( Normal(38 MIN, 1 MIN));
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


class EveryHour : public Event 
{
    void Behavior() 
    {
        Print("Day: %03d, %02d:%02d : Stolni olej\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60);
        Print("Day: %03d, %02d:%02d : Lis1 fronta: %d\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60 ,Lis1.QueueLen());
        Print("Day: %03d, %02d:%02d : Lis2 fronta: %d\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60 ,Lis2.QueueLen());
        Print("Day: %03d, %02d:%02d : Filtr fronta: %d\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60 ,Filtr.QueueLen());
        Print("Day: %03d, %02d:%02d : RafinacniJednotka fronta: %d\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60 ,RafinacniJednotka.QueueLen());
        Activate(Time+60); // aktivace za 1h
    }
};

int main() {
	RandomSeed(time(NULL));
	SetOutput("x1.out");
	Print("Model vyroby repkoveho oleje\n");

	Init(0, 80 HOD);	// inicializace experimentu
	(new Generator)->Activate();	// aktivace generatoru pozadavku
    (new EveryHour)->Activate();
	
	Run();	// simulace

	Print("Stolni olej: \n");
	stStolniOlej.Output();
	Print("Panensky olej: \n");
	stPanenskyOlej.Output();
	Print("Vylisky: \n");
	stVylisky.Output();

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