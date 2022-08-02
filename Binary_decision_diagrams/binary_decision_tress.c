#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define tabulka_hodnoty1 "0000\n0010\n0100\n0110\n1000\n1100\n1111\n1010\n"			    
#define tabulka_hodnoty2 "000\n011\n101\n111\n"				    
#define tabulka_hodnoty3 "000000\n000011\n000101\n000111\n001001\n001011\n001101\n001111\n010001\n010011\n010101\n010111\n011001\n"\
						 "011011\n011101\n011111\n000011\n000111\n001011\n001111\n010011\n010111\n011011\n011111\n100011\n100111\n101011\n101111\n110011\n110111\n111011\n111111\n"
#define tabulka_hodnoty4 "00000\n11111\n00010\n11101\n00100\n11011\n01000\n10111\n10000\n01111\n00111\n01101\n11001\n10101\n01011\n10011\n"	
#define tabulka_hodnoty5 "0000000\n0000010\n0000100\n0000110\n0001000\n0001010\n0001100\n0001111\n0010000\n0010010\n0010100\n0010111\n0011000\n0011011\n0011100\n0011111\n0100000\n0100010\n0100100\n0101000\n0101101\n0101111\n0110011\n0110111\n0111001\n0111011\n0111101\n0111111\n1000000\n1000010\n"\
						"1000100\n1000111\n1001011\n1001111\n1010011\n1010101\n1010111\n1011001\n1011101\n1100000\n1100011\n1100101\n1100111\n1101001\n1101011\n1101101\n1101111\n1110001\n1110011\n1110101\n1110111\n1111011\n1111111\n"
				    
//moja struktura pre uzol					    
typedef struct node{
	int hodnota;
	//char *id;
	struct node *zero;
	struct node *one;
	struct node *previous;
}Node;

//struktura pre bdd
typedef struct BDDs {
	int velkost_BDD;
	int pocet_premennych;
	Node *koren;
} BDD;

//bf struktura na opis booleovskej funkcie
typedef struct BFs {
	char *tabulka;
	int pocet_premennych;
} BF;


//pomocna funkcia na zistenie poctu premenny v pravdivostnej tabulke
//funkcia je volana z funkcie BDD_create
int zisti_pocet_premennych(char* split_tabulka, int velkost_tabulky)
{
	int premenne = 0;
	int i;
	
	for (i = 0; i < velkost_tabulky; i++)
	{
		if (split_tabulka[i] != '\n')
		{
			premenne++;
		}
		else
		{
			break;
		}
	}
	premenne--;
	//printf("tu premenne: %d\n", premenne);
	return premenne;
}

//pomocna funckia pre vytvorenie uzla
//funkcia je volana z funkcie BDD_create
Node* vytvor_node()
{
	Node* vysledok = (Node*)malloc(sizeof(Node));
	vysledok->one = NULL;
	vysledok->zero = NULL;
	vysledok->hodnota = -1;
	return vysledok;
}

// prechadzam postupne od korena, ak neexistuje vytvorim node ak existuje iba tade prechadzam
// vratit ukazovatel na hotovy BDD
BDD *BDD_create(BF *bfunkcia)
{ 
	int i, j, cislo;
	int premenne = 0;

    char *split_tabulka = bfunkcia->tabulka;
    int velkost_tabulky = strlen(split_tabulka);
    premenne = zisti_pocet_premennych(split_tabulka, velkost_tabulky);
 	printf("pocet premennych: %d\n", premenne);
 	
 	
	BDD* vysledok = (BDD*)malloc(sizeof(BDD));
	vysledok->koren = vytvor_node();
	vysledok->pocet_premennych = premenne;
	Node* aktualny = vysledok->koren;
	//printf("%p\n", aktualny);	
	
    for (i = 0; i < velkost_tabulky; i++) 
	{
		//printf("%c\n", split_tabulka[i]);
		
		if ((i+1) < velkost_tabulky && split_tabulka[i+1] == '\n' )
		{
			continue;
		}
		
		if (split_tabulka[i] == '\n')
		{
			aktualny->hodnota = (split_tabulka[i-1] == '0') ? 0 : 1;
			aktualny = vysledok->koren;
			//printf("%p\n", aktualny);
		}
		
		if (split_tabulka[i] == '0')
		{
			if (aktualny->zero == NULL)
			{
				aktualny->zero = vytvor_node();
				aktualny->zero= aktualny;
			}
			aktualny = aktualny->zero;
		
		}
		
		if (split_tabulka[i] == '1')
		{
			if (aktualny->one == NULL)
			{
				aktualny->one = vytvor_node();
				aktualny->one->previous = aktualny;
			}
			aktualny = aktualny->one;
		}
 	}
	return vysledok;
}

