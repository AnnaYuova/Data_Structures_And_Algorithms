#include <stdio.h>
#include <math.h>

#define VELKOST 2000

void* smernik;

//fukncia vrati adresu vhodnej hlavicky na alokaciu
void* najdiVolnyBlok(void* pomocnySmernik, unsigned int velkost,void* pomocnaPaticka,void* nulovySmernik)
{
	void* aktualnyBlok = pomocnySmernik; 
	int hodnotaBloku = *(int*)aktualnyBlok; 
	
	//ak nie je dostatok miesta, vrati adresu nuloveho smernika
	if (aktualnyBlok > pomocnaPaticka)
	{
		return nulovySmernik;
	}
	else if (hodnotaBloku < 0)
	{
		//ak je miesto uz alokovane, posunie sa na dalsiu hlavicku a rekurzivne sa zavola
		aktualnyBlok += abs(hodnotaBloku) + 2*sizeof(int);
		return (najdiVolnyBlok(aktualnyBlok, velkost, pomocnaPaticka, nulovySmernik));
	}
	else if ( (hodnotaBloku > 0) && (hodnotaBloku < (velkost + 2*sizeof(int))) ) 
	{
		//ak miesto nie je obsadene ale nevyhovuje velkost, posunie sa na dalsiu hlavicku a zavola rekurziu
		aktualnyBlok += hodnotaBloku + 2*sizeof(int);
		return (najdiVolnyBlok(aktualnyBlok, velkost, pomocnaPaticka,nulovySmernik));
	}
	else if ( (hodnotaBloku > 0) && (hodnotaBloku > (velkost + 2*sizeof(int))) )
	{
		//ak je miesto volne a vyhovuje
		return aktualnyBlok;
	}
}


void *memory_alloc(unsigned int size)
{	
	//adresa poslednej paticky, aby som vedela, kde konci moje pole
	void* pomocnaPaticka = (char*)smernik + *(int*)(smernik-4) -(3*sizeof(int) + sizeof(char)) + sizeof(int);
	
	//tento smernik je mimo pola a je na porovnane, ci mi fukncia vrati null, (len pomocna premenna)
	void* nulovySmernik = ((char*)smernik - 400*sizeof(int));
	
	//fukncia mi vrati adresu vhodneho bloku na alokaciu
	void* volnyBlok = najdiVolnyBlok(smernik, size, pomocnaPaticka, nulovySmernik);
	int volnyBlokVelkost = *(int*)volnyBlok;
	
	//ak nie je dostatok miesta na alokaciu, vrati null
	if (volnyBlok == nulovySmernik)
	{
		printf("MALO MIESTA NA MALLOC\n");
		return NULL;
	}
	
	//vypocitam si, kde sa bude nachadzat nasledujuca hlavicka a jej hodnotu
	void* dalsiaHlavicka = (char*)volnyBlok + size + 2*sizeof(int);
	int dalsiaHlavickaVelkost = volnyBlokVelkost - 2*sizeof(int) - size;
	*(int*)dalsiaHlavicka = dalsiaHlavickaVelkost;
	
	//vypocitam si, kde sa bude nachadzat nasledujuca paticka a jej hodnotu
	void* dalsiaPaticka = (char*)dalsiaHlavicka + dalsiaHlavickaVelkost + sizeof(int);
	int dalsiaPatickaVelkost = volnyBlokVelkost - 2*sizeof(int) - size;
	*(int*)dalsiaPaticka = dalsiaPatickaVelkost;
	
	//do hlavicky si vlozim adresu volneho miesta, ktore idem alokovat a hodnotu zmenim na zapornu
	void* hlavicka = volnyBlok;
	int hlavickaVelkost = 0 - size;
	*(int*)hlavicka = hlavickaVelkost;
			
	//do hlavicky si vlozim adresu volneho miesta posunutu na paticku, ktore idem alokovat a hodnotu zmenim na zapornu
	void* paticka = (char*)volnyBlok + size + sizeof(int);
	int patickaVelkosti = 0 - size;
	*(int*)paticka = patickaVelkosti;
	
	return volnyBlok;
}


