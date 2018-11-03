/**
* @file 	ims.cpp
* @author	Roman Janik (xjanik20)
* @author	Karel Klecka(xkleck04)
* @date 	31.10.2018
* @brief	IMS projekt - Vyroba repkoveho oleje
*/

#include "simlib.h"

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

//deklarace statistik
Stat stPanenskyOlej;
Stat stStolniOlej;
Stat stVylisky;

//deklarace globalnich zdroju
double panenskyOlej = 0;
double stolniOlej = 0;
double vylisky = 0;

int frontaLis = 0;

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

		if (Random() <= SANCE_NA_KVALITNI_REPKU) // repka je v poradku //99.5
		{
			for (int i = 0; i < 25; i++)
				(new Repka)->Activate(); // 1 tuna repkoveho semene
		}
	}
};


class Repka : public Process {
public:
  double mnozstvi = 1000; //kg
	void Behavior() {
		// mam problem s tim odpadem 2%, kdyz Repka ma byt 1 tuna
    // může to vygenerovat nový proces odpadu
    // třízení by mělo zabrat čas?

    double mnozstviOdpadu = Uniform(10,20);
    mnozstvi -= mnozstviOdpadu;

    frontaLis += mnozstvi;

    if (frontaLis > KAPACITA_LISU) {
      frontaLis -= KAPACITA_LISU;


      Enter(Lis1, 1);
      Wait( Normal(30, 1));
      (new PanenskyOlej)->Activate();
      Leave(Lis1, 1);

      Enter(Lis2, 1);
      Wait( Normal(40, 1));
      (new Olej)->Activate();
      Leave(Lis2, 1);
    }

	}
};

class Olej : public Process { // 100 l oleje
  public: Olej() : Process(0) { }

  void Behavior() {
    Enter(Filtr, 1);
    Wait( Uniform(10-12));
    Leave(Filtr, 1);
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

	// tisk vysledku
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