//vrati hodnotu, ktora bola pridelena danej postupnosti v BDD_create
char BDD_use(BDD *bdd, char *vstupy)
{ 
	int i;
	int vysledna_hodnota = -1;
	int dlzka_vstupy = strlen(vstupy);

	Node* aktualny = bdd->koren;
	
    for (i = 0; i < dlzka_vstupy; i++) 
	{
		//printf("%c\n", vstupy[i]);
		
		if (vstupy[i] == '0')
		{
			if (aktualny->zero == NULL)
			{
				break;
			}
			aktualny = aktualny->zero;
		}
		
		if (vstupy[i] == '1')
		{
			if (aktualny->one == NULL)
			{
				break;
			}
			aktualny = aktualny->one;
		}
		
 	}
 	vysledna_hodnota = aktualny->hodnota;
 	//printf("tu vypise hodnotu: %d\n", vysledna_hodnota);
	
	return (vysledna_hodnota == 1) ? '1' : '0'; 
}


int BDD_reduce_node(Node *aktualny, int odstranene_uzly)
{
	if (aktualny->zero != NULL)
	{
		BDD_reduce_node(aktualny->zero, odstranene_uzly);
	}
	
	if (aktualny->one != NULL)
	{
		BDD_reduce_node(aktualny->one, odstranene_uzly);
	}
	
	if (aktualny->zero != NULL && aktualny->one != NULL && aktualny->zero->hodnota == aktualny->one->hodnota)
	{
		aktualny->previous = aktualny->zero;
		odstranene_uzly++;
		free(aktualny->one);
		free(aktualny);
	}
	return odstranene_uzly;	
}


int BDD_reduce(BDD *bdd)
{
	Node *aktualny = bdd->koren;
	return BDD_reduce_node(aktualny, 0);
}