int memory_free(void *valid_ptr)
{
	void* aktualnySmernik = valid_ptr;
	int aktualnySmernikHodnota = *(int*)aktualnySmernik;
	
	//ak je hodnota kladna, nie je co uvolnit
	if (aktualnySmernikHodnota > 0)
	{
		return 1;
	}
	
	//ak je hodnota zaporna
	if (aktualnySmernikHodnota < 0) 
	{
		//hodnotu hlavicky zmeni na jej absolutnu hodnotu
		*(int*)aktualnySmernik = abs(aktualnySmernikHodnota);
		
		//posunie sa paticku
		aktualnySmernik = (char*)aktualnySmernik + abs(aktualnySmernikHodnota)+ sizeof(int);
		
		//hodnotu paticky zmeni na jej absolutnu hodnotu 
		*(int*)aktualnySmernik = abs(aktualnySmernikHodnota);
	}
	
	
	//pomocne vypocty pre kontrolu dolava
	int hodnotaPatickyPovodnaDolava = *(int*)aktualnySmernik;
	void* adresaPatickyDolava = aktualnySmernik - *(int*)aktualnySmernik - 2*sizeof(int);
	int hodnotaPatickyDolava = *(int*)adresaPatickyDolava;
	
	//skontrolujem ci je paticka nalavo kladna a ak ano tak ich spojim 
	//najprv sa pozriem ci nie som mimo pola
	if (adresaPatickyDolava >= smernik)
	{
		//skontrolujem ci je hodnota paticky nalavo kladna
		if (hodnotaPatickyDolava > 0) // ak je nalavo plusova hodnota tak spajam
		{
			//velkost noveho bloku volneho po spojeni
			int spocitanieVelkostiDolava = hodnotaPatickyDolava + *(int*)aktualnySmernik + 2 * sizeof(int); 

			//posuniem sa z paticky na hlavicku a do nej dam novu hodnotu
			aktualnySmernik = (char*)aktualnySmernik- *(int*)aktualnySmernik - 3*sizeof(int) - hodnotaPatickyDolava; 
			*(int*)aktualnySmernik = spocitanieVelkostiDolava; 

			//posuniem sa z hlavicky na paticku a do nej dam novu hodnotu
			aktualnySmernik = (char*)aktualnySmernik + *(int*)aktualnySmernik + sizeof(int); 
			*(int*)aktualnySmernik = spocitanieVelkostiDolava; 

			//vynulujem povodnu hlavicku
			aktualnySmernik = (char*)aktualnySmernik - sizeof(int) - hodnotaPatickyPovodnaDolava; 
			*(int*)aktualnySmernik = 0;
			
			//vynulujem povodnu paticku
			aktualnySmernik = (char*)aktualnySmernik - sizeof(int); 
			*(int*)aktualnySmernik = 0; //vynulujem
			
			//posuniem sa na novu paticku - jedinu co teraz mam
			aktualnySmernik = (char*)aktualnySmernik + 2*sizeof(int) +hodnotaPatickyPovodnaDolava;
		}
	}
	
	//pomocne vypocty pre kontrolu doprava
	int hodnotaPatickyPovodnaDoprava = *(int*)aktualnySmernik;
	void* adresaHlavickyDoprava = (char*)aktualnySmernik + sizeof(int);
	int hodnotaHlavickyDoprava = *(int*)adresaHlavickyDoprava;
	
	//skontrolujem ci je hodnota paticky nalavo kladna a ak ano spojim
	if (hodnotaHlavickyDoprava > 0) 
	{
		//velkost noveho bloku volneho po spojeni
		int spocitanieVelkosti = *(int*)aktualnySmernik + hodnotaHlavickyDoprava + 2*sizeof(int); 
		
		//posuniem sa z paticky na hlavicku a dam do nej novu hodnotu
		aktualnySmernik = (char*)aktualnySmernik - *(int*)aktualnySmernik - sizeof(int); 
		*(int*)aktualnySmernik = spocitanieVelkosti; 
		
		//posuniem sa z hlavicky na paticku a dam do nej novu hodnotu
		aktualnySmernik = (char*)aktualnySmernik + *(int*)aktualnySmernik + sizeof(int); 
		*(int*)aktualnySmernik = spocitanieVelkosti; 

		//vynulujem povodnu hlavicku
		aktualnySmernik = (char*)aktualnySmernik - sizeof(int) - hodnotaHlavickyDoprava; 
		*(int*)aktualnySmernik = 0; 
		
		//vynulujem povodnu paticku
		aktualnySmernik = (char*)aktualnySmernik - sizeof(int); 
		*(int*)aktualnySmernik = 0; 
		
		//znovu sa posuniem na novu paticku
		aktualnySmernik = (char*)aktualnySmernik + 2*sizeof(int) + hodnotaHlavickyDoprava;
	}
	return 0;
}

