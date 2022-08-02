#include <stdio.h>
#include <math.h>

#define VELKOST 90

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
	
	//tento smernik je mimo pola a je na porovnane, ci mi fukncia vrati null, len pomocna premenna
	void* nulovySmernik = ((char*)smernik - 400*sizeof(int));
	
	//fukncia mi vrati adresu vhodneho bloku na alokaciu
	void* volnyBlok = najdiVolnyBlok(smernik, size, pomocnaPaticka,nulovySmernik);
	int volnyBlokVelkost = *(int*)volnyBlok;
	
	//ak nie je dostatok miesta na alokaciu, vrati null
	if (volnyBlok == nulovySmernik)
	{
		printf("malo miesta na alokaciu malloc\n");
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
		//skontrolujem ci bol malakovany, teda ma zapornu hodnotu
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


int main()
{
	char region[VELKOST];
	memory_init(region, VELKOST);
	
	int* ptr1 = memory_alloc(8);
	int* ptr2 = memory_alloc(6);
	memory_free(ptr2);
	

	printf("hodnota smernika: %d\n", *(int*)(smernik));
	printf("adresa smernika: %d\n\n", smernik);
	printf("hodnota paticky: %d\n", *(int*)(smernik+8+4+4+4+61));
	printf("adresa paticky: %d\n\n", (char*)(smernik+8+4+4+4+61));
	
	printf("%c ", *(int*)(smernik-4-1));
	printf("%d ", *(int*)(smernik-4));
	
	printf("%d ", *(int*)(smernik));
	printf("%d ", *(int*)(smernik+8+4));
	
	printf("%d ", *(int*)(smernik+8+4+4));
	printf("%d ", *(int*)(smernik+8+4+4+4+61));
	
	
	return 0;
}



