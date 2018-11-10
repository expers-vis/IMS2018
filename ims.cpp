/**
* @file 	ims.cpp
* @author	Roman Janik (xjanik20)
* @author	Karel Klecka (xkleck04)
* @date 	31.10.2018
* @brief	IMS projekt - Vyroba repkoveho oleje
*/

#include "simlib.h"
#include <time.h>

#include <unistd.h>

#define MIN *1
#define HOD *60

#define POCET_FILTRU 3

#define KAPACITA_LISU 1000 //kg
#define SANCE_NA_KVALITNI_REPKU 0.995
#define SANCE_NA_SPATNY_STOLNI_OLEJ 0.00001
#define SANCE_NA_SPATNY_PANENSKY_OLEJ 0.00001



//deklarace zarizeni
Store VstupniKontrolorKvality("KontrolorKvality", 1);

Store Lis1("Lis prvniho stupne", 1);
Store Lis2("Lis druheho stupne", 1);

Store Filtr("Filtr", POCET_FILTRU);

Store RafinacniJednotka("Rafinacni jednotka", 1);
Store VystupniKontrolorKvality("Kontrolor kvality oleje", 1);

//deklarace statistik
Stat stPanenskyOlej;
Stat stStolniOlej;
Stat stVylisky;

Histogram Lis1Cekani("Cekaci doba na lisu prvniho stupne", 0, 25, 20);
Histogram Lis2Cekani("Cekaci doba na lisu druheho stupne", 0, 25, 20);

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

		Enter(VystupniKontrolorKvality, 1);
		Wait(Uniform(10 MIN, 15 MIN));
		Leave(VystupniKontrolorKvality, 1);

		if ( Random() <= SANCE_NA_SPATNY_PANENSKY_OLEJ) {
			Print("Day: %03d, %02d:%02d : Spatny panensky olej\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60);
			Terminate();
		}

		stPanenskyOlej(100);
		Print("Day: %03d, %02d:%02d : Panensky olej\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60);
	}
};

class StolniOlej : public Process { // 100 kg oleje
	public: StolniOlej() : Process(0) { }

	void Behavior() {
		Enter(Filtr, 1);
		Wait( Uniform(10 MIN, 12 MIN));
		Leave(Filtr, 1);

		Enter(RafinacniJednotka, 1);
		Wait( Uniform(10 MIN, 12 MIN));
		Leave(RafinacniJednotka, 1);

		Enter(VystupniKontrolorKvality, 1);
		Wait(Uniform(10 MIN, 15 MIN));
		Leave(VystupniKontrolorKvality, 1);


		if ( Random() <= SANCE_NA_SPATNY_STOLNI_OLEJ) {
			Print("Day: %03d, %02d:%02d : Spatny stolni olej\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60);
			Terminate();
		}

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
	double Prichod;

	void Behavior() {

		Prichod = Time;
		Enter(Lis1, 1);
		Lis1Cekani(Time - Prichod);
		Wait( Normal(25 MIN, 1 MIN));
		(new PanenskyOlej)->Activate();
		Leave(Lis1, 1);

		Prichod = Time;
		Enter(Lis2, 1);
		Lis2Cekani(Time - Prichod);
		Wait( Normal(35 MIN, 1 MIN));
		(new StolniOlej)->Activate();
		(new StolniOlej)->Activate();
		(new Vylisky)->Activate();
		Leave(Lis2, 1);

	}
};

class Kamion : public Process {
public:
	void Behavior() {
		Enter(VstupniKontrolorKvality, 1); // kontrola kvality
		Wait(Uniform(25 MIN, 35 MIN));
		Leave(VstupniKontrolorKvality, 1);

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
		Activate(Time + 16 HOD);
	}
};


class EveryHour : public Event 
{
	void Behavior() 
	{
		Print("Day: %03d, %02d:%02d : VstupniKontrolorKvality fronta: %d\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60 ,VstupniKontrolorKvality.QueueLen());
		Print("Day: %03d, %02d:%02d : Lis1 fronta: %d\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60 ,Lis1.QueueLen());
		Print("Day: %03d, %02d:%02d : Lis2 fronta: %d\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60 ,Lis2.QueueLen());
		Print("Day: %03d, %02d:%02d : Filtr fronta: %d\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60 ,Filtr.QueueLen());
		Print("Day: %03d, %02d:%02d : RafinacniJednotka fronta: %d\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60 ,RafinacniJednotka.QueueLen());
		Print("Day: %03d, %02d:%02d : VystupniKontrolorKvality fronta: %d\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60 ,VystupniKontrolorKvality.QueueLen());
		Activate(Time + 1 HOD); // aktivace za 1h
	}
};

int main(int argc, char *argv[]) {
	int c;
	while ((c = getopt (argc, argv, "F:1:2:v:")) != -1) 
	{
		switch (c)
		{
			case 'F':
				Filtr.SetCapacity(atoi(optarg));
				break;
			case '1':
				Lis1.SetCapacity(atoi(optarg));
				break;
			case '2':
				Lis2.SetCapacity(atoi(optarg));
				break;
			default:
				break;
		}
	}

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

	Print("Lis druheho stupne - fronta: \n");
	Lis1Cekani.Output();
	Print("Lis druheho stupne - fronta: \n");
	Lis2Cekani.Output();

	Lis1.Output();

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