int memory_check(void *ptr)
{
	void* hladanySmernik = ptr;
	int hladanySmernikHodnota = *(int*)hladanySmernik;
	
	//skontrolujem ci je smernik v rozsahu pola
	if ( (hladanySmernik >= smernik) && (hladanySmernik <= ((char*)smernik + *(int*)(smernik-4) -(3*sizeof(int) + sizeof(char)) + sizeof(int))) )
	{
		//skontrolujem ci bol malakovany a este nebol uvolneny, teda ma zapornu hodnotu 
		if ( hladanySmernikHodnota < 0 )
		{
			return 1;
		}
		else //ak je v poli ale bol uvolneny
		{
			return 0;
		}
	}
	else //ak nie je z rozsahu mojho pola
	{
		printf("Mimo rozsahu mojho pola\n");
		return 0;
	}
}

void memory_init(void *ptr, unsigned int size)
{
	//smernik = ukazovatel na zaciatok pola
	smernik = ptr;	
	
	//prvy znak v poli, aby som vedela, kde moje pole zacina					
	*(char*)(smernik) = '+';		
	
	//posuniem sa vedla a vlozim velkost pola
	*(int*)((char*)smernik + sizeof(char)) = size;	
	
	//smernik ukazuje na adresu pola, s ktorou budem pracovat
	smernik =((char*)smernik + sizeof(char)+ sizeof(int));
			printf("adresa smernika v init: %d    ", smernik);
	
	//do hlavicky vlozim kolko volneho miesta je na alokaciu		
	*(int*)smernik = size - 3*sizeof(int) - sizeof(char); //hlavicka = 77
	int hodnotaSmernik = *(int*)smernik;
	
	//do paticky tiez vlozim kolko volneho miesta mam na alokaciu
	*(int*)((char*)smernik + hodnotaSmernik + sizeof(int)) = size - 3*sizeof(int) - sizeof(char); //paticka = 77
			printf("adresa paticka v init: %d \n", (char*)smernik + hodnotaSmernik + sizeof(int));
}


//testovacia fukncia kontroluje ci je zoznam hodnot na danych indexoch
//funkcia volana z main
//vypise vysledne alokovane pole
void vypis(int *pole_indexov, int pocet)
{
	int i;
	
	printf("\nTakto vyzera konecne alokovane pole\n\n");
	printf("%c ", *(int*)(smernik-4-1));
	printf("%d ", *(int*)(smernik-4));
	
	for (i =  0; i < pocet; i++)
	{
		int pomocna = pole_indexov[i];
		printf("%d ", *(int*)(smernik+pomocna)); 
	}
}