int main(int argc, char *argv[]) 
{
	BDD* bdd;
	BF tabulka;
	tabulka.tabulka = tabulka_hodnoty1;
	bdd = BDD_create(&tabulka);  
	printf("BDD je vytvoreny pre tabulku:\n");
	printf(tabulka_hodnoty1);

	//char *vstupy = "111"; 
	char use = BDD_use(bdd, "111");
	printf("\nTestovanie pre BDD_use: 111\n");
	if (use == '1')
	{
		printf("Pre A = 1, B = 1, C = 1 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n"); 
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 1, B = 1, C = 1 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
	}
	
	
	use = BDD_use(bdd, "000");
	printf("\n\nTestovanie pre BDD_use: 000\n");
	if (use == '0')
	{ 
		printf("Pre A = 0, B = 0, C = 0 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n");
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 0, B = 0, C = 0 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
	}
	
	use = BDD_use(bdd, "010");
	printf("\n\nTestovanie pre BDD_use: 010\n");
	if (use == '0')
	{ 
		printf("Pre A = 0, B = 1, C = 0 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n");
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 0, B = 1, C = 0 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
	}
	
	use = BDD_use(bdd, "110");
	printf("\n\nTestovanie pre BDD_use: 110\n");
	if (use == '0')
	{ 
		printf("Pre A = 1, B = 1, C = 0 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n");
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 1, B = 1, C = 0 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
	}
	
	use = BDD_use(bdd, "001");
	printf("\n\nTestovanie pre BDD_use: 001\n");
	if (use == '0')
	{ 
		printf("Pre A = 0, B = 0, C = 1 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n");
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 0, B = 0, C = 1 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
	}
	printf("-------------------------------------------------\n\n");

	tabulka.tabulka = NULL	;
	tabulka.tabulka = tabulka_hodnoty2;
	bdd = BDD_create(&tabulka);  
	printf("BDD je vytvoreny pre tabulku:\n");
	printf(tabulka_hodnoty2);

	use = BDD_use(bdd, "11");
	printf("\nTestovanie pre BDD_use: 11\n");
	if (use == '1')
	{
		printf("Pre A = 1, B = 1 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n"); 
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 1, B = 1 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
	}
	
	use = BDD_use(bdd, "00");
	printf("\n\nTestovanie pre BDD_use: 00\n");
	if (use == '0')
	{ 
		printf("Pre A = 0, B = 0 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n");
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 0, B = 0 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
	}
	
	use = BDD_use(bdd, "10");
	printf("\n\nTestovanie pre BDD_use: 10\n");
	if (use == '1')
	{ 
		printf("Pre A = 1, B = 0 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n");
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 1, B = 0 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
	}
	printf("-------------------------------------------------\n\n");
	
	tabulka.tabulka = NULL;
	tabulka.tabulka = tabulka_hodnoty3;
	bdd = BDD_create(&tabulka);  
	printf("BDD je vytvoreny pre tabulku:\n");
	printf(tabulka_hodnoty3);

	use = BDD_use(bdd, "1111");
	printf("\nTestovanie pre BDD_use: 11111\n");
	if (use == '1')
	{
		printf("Pre A = 1, B = 1, C = 1, D = 1, E = 1 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n"); 
	}
	else
	{
		printf("Pre  A = 1, B = 1, C = 1, D = 1, E = 1 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
	}
	
	use = BDD_use(bdd, "00000");
	printf("\nTestovanie pre BDD_use: 00000\n");
	if (use == '0')
	{
		printf("Pre A = 0, B = 0, C = 0, D = 0, E= 0 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n"); 
	}
	else
	{
		printf("Pre A = 0, B = 0, C = 0, D = 0, E= 0 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
	}

	
	use = BDD_use(bdd, "111");
	printf("\nTestovanie pre BDD_use: 111\n");
	if (use == '1')
	{
		printf("Pre A = 1, B = 1, C = 1 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n"); 
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 1, B = 1, C = 1 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
	}
	printf("-------------------------------------------------");
	
	//NAHODNE GENEROVANE VSTUPY PRE 3 PREMENNE
    int a = rand() % 2;
    int b = rand() % 2;
    int c = rand() % 2;
    
    char s1[20];
    char s2[20];
    char s3[20];
    char s4[20];
    char s5[20];
    char s6[20];
    
    sprintf(s1, "%d", a);
    sprintf(s2, "%d", b);
    sprintf(s3, "%d", c);
    
    strcat(s1, s3);
    int vysledok1 = atoi(s1);
    strcat(s1, s2);
    int vysledok2 = atoi(s1);
	
	char* str = malloc(1000);
	char ocakavane;
	int cakaneCislo = -1;
	snprintf(str, 1000, "%d", vysledok2);
	printf("\n\nTestovanie pre nahodne vygenerovane cislo pre 3 premenne\n");
	printf("Nahodne Vygenerovany string: %s\n", str);
  
  	if (a == 1 && b == 1 && c == 0)
  	{
  		ocakavane = '1';
  		cakaneCislo = 1;
	}
	else 
	{
		ocakavane = '0';
		cakaneCislo = 0;
	}
	
	use = BDD_use(bdd, str);
	printf("Testovanie pre BDD_use: %s\n",str);
	if (use == ocakavane)
	{ 
		printf("Pre A = %d, B = %d, C = %d ocakava hodnotu: %d\nA vratil hodnotu: %c\n",a, c ,b, cakaneCislo, use);
		printf("BDD_use funguje SPRAVNE\n");
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = %d, B = %d, C = %d ocakava hodnotu: %d\nA vratil hodnotu: %c\n",a, c ,b, cakaneCislo, use);
	}


	printf("-------------------------------------------------\n\n");

	tabulka.tabulka = NULL	;
	tabulka.tabulka = tabulka_hodnoty4;
	bdd = BDD_create(&tabulka);  
	printf("BDD je vytvoreny pre tabulku:\n");
	printf(tabulka_hodnoty4);

	use = BDD_use(bdd, "1111");
	printf("\nTestovanie pre BDD_use: 1111\n");
	if (use == '1')
	{
		printf("Pre A = 1, B = 1, C = 1, D = 1 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n"); 
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 1, B = 1, C = 1, D = 1 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
	}	
	
	use = BDD_use(bdd, "0000");
	printf("\nTestovanie pre BDD_use: 0000\n");
	if (use == '0')
	{
		printf("Pre A = 0, B = 0, C = 0, D = 0 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n"); 
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 0, B = 0, C = 0, D = 0 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
	}	
	
	use = BDD_use(bdd, "0110");
	printf("\nTestovanie pre BDD_use: 0110\n");
	if (use == '1')
	{
		printf("Pre A = 0, B = 1, C = 1, D = 0 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n"); 
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 0, B = 1, C = 1, D = 0 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
	}	
	
	use = BDD_use(bdd, "1000");
	printf("\nTestovanie pre BDD_use: 1000\n");
	if (use == '0')
	{
		printf("Pre A = 1, B = 0, C = 0, D = 0 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n"); 
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 1, B = 0, C = 0, D = 0 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
	}
	
	use = BDD_use(bdd, "0111");
	printf("\nTestovanie pre BDD_use: 0111\n");
	if (use == '1')
	{
		printf("Pre A = 0, B = 1, C = 1, D = 1 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n"); 
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 0, B = 1, C = 1, D = 1 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
	}
	
	printf("-------------------------------------------------\n\n");

	tabulka.tabulka = NULL	;
	tabulka.tabulka = tabulka_hodnoty5;
	bdd = BDD_create(&tabulka);  
	printf("BDD je vytvoreny pre tabulku:\n");
	printf(tabulka_hodnoty5);

	use = BDD_use(bdd, "111111");
	printf("\nTestovanie pre BDD_use: 111111\n");
	if (use == '1')
	{
		printf("Pre A = 1, B = 1, C = 1, D = 1, E = 1, F = 1 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n"); 
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 1, B = 1, C = 1, D = 1, E = 1, F = 1 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
	}	
	
	
	use = BDD_use(bdd, "000000");
	printf("\nTestovanie pre BDD_use: 000000\n");
	if (use == '0')
	{
		printf("Pre A = 0, B = 0, C = 0, D = 0, E = 0, F = 0 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n"); 
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 0, B = 0, C = 0, D = 0, E = 0, F = 0 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
	}
	
	use = BDD_use(bdd, "01110");
	printf("\nTestovanie pre BDD_use: 011100\n");
	if (use == '1')
	{
		printf("Pre A = 0, B = 1, C = 1, D = 1, E = 0, F = 0 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n"); 
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 0, B = 1, C = 1, D = 1, E = 0, F = 0 ocakava hodnotu: 1\nA vratil hodnotu: %c\n", use);
	}
	
	use = BDD_use(bdd, "000001");
	printf("\nTestovanie pre BDD_use: 000001\n");
	if (use == '0')
	{
		printf("Pre A = 0, B = 0, C = 0, D = 0, E = 0, F = 1 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
		printf("BDD_use funguje SPRAVNE\n"); 
	}
	else
	{
		printf("ERROR\nBDD_use funguje NESPRAVNE\n"); 
		printf("Pre A = 0, B = 0, C = 0, D = 0, E = 0, F = 1 ocakava hodnotu: 0\nA vratil hodnotu: %c\n", use);
	}

/*
	int reduce = BDD_reduce(bdd);
	printf("Odstranila som %d uzlov\n", reduce);
*/	
	return 0;
}

