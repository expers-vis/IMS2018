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
#include <string>

#define MIN *1
#define HOD *60
#define DEN *60*24
#define HUSTOTA_REPKOVEHO_OLEJE 0.915

#define KAPACITA_RJ 5 //rafinacni jednotky, ks
#define SANCE_NA_KVALITNI_REPKU 0.995
#define SANCE_NA_SPATNY_STOLNI_OLEJ 0.00001
#define SANCE_NA_SPATNY_PANENSKY_OLEJ 0.00001


//deklarace zarizeni
Store VstupniKontrolaKvality("Kontrola kvality repky", 1);

Store Lis1("Lis prvniho stupne", 1);
Store Lis2("Lis druheho stupne", 1);

Store Filtr("Filtr", 3);

Store RafinacniJednotka("Rafinacni jednotka", 1);
Store VystupniKontrolaKvality("Kontrola kvality oleje", 1);

//deklarace statistik
Histogram Lis1Cekani("Cekaci doba na lisu prvniho stupne", 0, 25, 20);
Histogram Lis2Cekani("Cekaci doba na lisu druheho stupne", 0, 25, 20);

//deklarace globalnich zdroju
int extraPanenskyOlej = 0;
int stolniOlej = 0;
int vylisky = 0;

int frontaRafinacni = 0;

int intervalPrijezdu = 16 HOD;

bool hodinovyVypis = false;

//deklarace procesu

class ExtraPanenskyOlej : public Process {
	public: ExtraPanenskyOlej() : Process(2) { }

	void Behavior() {
		Enter(Filtr, 1);
		Wait(Uniform(10 MIN, 12 MIN));
		Leave(Filtr, 1);

		Enter(VystupniKontrolaKvality, 1);
		Wait(Uniform(10 MIN, 16 MIN));
		Leave(VystupniKontrolaKvality, 1);

		if (Random() <= SANCE_NA_SPATNY_PANENSKY_OLEJ) {
			//Print("Day: %03d, %02d:%02d : Spatny panensky olej\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60);
			Terminate();
		}

		extraPanenskyOlej += 100;
		//Print("Day: %03d, %02d:%02d : Panensky olej\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60);
	}
};

class StolniOlej : public Process { // 100 kg oleje
	public: StolniOlej() : Process(1) { }

	void Behavior() {

		Enter(VystupniKontrolaKvality, 1);
		Wait(Uniform(10 MIN, 16 MIN));
		Leave(VystupniKontrolaKvality, 1);


		if (Random() <= SANCE_NA_SPATNY_STOLNI_OLEJ) {
			//Print("Day: %03d, %02d:%02d : Spatny stolni olej\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60);
			Terminate();
		}

		stolniOlej += 100;
		//Print("Day: %03d, %02d:%02d : Stolni olej\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60);
	}
};

class PanenskyOlej : public Process {
	public : PanenskyOlej() : Process(0) { }

	void Behavior() {
		
		Enter(Filtr, 1);
		Wait(Uniform(10 MIN, 12 MIN));
		Leave(Filtr, 1);

		if (++frontaRafinacni != KAPACITA_RJ) {
			Terminate();
		}
		frontaRafinacni -= KAPACITA_RJ;

		Enter(RafinacniJednotka, 1);
		Wait(Uniform(55 MIN, 65 MIN)); //normal 60 5
		for(int i = 0; i < KAPACITA_RJ; i++) {
			(new StolniOlej)->Activate();
		}
		Leave(RafinacniJednotka, 1);

	}
};

class Vylisky : public Process {

	void Behavior() {
		vylisky += 700;
	}
};


class Repka : public Process {
	double Prichod;

	void Behavior() {

		Prichod = Time;
		Enter(Lis1, 1);
		Lis1Cekani(Time - Prichod);
		Wait(Uniform(29 MIN, 31 MIN));
		(new ExtraPanenskyOlej)->Activate();
		Leave(Lis1, 1);

		Prichod = Time;
		Enter(Lis2, 1);
		Lis2Cekani(Time - Prichod);
		Wait(Uniform(38 MIN, 40 MIN));
		(new PanenskyOlej)->Activate();
		(new PanenskyOlej)->Activate();
		(new Vylisky)->Activate();
		Leave(Lis2, 1);

	}
};

class Kamion : public Process {
public:
	void Behavior() {
		Enter(VstupniKontrolaKvality, 1); // kontrola kvality
		Wait(Uniform(25 MIN, 35 MIN));
		Leave(VstupniKontrolaKvality, 1);

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
		Activate(Time + intervalPrijezdu);
	}
};


class EveryHour : public Event 
{
	void Behavior() 
	{
		Print("Day: %03d, %02d:%02d : VstupniKontrolaKvality fronta: %d\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60 ,VstupniKontrolaKvality.QueueLen());
		Print("Day: %03d, %02d:%02d : Lis1 fronta: %d\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60 ,Lis1.QueueLen());
		Print("Day: %03d, %02d:%02d : Lis2 fronta: %d\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60 ,Lis2.QueueLen());
		Print("Day: %03d, %02d:%02d : Filtr fronta: %d\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60 ,Filtr.QueueLen());
		Print("Day: %03d, %02d:%02d : RafinacniJednotka fronta: %d\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60 ,RafinacniJednotka.QueueLen() + frontaRafinacni);
		Print("Day: %03d, %02d:%02d : VystupniKontrolaKvality fronta: %d\n" ,((int)Time/1440), ((int)Time/60)%24, ((int)Time)%60 ,VystupniKontrolaKvality.QueueLen());
		Activate(Time + 1 HOD); // aktivace za 1h
	}
};

int main(int argc, char *argv[]) {
	int c;
	std::string vystupniSoubor = "basic.out";

	while ((c = getopt (argc, argv, "F:1:2:o:hf:")) != -1) 
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
			case 'h':
				hodinovyVypis = true;
				break;
			case 'o':
				vystupniSoubor = optarg;
				break;
			case 'f':
				intervalPrijezdu = atoi(optarg);
				break;
			default:
				break;
		}
	}

	RandomSeed(time(NULL));
	SetOutput(vystupniSoubor.c_str());
	Print("Model vyroby repkoveho oleje\n");

	Init(0, 7 DEN);	// inicializace experimentu
	(new Generator)->Activate();	// aktivace generatoru pozadavku
	if ( hodinovyVypis ) {
		(new EveryHour)->Activate();
	}
	
	
	Run();	// simulace

	Print("Celkem vyrobeno:\n");
	Print("Stolni olej: %g l   (%d kg)\n", stolniOlej / HUSTOTA_REPKOVEHO_OLEJE, stolniOlej);
	Print("Panensky olej: %g l   (%d kg)\n", extraPanenskyOlej / HUSTOTA_REPKOVEHO_OLEJE, extraPanenskyOlej);
	Print("Vylisky: %d kg \n\n", vylisky);

	Print("Lis druheho stupne - fronta: \n");
	Lis1Cekani.Output();
	Print("Lis druheho stupne - fronta: \n");
	Lis2Cekani.Output();

	VstupniKontrolaKvality.Output();
	Lis1.Output();
	Lis2.Output();
	Filtr.Output();
	RafinacniJednotka.Output();
	VystupniKontrolaKvality.Output();
}