int main()
{
	char region[VELKOST];
	memory_init(region, VELKOST);
	
	//TESTOVANIE
	
	//region[2000]
	
	int* ptr1 = memory_alloc(650);
	int* ptr2 = memory_alloc(1000);
	memory_free(ptr2);
	int* ptr3 = memory_alloc(300);
	memory_free(ptr1);
	int* ptr4 = memory_alloc(250);
	printf("hodnota smernika: %d\n", *(int*)(smernik));
	printf("adresa smernika: %d\n\n", smernik);
	printf("hodnota paticky: %d\n", *(int*)(smernik+1991));
	printf("adresa paticky: %d\n", (int*)(smernik+1991));
	printf("Adresy a hodnoty hlavicky a paticky sa zhoduju\n\n");
	
	int pole_indexov[15]= {0, 254, 258, 654, 658, 962, 966, 1991};
	int pocet = 8;
	vypis(pole_indexov, pocet);
	
	
	
	/*
	//region[90]
	
	int* ptr1 = memory_alloc(5);
	printf("hodnota smernika: %d\n", *(int*)(smernik));
	printf("adresa smernika: %d\n\n", smernik);
	printf("hodnota paticky: %d\n", *(int*)(smernik+5+4+4+64+4));
	printf("adresa paticky: %d\n", (int*)(smernik+5+4+4+64+4));
	printf("Adresy a hodnoty hlavicky a paticky sa zhoduju\n\n");
	
	int pole_indexov[10]= {0, 9, 13, 81};
	int pocet = 4;
	vypis(pole_indexov, pocet);
	*/
	
	
	/*
	//region[100]
	
	int* ptr1 = memory_alloc(22);
	int* ptr2 = memory_alloc(22);
	printf("hodnota smernika: %d\n", *(int*)(smernik));
	printf("adresa smernika: %d\n\n", smernik);
	printf("hodnota paticky: %d\n", *(int*)(smernik+22+4+4+22+4+4+4+27));
	printf("adresa paticky: %d\n", (int*)(smernik+22+4+4+22+4+4+4+27));
	printf("Adresy a hodnoty hlavicky a paticky sa zhoduju\n\n");
	
	
	int pole_indexov[10]= {0, 26, 30, 56, 60, 91};
	int pocet =6;
	vypis(pole_indexov, pocet);
	*/
	
	
	/*
	//region[200]
	
	int* ptr1 = memory_alloc(17);
	int* ptr2 = memory_alloc(17);
	int* ptr3 = memory_alloc(17);
	int* ptr4 = memory_alloc(17);
	printf("hodnota smernika: %d\n", *(int*)(smernik));
	printf("adresa smernika: %d\n\n", smernik);
	printf("hodnota paticky: %d\n", *(int*)(smernik+191));
	printf("adresa paticky: %d\n", (int*)(smernik+191));
	printf("Adresy a hodnoty hlavicky a paticky sa zhoduju\n\n");
	
	
	int pole_indexov[15]= {0, 21, 25, 46, 50, 71, 75, 96, 100, 191};
	int pocet = 10;
	vypis(pole_indexov, pocet);
	*/
	
	
	/*
	//region[50]
	
	int* ptr1 = memory_alloc(8);
	int* ptr2 = memory_alloc(10);
	printf("hodnota smernika: %d\n", *(int*)(smernik));
	printf("adresa smernika: %d\n\n", smernik);
	printf("hodnota paticky: %d\n", *(int*)(smernik+8+4+4+10+4+4+3+4));
	printf("adresa paticky: %d\n", (int*)(smernik+8+4+4+10+4+4+3+4));
	printf("Adresy a hodnoty hlavicky a paticky sa zhoduju\n\n");
	
	int pole_indexov[15]= {0, 12, 16, 30, 34, 41};
	int pocet = 6;
	vypis(pole_indexov, pocet); 
	*/
	
	
	/*
	//region[100]
	
	int* ptr1 = memory_alloc(12);
	int* ptr2 = memory_alloc(15);
	int* ptr3 = memory_alloc(22);
	printf("hodnota smernika: %d\n", *(int*)(smernik));
	printf("adresa smernika: %d\n\n", smernik);
	printf("hodnota paticky: %d\n", *(int*)(smernik+12+4+4+15+4+4+22+4+4+14+4));
	printf("adresa paticky: %d\n", (int*)(smernik+12+4+4+15+4+4+22+4+4+14+4));
	printf("Adresy a hodnoty hlavicky a paticky sa zhoduju\n\n");
	
	int pole_indexov[15]= {0, 16, 20, 39, 43, 69, 73, 91};
	int pocet = 8;
	vypis(pole_indexov, pocet); 
	*/
	
	
	/*
	//region[200]
	
	int* ptr1 = memory_alloc(20);
	int* ptr2 = memory_alloc(24);
	int* ptr3 = memory_alloc(10);
	printf("hodnota smernika: %d\n", *(int*)(smernik));
	printf("adresa smernika: %d\n\n", smernik);
	printf("hodnota paticky: %d\n", *(int*)(smernik+20+4+4+24+4+4+10+4+4+109+4));
	printf("adresa paticky: %d\n", (int*)(smernik+20+4+4+24+4+4+10+4+4+109+4));
	printf("Adresy a hodnoty hlavicky a paticky sa zhoduju\n\n");
	
	int pole_indexov[15]= {0, 24, 28, 56, 60, 74, 78, 191};
	int pocet = 8;
	vypis(pole_indexov, pocet);
	*/
	
	/*
	//region[1000]
	
	int* ptr1 = memory_alloc(600);
	int* ptr2 = memory_alloc(50);
	printf("hodnota smernika: %d\n", *(int*)(smernik));
	printf("adresa smernika: %d\n\n", smernik);
	printf("hodnota paticky: %d\n", *(int*)(smernik+600+4+4+50+4+4+321+4));
	printf("adresa paticky: %d\n", (int*)(smernik+600+4+4+50+4+4+321+4));
	printf("Adresy a hodnoty hlavicky a paticky sa zhoduju\n\n");
	
	int pole_indexov[15]= {0, 604, 608, 662, 666, 991};
	int pocet = 6;
	vypis(pole_indexov, pocet);
	*/
	
	
	/*
	//region[4000]
	
	int* ptr1 = memory_alloc(2000);
	int* ptr2 = memory_alloc(800);
	int* ptr3 = memory_alloc(560);
	printf("hodnota smernika: %d\n", *(int*)(smernik));
	printf("adresa smernika: %d\n\n", smernik);
	printf("hodnota paticky: %d\n", *(int*)(smernik+3991));
	printf("adresa paticky: %d\n", (int*)(smernik+3991));
	printf("Adresy a hodnoty hlavicky a paticky sa zhoduju\n\n");
	
	int pole_indexov[15]= {0, 2004, 2008, 2812, 2816, 3380, 3384, 3991};
	int pocet = 8;
	vypis(pole_indexov, pocet);
	*/
	
	
	/*
	//region[9000]
	
	int* ptr1 = memory_alloc(4500);
	int* ptr2 = memory_alloc(900);
	int* ptr3 = memory_alloc(1200);
	int* ptr4 = memory_alloc(2300);
	printf("hodnota smernika: %d\n", *(int*)(smernik));
	printf("adresa smernika: %d\n\n", smernik);
	printf("hodnota paticky: %d\n", *(int*)(smernik+8991));
	printf("adresa paticky: %d\n", (int*)(smernik+8991));
	printf("Adresy a hodnoty hlavicky a paticky sa zhoduju\n\n");
	
	int pole_indexov[15]= {0, 4504, 4508, 5412, 5416, 6620, 6624, 8928, 8932, 8991};
	int pocet = 10;
	vypis(pole_indexov, pocet);
	*/
	
	
	/*
	//region[20000]
	
	int* ptr1 = memory_alloc(6000);
	int* ptr2 = memory_alloc(10000);
	int* ptr3 = memory_alloc(3500);
	printf("hodnota smernika: %d\n", *(int*)(smernik));
	printf("adresa smernika: %d\n\n", smernik);
	printf("hodnota paticky: %d\n", *(int*)(smernik+19991));
	printf("adresa paticky: %d\n", (int*)(smernik+19991));
	printf("Adresy a hodnoty hlavicky a paticky sa zhoduju\n\n");
	
	int pole_indexov[15]= {0, 6004, 6008, 16012, 16016, 19520, 19524, 19991};
	int pocet = 8;
	vypis(pole_indexov, pocet);
	*/
	
	
	/*
	//region[45000]
	
	int* ptr1 = memory_alloc(28000);
	int* ptr2 = memory_alloc(15000);
	int* ptr3 = memory_alloc(1900);
	printf("hodnota smernika: %d\n", *(int*)(smernik));
	printf("adresa smernika: %d\n\n", smernik);
	printf("hodnota paticky: %d\n", *(int*)(smernik+44991));
	printf("adresa paticky: %d\n", (int*)(smernik+44991));
	printf("Adresy a hodnoty hlavicky a paticky sa zhoduju\n\n");
	
	int pole_indexov[15]= {0, 28004, 28008, 43012, 43016, 44920, 44924, 44991};
	int pocet = 8;
	vypis(pole_indexov, pocet);
	*/
	
	
	/*
	//region[90]
	
	int* ptr1 = memory_alloc(8);
	int* ptr2 = memory_alloc(6);
	memory_free(ptr1);
	memory_free(ptr2);
	printf("hodnota smernika: %d\n", *(int*)(smernik));
	printf("adresa smernika: %d\n\n", smernik);
	printf("hodnota paticky: %d\n", *(int*)(smernik+77+4));
	printf("adresa paticky: %d\n", (int*)(smernik+77+4));
	printf("Adresy a hodnoty hlavicky a paticky sa zhoduju\n\n");
	
	int pole_indexov[15]= {0, 81};
	int pocet = 2;
	vypis(pole_indexov, pocet);
	*/
	
	
	/*
	//region[300]
	
	int* ptr1 = memory_alloc(40);
	int* ptr2 = memory_alloc(15);
	memory_free(ptr2);
	printf("hodnota smernika: %d\n", *(int*)(smernik));
	printf("adresa smernika: %d\n\n", smernik);
	printf("hodnota paticky: %d\n", *(int*)(smernik+40+4+239+4+4));
	printf("adresa paticky: %d\n", (int*)(smernik+40+4+239+4+4));
	printf("Adresy a hodnoty hlavicky a paticky sa zhoduju\n\n");
	
	int pole_indexov[15]= {0, 44, 48, 291};
	int pocet = 4;
	vypis(pole_indexov, pocet);
	*/
	
	
	/*
	//region[300]
	
	int* ptr1 = memory_alloc(40);
	int* ptr2 = memory_alloc(15);
	memory_free(ptr2);
	int* ptr3 = memory_alloc(70);
	printf("hodnota smernika: %d\n", *(int*)(smernik));
	printf("adresa smernika: %d\n\n", smernik);
	printf("hodnota paticky: %d\n", *(int*)(smernik+40+4+4+70+4+4+161+4));
	printf("adresa paticky: %d\n", (int*)(smernik+40+4+4+70+4+4+161+4));
	printf("Adresy a hodnoty hlavicky a paticky sa zhoduju\n\n");
	
	int pole_indexov[15]= {0, 44, 48, 122, 126, 291};
	int pocet = 6;
	vypis(pole_indexov, pocet);
	*/
	
	
	/*
	//region[90]
	
	int* ptr1 = memory_alloc(8);
	int* ptr2 = memory_alloc(6);
	int* ptr3 = (char*)smernik - 50*sizeof(int); //adresa je naschval v rozsahu mimo pola - vypise to 
	memory_free(ptr2);
	memory_check(ptr2);
	memory_check(ptr3);
	printf("hodnota smernika: %d\n", *(int*)(smernik));
	printf("adresa smernika: %d\n\n", smernik);
	printf("hodnota paticky: %d\n", *(int*)(smernik+12+4+61+4));
	printf("adresa paticky: %d\n", (int*)(smernik+12+4+61+4));
	printf("Adresy a hodnoty hlavicky a paticky sa zhoduju\n\n");
	
	int pole_indexov[15]= {0, 12, 16, 81};
	int pocet = 4;
	vypis(pole_indexov, pocet);
	*/
	
	return 0;
}
